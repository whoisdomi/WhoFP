#!/usr/bin/env python3
import os
from openpilot.system.hardware import TICI
os.environ['DEV'] = 'QCOM' if TICI else 'CPU'
USBGPU = "USBGPU" in os.environ
if USBGPU:
  os.environ['DEV'] = 'AMD'
  os.environ['AMD_IFACE'] = 'USB'
from tinygrad.tensor import Tensor
from tinygrad.dtype import dtypes
import time
import pickle
import numpy as np
import cereal.messaging as messaging
from cereal import car, log
from pathlib import Path
from cereal.messaging import PubMaster, SubMaster
from msgq.visionipc import VisionIpcClient, VisionStreamType, VisionBuf
from opendbc.car.car_helpers import get_demo_car_params
from openpilot.common.swaglog import cloudlog
from openpilot.common.params import Params
from openpilot.common.filter_simple import FirstOrderFilter
from openpilot.common.realtime import config_realtime_process, DT_MDL
from openpilot.common.transformations.camera import DEVICE_CAMERAS
from openpilot.common.transformations.model import get_warp_matrix
from openpilot.selfdrive.controls.lib.desire_helper import DesireHelper
from openpilot.selfdrive.controls.lib.drive_helpers import get_accel_from_plan, smooth_value, get_curvature_from_plan
from openpilot.selfdrive.modeld.parse_model_outputs import Parser
from openpilot.selfdrive.modeld.fill_model_msg import fill_model_msg, fill_pose_msg, PublishState
from openpilot.selfdrive.modeld.constants import ModelConstants, Plan
from openpilot.selfdrive.modeld.models.commonmodel_pyx import DrivingModelFrame, CLContext
from openpilot.selfdrive.modeld.runners.tinygrad_helpers import qcom_tensor_from_opencl_address

from openpilot.frogpilot.common.frogpilot_variables import get_frogpilot_toggles, MODELS_PATH


PROCESS_NAME = "selfdrive.modeld.modeld"
SEND_RAW_PRED = os.getenv('SEND_RAW_PRED')

# Default model paths (used for bd2/default model)
VISION_PKL_PATH = Path(__file__).parent / 'models/driving_vision_tinygrad.pkl'
POLICY_PKL_PATH = Path(__file__).parent / 'models/driving_policy_tinygrad.pkl'
VISION_METADATA_PATH = Path(__file__).parent / 'models/driving_vision_metadata.pkl'
POLICY_METADATA_PATH = Path(__file__).parent / 'models/driving_policy_metadata.pkl'
OFF_POLICY_PKL_PATH = Path(__file__).parent / 'models/driving_off_policy_tinygrad.pkl'
OFF_POLICY_METADATA_PATH = Path(__file__).parent / 'models/driving_off_policy_metadata.pkl'

LAT_SMOOTH_SECONDS = 0.1
LONG_SMOOTH_SECONDS = 0.3
MIN_LAT_CONTROL_SPEED = 0.3


