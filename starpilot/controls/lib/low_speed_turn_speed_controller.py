#!/usr/bin/env python3
import numpy as np

from openpilot.common.constants import CV
from openpilot.common.realtime import DT_MDL

from openpilot.starpilot.common.starpilot_variables import CITY_SPEED_LIMIT

LSTSC_MIN_SPEED = 5 * CV.MPH_TO_MS
LSTSC_MAX_SPEED = CITY_SPEED_LIMIT * CV.MPH_TO_MS

LSTSC_MIN_STEER_ANGLE_DEG = 30.0

ANGLE_BUCKET_DEG = 5
MIN_ANGLE_BUCKET_DEG = 30
MAX_ANGLE_BUCKET_DEG = 540

SPEED_BUCKET_MPH = 1
MIN_SPEED_BUCKET_MPH = 5
MAX_SPEED_BUCKET_MPH = 25

TORQUE_HOLD_THRESHOLD = 0.55
TORQUE_SOFT_THRESHOLD = 0.75
TORQUE_HARD_THRESHOLD = 0.90
TORQUE_RELEASE_THRESHOLD = 0.50
TORQUE_TARGET_HEADROOM = 0.65

SATURATION_LATCH_TIME = 0.4
SATURATION_RELEASE_TIME = 0.5

COMFORT_DECEL = 1.5
HARD_DECEL = 2.0
SOFT_DECEL_MIN = 0.3

MIN_SAMPLES_PER_CELL = 5
PERSIST_INTERVAL = 2.0