def get_action_from_model(model_output: dict[str, np.ndarray], prev_action: log.ModelDataV2.Action,
                          lat_action_t: float, long_action_t: float, v_ego: float, desire: int = 0,
                          frogpilot_toggles = None) -> log.ModelDataV2.Action:
    plan = model_output['plan'][0]
    desired_accel, should_stop = get_accel_from_plan(plan[:,Plan.VELOCITY][:,0],
                                                     plan[:,Plan.ACCELERATION][:,0],
                                                     ModelConstants.T_IDXS,
                                                     action_t=long_action_t)
    desired_accel = smooth_value(desired_accel, prev_action.desiredAcceleration, LONG_SMOOTH_SECONDS)

    desired_curvature = get_curvature_from_plan(plan[:,Plan.T_FROM_CURRENT_EULER][:,2],
                                                plan[:,Plan.ORIENTATION_RATE][:,2],
                                                ModelConstants.T_IDXS,
                                                v_ego,
                                                lat_action_t)

    # Advanced Turn Desires (ATD)
    atd_enabled = frogpilot_toggles and frogpilot_toggles.advanced_turn_desires
    is_turning = desire in (log.Desire.turnLeft, log.Desire.turnRight)

    # Initialize post-turn timer
    if not hasattr(get_action_from_model, 'post_turn_timer'):
      get_action_from_model.post_turn_timer = 0.0

    # Apply ATD logic if enabled
    if atd_enabled:
      # Get ATD parameters from frogpilot_toggles
      turn_lat_smooth = frogpilot_toggles.turn_lat_smooth
      turn_left_bias_percent = frogpilot_toggles.turn_left_bias_percent
      turn_right_bias_percent = frogpilot_toggles.turn_right_bias_percent
      post_turn_smoothing_time = frogpilot_toggles.post_turn_smoothing_time

      # Update post-turn timer
      if is_turning:
        get_action_from_model.post_turn_timer = post_turn_smoothing_time
      elif get_action_from_model.post_turn_timer > 0:
        get_action_from_model.post_turn_timer -= DT_MDL

      # Apply curvature bias ONLY during active turn
      if is_turning:
        if desire == log.Desire.turnLeft:
          bias_percent = turn_left_bias_percent
        else:  # turnRight
          bias_percent = turn_right_bias_percent
        curvature_bias = desired_curvature * (bias_percent / 100.0)
        desired_curvature += curvature_bias

      # Use fast smoothing during turn AND for post-turn period
      if is_turning or get_action_from_model.post_turn_timer > 0:
        lat_smooth = turn_lat_smooth
      else:
        lat_smooth = frogpilot_toggles.latSmoothSeconds if frogpilot_toggles else LAT_SMOOTH_SECONDS
    else:
      lat_smooth = frogpilot_toggles.latSmoothSeconds if frogpilot_toggles else LAT_SMOOTH_SECONDS

    # Apply lane position offset as curvature correction
    # Positive offset = shift right = need negative curvature (steer right)
    if frogpilot_toggles and v_ego > MIN_LAT_CONTROL_SPEED:
      lane_offset = getattr(frogpilot_toggles, 'lane_position_offset', 0.0)
      if lane_offset != 0:
        # Scale correction by velocity - smaller correction at higher speeds for stability
        # At 10m/s with 0.1m offset: -0.003 curvature (noticeable steering)
        # At 30m/s with 0.1m offset: -0.001 curvature (gentler at highway speeds)
        curvature_correction = -lane_offset * 0.3 / max(v_ego, 5.0)
        desired_curvature += curvature_correction

    if v_ego > MIN_LAT_CONTROL_SPEED:
      desired_curvature = smooth_value(desired_curvature, prev_action.desiredCurvature, lat_smooth)
    else:
      desired_curvature = prev_action.desiredCurvature

    return log.ModelDataV2.Action(desiredCurvature=float(desired_curvature),
                                  desiredAcceleration=float(desired_accel),
                                  shouldStop=bool(should_stop))

class FrameMeta:
  frame_id: int = 0
  timestamp_sof: int = 0
  timestamp_eof: int = 0

  def __init__(self, vipc=None):
    if vipc is not None:
      self.frame_id, self.timestamp_sof, self.timestamp_eof = vipc.frame_id, vipc.timestamp_sof, vipc.timestamp_eof

class InputQueues:
  def __init__ (self, model_fps, env_fps, n_frames_input):
    assert env_fps % model_fps == 0
    assert env_fps >= model_fps
    self.model_fps = model_fps
    self.env_fps = env_fps
    self.n_frames_input = n_frames_input

    self.dtypes = {}
    self.shapes = {}
    self.q = {}

  def update_dtypes_and_shapes(self, input_dtypes, input_shapes) -> None:
    self.dtypes.update(input_dtypes)
    if self.env_fps == self.model_fps:
      self.shapes.update(input_shapes)
    else:
      for k in input_shapes:
        shape = list(input_shapes[k])
        if 'img' in k:
          n_channels = shape[1] // self.n_frames_input
          shape[1] = (self.env_fps // self.model_fps + (self.n_frames_input - 1)) * n_channels
        else:
          shape[1] = (self.env_fps // self.model_fps) * shape[1]
        self.shapes[k] = tuple(shape)

  def reset(self) -> None:
    self.q = {k: np.zeros(self.shapes[k], dtype=self.dtypes[k]) for k in self.dtypes.keys()}

  def enqueue(self, inputs:dict[str, np.ndarray]) -> None:
    for k in inputs.keys():
      if inputs[k].dtype != self.dtypes[k]:
        raise ValueError(f'supplied input <{k}({inputs[k].dtype})> has wrong dtype, expected {self.dtypes[k]}')
      input_shape = list(self.shapes[k])
      input_shape[1] = -1
      single_input = inputs[k].reshape(tuple(input_shape))
      sz = single_input.shape[1]
      self.q[k][:,:-sz] = self.q[k][:,sz:]
      self.q[k][:,-sz:] = single_input

  def get(self, *names) -> dict[str, np.ndarray]:
    if self.env_fps == self.model_fps:
      return {k: self.q[k] for k in names}
    else:
      out = {}
      for k in names:
        shape = self.shapes[k]
        if 'img' in k:
          n_channels = shape[1] // (self.env_fps // self.model_fps + (self.n_frames_input - 1))
          out[k] = np.concatenate([self.q[k][:, s:s+n_channels] for s in np.linspace(0, shape[1] - n_channels, self.n_frames_input, dtype=int)], axis=1)
        elif 'pulse' in k:
          # any pulse within interval counts
          out[k] = self.q[k].reshape((shape[0], shape[1] * self.model_fps // self.env_fps, self.env_fps // self.model_fps, -1)).max(axis=2)
        else:
          idxs = np.arange(-1, -shape[1], -self.env_fps // self.model_fps)[::-1]
          out[k] = self.q[k][:, idxs]
      return out

class ModelState:
  frames: dict[str, DrivingModelFrame]
  inputs: dict[str, np.ndarray]
  output: np.ndarray
  prev_desire: np.ndarray  # for tracking the rising edge of the pulse

  def __init__(self, context: CLContext):
    # Get model ID and version from params for dynamic path building
    params = Params()
    model_id = params.get("Model")
    model_id = model_id.decode("utf-8") if model_id else "bd2"
    model_version = params.get("ModelVersion")
    model_version = model_version.decode("utf-8") if model_version else "v11"

    # Build paths based on model ID
    if model_id == "bd2":
      # Default model uses built-in files
      vision_pkl_path = VISION_PKL_PATH
      policy_pkl_path = POLICY_PKL_PATH
      vision_metadata_path = VISION_METADATA_PATH
      policy_metadata_path = POLICY_METADATA_PATH
      off_policy_pkl_path = OFF_POLICY_PKL_PATH
      off_policy_metadata_path = OFF_POLICY_METADATA_PATH
    else:
      # Custom models use /data/models/{model_id}_*.pkl
      vision_pkl_path = MODELS_PATH / f"{model_id}_driving_vision_tinygrad.pkl"
      policy_pkl_path = MODELS_PATH / f"{model_id}_driving_policy_tinygrad.pkl"
      vision_metadata_path = MODELS_PATH / f"{model_id}_driving_vision_metadata.pkl"
      policy_metadata_path = MODELS_PATH / f"{model_id}_driving_policy_metadata.pkl"
      off_policy_pkl_path = MODELS_PATH / f"{model_id}_driving_off_policy_tinygrad.pkl"
      off_policy_metadata_path = MODELS_PATH / f"{model_id}_driving_off_policy_metadata.pkl"

    with open(vision_metadata_path, 'rb') as f:
      vision_metadata = pickle.load(f)
      self.vision_input_shapes =  vision_metadata['input_shapes']
      self.vision_input_names = list(self.vision_input_shapes.keys())
      self.vision_output_slices = vision_metadata['output_slices']
      vision_output_size = vision_metadata['output_shapes']['outputs'][1]

    with open(policy_metadata_path, 'rb') as f:
      policy_metadata = pickle.load(f)
      self.policy_input_shapes =  policy_metadata['input_shapes']
      self.policy_output_slices = policy_metadata['output_slices']
      policy_output_size = policy_metadata['output_shapes']['outputs'][1]

    self.frames = {name: DrivingModelFrame(context, ModelConstants.MODEL_RUN_FREQ//ModelConstants.MODEL_CONTEXT_FREQ) for name in self.vision_input_names}
    self.prev_desire = np.zeros(ModelConstants.DESIRE_LEN, dtype=np.float32)

    # policy inputs
    self.numpy_inputs = {k: np.zeros(self.policy_input_shapes[k], dtype=np.float32) for k in self.policy_input_shapes}
    self.full_input_queues = InputQueues(ModelConstants.MODEL_CONTEXT_FREQ, ModelConstants.MODEL_RUN_FREQ, ModelConstants.N_FRAMES)
    for k in ['desire_pulse', 'features_buffer']:
      self.full_input_queues.update_dtypes_and_shapes({k: self.numpy_inputs[k].dtype}, {k: self.numpy_inputs[k].shape})
    self.full_input_queues.reset()

    # img buffers are managed in openCL transform code
    self.vision_inputs: dict[str, Tensor] = {}
    self.vision_output = np.zeros(vision_output_size, dtype=np.float32)
    self.policy_inputs = {k: Tensor(v, device='NPY').realize() for k,v in self.numpy_inputs.items()}
    self.policy_output = np.zeros(policy_output_size, dtype=np.float32)
    self.parser = Parser()

    with open(vision_pkl_path, "rb") as f:
      self.vision_run = pickle.load(f)

    with open(policy_pkl_path, "rb") as f:
      self.policy_run = pickle.load(f)

    # Off-policy model (optional, for v12 models)
    self.off_policy_enabled = False
    self.off_policy_output_slices: dict[str, slice] = {}
    self.off_policy_numpy_inputs: dict[str, np.ndarray] = {}
    self.off_policy_inputs: dict[str, Tensor] = {}
    self.off_policy_output: np.ndarray | None = None
    self.off_policy_parser = Parser()

    # Load off-policy model if v12 or if files exist
    if model_version == "v12" or off_policy_metadata_path.is_file():
      try:
        with open(off_policy_metadata_path, 'rb') as f:
          off_policy_metadata = pickle.load(f)
        self.off_policy_input_shapes = off_policy_metadata['input_shapes']
        self.off_policy_output_slices = off_policy_metadata['output_slices']
        off_policy_output_size = off_policy_metadata['output_shapes']['outputs'][1]

        # Build off-policy inputs (mirror policy inputs structure)
        self.off_policy_numpy_inputs = {k: np.zeros(self.off_policy_input_shapes[k], dtype=np.float32) for k in self.off_policy_input_shapes}
        self.off_policy_inputs = {k: Tensor(v, device='NPY').realize() for k, v in self.off_policy_numpy_inputs.items()}
        self.off_policy_output = np.zeros(off_policy_output_size, dtype=np.float32)

        with open(off_policy_pkl_path, "rb") as f:
          self.off_policy_run = pickle.load(f)

        self.off_policy_enabled = True
        cloudlog.warning(f"Off-policy model loaded successfully for {model_id}")
      except FileNotFoundError as e:
        cloudlog.warning(f"Off-policy model not available: {e}")
      except Exception as e:
        cloudlog.error(f"Failed to load off-policy model: {e}")

  def slice_outputs(self, model_outputs: np.ndarray, output_slices: dict[str, slice]) -> dict[str, np.ndarray]:
    parsed_model_outputs = {k: model_outputs[np.newaxis, v] for k,v in output_slices.items()}
    return parsed_model_outputs

  def run(self, bufs: dict[str, VisionBuf], transforms: dict[str, np.ndarray],
                inputs: dict[str, np.ndarray], prepare_only: bool) -> dict[str, np.ndarray] | None:
    # Model decides when action is completed, so desire input is just a pulse triggered on rising edge
    inputs['desire_pulse'][0] = 0
    new_desire = np.where(inputs['desire_pulse'] - self.prev_desire > .99, inputs['desire_pulse'], 0)
    self.prev_desire[:] = inputs['desire_pulse']

    imgs_cl = {name: self.frames[name].prepare(bufs[name], transforms[name].flatten()) for name in self.vision_input_names}

    if TICI and not USBGPU:
      # The imgs tensors are backed by opencl memory, only need init once
      for key in imgs_cl:
        if key not in self.vision_inputs:
          self.vision_inputs[key] = qcom_tensor_from_opencl_address(imgs_cl[key].mem_address, self.vision_input_shapes[key], dtype=dtypes.uint8)
    else:
      for key in imgs_cl:
        frame_input = self.frames[key].buffer_from_cl(imgs_cl[key]).reshape(self.vision_input_shapes[key])
        self.vision_inputs[key] = Tensor(frame_input, dtype=dtypes.uint8).realize()

    if prepare_only:
      return None

    self.vision_output = self.vision_run(**self.vision_inputs).contiguous().realize().uop.base.buffer.numpy()
    vision_outputs_dict = self.parser.parse_vision_outputs(self.slice_outputs(self.vision_output, self.vision_output_slices))

    self.full_input_queues.enqueue({'features_buffer': vision_outputs_dict['hidden_state'], 'desire_pulse': new_desire})
    for k in ['desire_pulse', 'features_buffer']:
      self.numpy_inputs[k][:] = self.full_input_queues.get(k)[k]
    self.numpy_inputs['traffic_convention'][:] = inputs['traffic_convention']

    self.policy_output = self.policy_run(**self.policy_inputs).contiguous().realize().uop.base.buffer.numpy()
    policy_outputs_dict = self.parser.parse_policy_outputs(self.slice_outputs(self.policy_output, self.policy_output_slices))

    combined_outputs_dict = {**vision_outputs_dict, **policy_outputs_dict}

    # Run off-policy model if enabled
    if self.off_policy_enabled:
      # Copy inputs from policy to off-policy
      for k in ['desire_pulse', 'features_buffer']:
        if k in self.off_policy_numpy_inputs and k in self.numpy_inputs:
          self.off_policy_numpy_inputs[k][:] = self.numpy_inputs[k]
      if 'traffic_convention' in self.off_policy_numpy_inputs:
        self.off_policy_numpy_inputs['traffic_convention'][:] = inputs['traffic_convention']

      self.off_policy_output = self.off_policy_run(**self.off_policy_inputs).contiguous().realize().uop.base.buffer.numpy()
      off_policy_outputs_dict = self.off_policy_parser.parse_policy_outputs(self.slice_outputs(self.off_policy_output, self.off_policy_output_slices))
      combined_outputs_dict.update(off_policy_outputs_dict)

    if SEND_RAW_PRED:
      raw_pred = [self.vision_output.copy(), self.policy_output.copy()]
      if self.off_policy_enabled and self.off_policy_output is not None:
        raw_pred.append(self.off_policy_output.copy())
      combined_outputs_dict['raw_pred'] = np.concatenate(raw_pred)

    return combined_outputs_dict


def main(demo=False):
  cloudlog.warning("modeld init")

  # FrogPilot variables
  frogpilot_toggles = get_frogpilot_toggles()

  if not USBGPU:
    # USB GPU currently saturates a core so can't do this yet,
    # also need to move the aux USB interrupts for good timings
    config_realtime_process(7, 54)

  st = time.monotonic()
  cloudlog.warning("setting up CL context")
  cl_context = CLContext()
  cloudlog.warning("CL context ready; loading model")
  model = ModelState(cl_context)
  cloudlog.warning(f"models loaded in {time.monotonic() - st:.1f}s, modeld starting")

  # visionipc clients
  while True:
    available_streams = VisionIpcClient.available_streams("camerad", block=False)
    if available_streams:
      use_extra_client = VisionStreamType.VISION_STREAM_WIDE_ROAD in available_streams and VisionStreamType.VISION_STREAM_ROAD in available_streams
      main_wide_camera = VisionStreamType.VISION_STREAM_ROAD not in available_streams
      break
    time.sleep(.1)

  vipc_client_main_stream = VisionStreamType.VISION_STREAM_WIDE_ROAD if main_wide_camera else VisionStreamType.VISION_STREAM_ROAD
  vipc_client_main = VisionIpcClient("camerad", vipc_client_main_stream, True, cl_context)
  vipc_client_extra = VisionIpcClient("camerad", VisionStreamType.VISION_STREAM_WIDE_ROAD, False, cl_context)
  cloudlog.warning(f"vision stream set up, main_wide_camera: {main_wide_camera}, use_extra_client: {use_extra_client}")

  while not vipc_client_main.connect(False):
    time.sleep(0.1)
  while use_extra_client and not vipc_client_extra.connect(False):
    time.sleep(0.1)

  cloudlog.warning(f"connected main cam with buffer size: {vipc_client_main.buffer_len} ({vipc_client_main.width} x {vipc_client_main.height})")
  if use_extra_client:
    cloudlog.warning(f"connected extra cam with buffer size: {vipc_client_extra.buffer_len} ({vipc_client_extra.width} x {vipc_client_extra.height})")

  # messaging
  pm = PubMaster(["modelV2", "drivingModelData", "cameraOdometry"])
  sm = SubMaster(["deviceState", "carState", "roadCameraState", "liveCalibration", "driverMonitoringState", "carControl", "liveDelay"])

  publish_state = PublishState()
  params = Params()

  # setup filter to track dropped frames
  frame_dropped_filter = FirstOrderFilter(0., 10., 1. / ModelConstants.MODEL_RUN_FREQ)
  frame_id = 0
  last_vipc_frame_id = 0
  run_count = 0

  model_transform_main = np.zeros((3, 3), dtype=np.float32)
  model_transform_extra = np.zeros((3, 3), dtype=np.float32)
  live_calib_seen = False
  buf_main, buf_extra = None, None
  meta_main = FrameMeta()
  meta_extra = FrameMeta()

  # Pre-allocate arrays used in hot loop to avoid allocations at 20Hz
  traffic_convention = np.zeros(2, dtype=np.float32)
  vec_desire = np.zeros(ModelConstants.DESIRE_LEN, dtype=np.float32)


  if demo:
    CP = get_demo_car_params()
  else:
    CP = messaging.log_from_bytes(params.get("CarParams", block=True), car.CarParams)
  cloudlog.info("modeld got CarParams: %s", CP.brand)

  # TODO this needs more thought, use .2s extra for now to estimate other delays
  # TODO Move smooth seconds to action function
  long_delay = frogpilot_toggles.longitudinalActuatorDelay + LONG_SMOOTH_SECONDS
  prev_action = log.ModelDataV2.Action()

  DH = DesireHelper()

  # FrogPilot variables
  sm = sm.extend(['frogpilotPlan'])
  pm = pm.extend(['frogpilotModelV2'])

  while True:
    # Keep receiving frames until we are at least 1 frame ahead of previous extra frame
    while meta_main.timestamp_sof < meta_extra.timestamp_sof + 25000000:
      buf_main = vipc_client_main.recv()
      meta_main = FrameMeta(vipc_client_main)
      if buf_main is None:
        break

    if buf_main is None:
      # cloudlog.debug("vipc_client_main no frame")  # Muted - too verbose at 20Hz
      continue

    if use_extra_client:
      # Keep receiving extra frames until frame id matches main camera
      while True:
        buf_extra = vipc_client_extra.recv()
        meta_extra = FrameMeta(vipc_client_extra)
        if buf_extra is None or meta_main.timestamp_sof < meta_extra.timestamp_sof + 25000000:
          break

      if buf_extra is None:
        # cloudlog.debug("vipc_client_extra no frame")  # Muted - too verbose at 20Hz
        continue

      if abs(meta_main.timestamp_sof - meta_extra.timestamp_sof) > 10000000:
        # cloudlog.error(f"frames out of sync! main: {meta_main.frame_id} ({meta_main.timestamp_sof / 1e9:.5f}),\
        #                  extra: {meta_extra.frame_id} ({meta_extra.timestamp_sof / 1e9:.5f})")  # Muted - expensive formatting at 20Hz
        pass

    else:
      # Use single camera
      buf_extra = buf_main
      meta_extra = meta_main

    sm.update(0)
    desire = DH.desire
    is_rhd = sm["driverMonitoringState"].isRHD
    frame_id = sm["roadCameraState"].frameId
    v_ego = max(sm["carState"].vEgo, 0.)
    lat_delay = sm["liveDelay"].lateralDelay + LAT_SMOOTH_SECONDS
    if sm.updated["liveCalibration"] and sm.seen['roadCameraState'] and sm.seen['deviceState']:
      device_from_calib_euler = np.array(sm["liveCalibration"].rpyCalib, dtype=np.float32)
      dc = DEVICE_CAMERAS[(str(sm['deviceState'].deviceType), str(sm['roadCameraState'].sensor))]
      model_transform_main = get_warp_matrix(device_from_calib_euler, dc.ecam.intrinsics if main_wide_camera else dc.fcam.intrinsics, False).astype(np.float32)
      model_transform_extra = get_warp_matrix(device_from_calib_euler, dc.ecam.intrinsics, True).astype(np.float32)
      live_calib_seen = True

    # Reset pre-allocated arrays in-place (avoids allocation at 20Hz)
    traffic_convention[:] = 0
    traffic_convention[int(is_rhd)] = 1

    vec_desire[:] = 0
    if desire >= 0 and desire < ModelConstants.DESIRE_LEN:
      vec_desire[desire] = 1

    # tracked dropped frames
    vipc_dropped_frames = max(0, meta_main.frame_id - last_vipc_frame_id - 1)
    frames_dropped = frame_dropped_filter.update(min(vipc_dropped_frames, 10))
    if run_count < 10: # let frame drops warm up
      frame_dropped_filter.x = 0.
      frames_dropped = 0.
    run_count = run_count + 1

    frame_drop_ratio = frames_dropped / (1 + frames_dropped)
    prepare_only = vipc_dropped_frames > 0
    if prepare_only:
      pass  # cloudlog.error(f"skipping model eval. Dropped {vipc_dropped_frames} frames")  # Muted - too verbose during frame drops

    bufs = {name: buf_extra if 'big' in name else buf_main for name in model.vision_input_names}
    transforms = {name: model_transform_extra if 'big' in name else model_transform_main for name in model.vision_input_names}
    inputs:dict[str, np.ndarray] = {
      'desire_pulse': vec_desire,
      'traffic_convention': traffic_convention,
    }

    mt1 = time.perf_counter()
    model_output = model.run(bufs, transforms, inputs, prepare_only)
    mt2 = time.perf_counter()
    model_execution_time = mt2 - mt1

    if model_output is not None:
      modelv2_send = messaging.new_message('modelV2')
      drivingdata_send = messaging.new_message('drivingModelData')
      posenet_send = messaging.new_message('cameraOdometry')

      action = get_action_from_model(model_output, prev_action, lat_delay + DT_MDL, long_delay + DT_MDL, v_ego, desire, frogpilot_toggles)
      prev_action = action
      lane_position_offset = frogpilot_toggles.lane_position_offset if frogpilot_toggles else 0.0
      fill_model_msg(drivingdata_send, modelv2_send, model_output, action,
                     publish_state, meta_main.frame_id, meta_extra.frame_id, frame_id,
                     frame_drop_ratio, meta_main.timestamp_eof, model_execution_time, live_calib_seen,
                     lane_position_offset)

      desire_state = modelv2_send.modelV2.meta.desireState
      l_lane_change_prob = desire_state[log.Desire.laneChangeLeft]
      r_lane_change_prob = desire_state[log.Desire.laneChangeRight]
      lane_change_prob = l_lane_change_prob + r_lane_change_prob
      DH.update(sm['carState'], sm['carControl'].latActive, lane_change_prob, sm['frogpilotPlan'], frogpilot_toggles)
      modelv2_send.modelV2.meta.laneChangeState = DH.lane_change_state
      modelv2_send.modelV2.meta.laneChangeDirection = DH.lane_change_direction
      drivingdata_send.drivingModelData.meta.laneChangeState = DH.lane_change_state
      drivingdata_send.drivingModelData.meta.laneChangeDirection = DH.lane_change_direction

      fill_pose_msg(posenet_send, model_output, meta_main.frame_id, vipc_dropped_frames, meta_main.timestamp_eof, live_calib_seen)
      pm.send('modelV2', modelv2_send)
      pm.send('drivingModelData', drivingdata_send)
      pm.send('cameraOdometry', posenet_send)

      # FrogPilot variables
      frogpilot_modelv2_send = messaging.new_message('frogpilotModelV2')
      frogpilot_modelv2_send.frogpilotModelV2.turnDirection = DH.turn_direction

      pm.send('frogpilotModelV2', frogpilot_modelv2_send)
    last_vipc_frame_id = meta_main.frame_id

    # FrogPilot variables
    if sm['frogpilotPlan'].togglesUpdated:
      frogpilot_toggles = get_frogpilot_toggles()


if __name__ == "__main__":
  try:
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--demo', action='store_true', help='A boolean for demo mode.')
    args = parser.parse_args()
    main(demo=args.demo)
  except KeyboardInterrupt:
    cloudlog.warning("got SIGINT")