class LowSpeedTurnSpeedController:
  def __init__(self, StarPilotVCruise):
    self.starpilot_planner = StarPilotVCruise.starpilot_planner

    self.enable_training = False
    self.calibrate_mode_active = False
    self.target_set = False

    self.target = 0.0
    self.torque_pct = 0.0
    self.calibration_progress = 0.0

    self.sat_time = 0.0
    self.release_time = 0.0
    self.intervention_active = False
    self._persist_timer = 0.0
    self._visited_angles = set()

    torque_data = self.starpilot_planner.params.get("LowSpeedTurnTorqueData")
    self.torque_data = self._normalize_torque_data(torque_data)

    # Reconstruct visited angles from loaded data so calibration progress
    # survives reboots — without this _visited_angles is always empty at startup.
    for cell_key in self.torque_data:
      angle_part = cell_key.split("_")[0]
      self._visited_angles.add(angle_part)

    self._update_calibration_progress()

  # ===== Bucketing helpers =====

  @staticmethod
  def _bucket_angle(steering_angle_deg):
    abs_angle = abs(float(steering_angle_deg))
    clipped = float(np.clip(abs_angle, MIN_ANGLE_BUCKET_DEG, MAX_ANGLE_BUCKET_DEG))
    bucket_index = round((clipped - MIN_ANGLE_BUCKET_DEG) / ANGLE_BUCKET_DEG)
    bucketed = MIN_ANGLE_BUCKET_DEG + (bucket_index * ANGLE_BUCKET_DEG)
    return str(int(bucketed))

  @staticmethod
  def _bucket_speed_mph(v_ego_ms):
    speed_mph = v_ego_ms * CV.MS_TO_MPH
    clipped = int(np.clip(round(speed_mph / SPEED_BUCKET_MPH) * SPEED_BUCKET_MPH,
                          MIN_SPEED_BUCKET_MPH, MAX_SPEED_BUCKET_MPH))
    return str(clipped)

  @classmethod
  def _cell_key(cls, angle_bucket, speed_bucket):
    return f"{angle_bucket}_{speed_bucket}"

  @classmethod
  def _normalize_torque_data(cls, torque_data):
    if not isinstance(torque_data, dict):
      return {}

    normalized = {}
    for key, value in torque_data.items():
      if not isinstance(value, dict):
        continue

      try:
        torque_avg = float(value["torque_avg"])
        torque_max = float(value["torque_max"])
        count = int(value["count"])
      except (KeyError, TypeError, ValueError):
        continue

      if count <= 0:
        continue

      normalized[key] = {
        "torque_avg": torque_avg,
        "torque_max": torque_max,
        "count": count,
      }

    return normalized

  # ===== Sensing =====

  @staticmethod
  def _get_active_torque(sm):
    actuators = sm["carControl"].actuators
    return float(abs(actuators.torque))

  @staticmethod
  def _get_lateral_saturation(sm):
    lat_state = sm["controlsState"].lateralControlState
    try:
      active = getattr(lat_state, lat_state.which())
      return bool(getattr(active, "saturated", False))
    except (AttributeError, TypeError):
      return False

  def _update_saturation_latch(self, saturated):
    if saturated:
      self.sat_time = min(self.sat_time + DT_MDL, SATURATION_LATCH_TIME * 2)
    else:
      self.sat_time = max(self.sat_time - DT_MDL, 0.0)
    return self.sat_time >= SATURATION_LATCH_TIME

  # ===== Calibration =====

  def _record_sample(self, angle_bucket, speed_bucket, torque_pct):
    key = self._cell_key(angle_bucket, speed_bucket)
    cell = self.torque_data.get(key)
    if cell is None:
      self.torque_data[key] = {
        "torque_avg": torque_pct,
        "torque_max": torque_pct,
        "count": 1,
      }
    else:
      count = cell["count"]
      cell["torque_avg"] = ((cell["torque_avg"] * count) + torque_pct) / (count + 1)
      cell["torque_max"] = max(cell["torque_max"], torque_pct)
      cell["count"] = count + 1
    self._visited_angles.add(angle_bucket)

  def _maybe_persist(self):
    self._persist_timer += DT_MDL
    if self._persist_timer >= PERSIST_INTERVAL:
      self._persist_timer = 0.0
      self.starpilot_planner.params.put_nonblocking("LowSpeedTurnTorqueData", self.torque_data)
      self._update_calibration_progress()
      self.starpilot_planner.params.put_nonblocking("LowSpeedTurnCalibrationProgress", self.calibration_progress)

  def _update_calibration_progress(self):
    if not self._visited_angles:
      self.calibration_progress = 0.0
      return

    calibrated = 0
    for angle_bucket in self._visited_angles:
      for speed_bucket_mph in range(MIN_SPEED_BUCKET_MPH, MAX_SPEED_BUCKET_MPH + 1, SPEED_BUCKET_MPH):
        key = self._cell_key(angle_bucket, str(speed_bucket_mph))
        cell = self.torque_data.get(key)
        if cell is not None and cell["count"] >= MIN_SAMPLES_PER_CELL:
          calibrated += 1
          break

    self.calibration_progress = (calibrated / len(self._visited_angles)) * 100.0

  def _predict_safe_speed(self, angle_bucket, v_ego):
    current_mph = int(np.clip(v_ego * CV.MS_TO_MPH, MIN_SPEED_BUCKET_MPH, MAX_SPEED_BUCKET_MPH))
    safe_mph = None
    for speed_mph in range(current_mph, MIN_SPEED_BUCKET_MPH - 1, -1):
      key = self._cell_key(angle_bucket, str(speed_mph))
      cell = self.torque_data.get(key)
      if cell is None or cell["count"] < MIN_SAMPLES_PER_CELL:
        continue
      if cell["torque_avg"] < TORQUE_TARGET_HEADROOM:
        safe_mph = speed_mph
        break
    if safe_mph is None:
      return None
    return safe_mph * CV.MPH_TO_MS

  # ===== Public API =====

  def calibration_log(self, v_ego, sm, in_low_speed_turn):
    self.calibrate_mode_active = False
    self.enable_training = False
    self.target_set = False
    self.intervention_active = False
    self.sat_time = 0.0
    self.release_time = 0.0

    self.torque_pct = self._get_active_torque(sm)

    aol_enabled = bool(sm["starpilotCarState"].alwaysOnLateralEnabled)
    eligible = (in_low_speed_turn
                and aol_enabled
                and not sm["carState"].standstill)

    if not eligible:
      return

    self.calibrate_mode_active = True
    self.enable_training = True
    angle_bucket = self._bucket_angle(sm["carState"].steeringAngleDeg)
    speed_bucket = self._bucket_speed_mph(v_ego)
    self._record_sample(angle_bucket, speed_bucket, self.torque_pct)
    self._maybe_persist()

  def log_data(self, v_ego, sm):
    self.calibrate_mode_active = False
    self.target_set = False
    self.intervention_active = False
    self.sat_time = 0.0
    self.release_time = 0.0

    self.torque_pct = self._get_active_torque(sm)

    long_active = bool(sm["carControl"].longActive)
    in_window = LSTSC_MIN_SPEED < v_ego < LSTSC_MAX_SPEED
    is_turning = abs(sm["carState"].steeringAngleDeg) >= LSTSC_MIN_STEER_ANGLE_DEG
    eligible = (long_active
                and in_window
                and is_turning
                and not sm["carState"].standstill)

    if not eligible:
      self.enable_training = False
      return

    self.enable_training = True
    angle_bucket = self._bucket_angle(sm["carState"].steeringAngleDeg)
    speed_bucket = self._bucket_speed_mph(v_ego)
    self._record_sample(angle_bucket, speed_bucket, self.torque_pct)
    self._maybe_persist()

  def update_target(self, v_ego, sm):
    self.calibrate_mode_active = False
    self.enable_training = True

    self.torque_pct = self._get_active_torque(sm)
    saturated = self._get_lateral_saturation(sm)
    sat_latched = self._update_saturation_latch(saturated)

    if self.torque_pct < TORQUE_RELEASE_THRESHOLD and not sat_latched:
      self.release_time = min(self.release_time + DT_MDL, SATURATION_RELEASE_TIME * 2)
    else:
      self.release_time = 0.0
    if self.intervention_active and self.release_time >= SATURATION_RELEASE_TIME:
      self.intervention_active = False

    reactive_decel = 0.0
    if sat_latched or self.torque_pct >= TORQUE_HARD_THRESHOLD:
      reactive_decel = HARD_DECEL
      self.intervention_active = True
    elif self.torque_pct >= TORQUE_SOFT_THRESHOLD:
      ramp = (self.torque_pct - TORQUE_SOFT_THRESHOLD) / (TORQUE_HARD_THRESHOLD - TORQUE_SOFT_THRESHOLD)
      reactive_decel = SOFT_DECEL_MIN + ramp * (COMFORT_DECEL - SOFT_DECEL_MIN)
      self.intervention_active = True
    elif self.torque_pct >= TORQUE_HOLD_THRESHOLD:
      self.intervention_active = True

    angle_bucket = self._bucket_angle(sm["carState"].steeringAngleDeg)
    predicted_safe = self._predict_safe_speed(angle_bucket, v_ego)
    predictive_target = None
    if predicted_safe is not None and predicted_safe < v_ego:
      predictive_target = max(predicted_safe, LSTSC_MIN_SPEED)

    if not self.target_set:
      self.target = v_ego
      self.target_set = True

    if reactive_decel > 0.0:
      self.target -= reactive_decel * DT_MDL
    elif self.intervention_active:
      self.target = min(self.target, v_ego)
    else:
      self.target = v_ego

    if predictive_target is not None:
      # Pace the predictive bleed using the model's time_to_curve when it's a real
      # number; otherwise default to a 3 second window for the in-turn ramp.
      time_to_curve = float(self.starpilot_planner.time_to_curve)
      pacing = max(time_to_curve, 3.0) if np.isfinite(time_to_curve) else 3.0
      decel_rate = max((v_ego - predictive_target) / pacing, 0.0)
      self.target = min(self.target, v_ego - decel_rate * DT_MDL)
      self.target = max(self.target, predictive_target)
      self.intervention_active = True

    self.target = float(np.clip(self.target, LSTSC_MIN_SPEED, v_ego))

    speed_bucket = self._bucket_speed_mph(v_ego)
    self._record_sample(angle_bucket, speed_bucket, self.torque_pct)
    self._maybe_persist()
