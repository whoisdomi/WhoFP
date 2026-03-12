import math
import numpy as np
from collections import deque

from cereal import log
from opendbc.car.lateral import get_friction
from opendbc.car.interfaces import LatControlInputs
from openpilot.common.constants import ACCELERATION_DUE_TO_GRAVITY
from openpilot.common.realtime import DT_CTRL
from openpilot.selfdrive.controls.lib.latcontrol import LatControl
from openpilot.common.pid import PIDController

# At higher speeds (25+mph) we can assume:
# Lateral acceleration achieved by a specific car correlates to
# torque applied to the steering rack. It does not correlate to
# wheel slip, or to speed.

# This controller applies torque to achieve desired lateral
# accelerations. To compensate for the low speed effects the
# proportional gain is increased at low speeds by the PID controller.
# Additionally, there is friction in the steering wheel that needs
# to be overcome to move it at all, this is compensated for too.

# === Gain Settings ===
# Base KP/KI come from torque_params (set via UI or car defaults)
# These are fallbacks if torque_params aren't set
DEFAULT_KP = 1.0
DEFAULT_KI = 0.3

# === Speed-Dependent Gain Table ===
# KP_MULTIPLIERS: proportional gain multiplier (× base kP from UI)
# LOW_SPEED_Y: error amplification factor (boosts P, I, and friction at low speeds)
#
# m/s:            1      1.5    2.0    3.0    5      7.5    10     15     30
# mph:            2.2    3.4    4.5    6.7    11.2   16.8   22.4   33.6   67.1
INTERP_SPEEDS =  [1,     1.5,   2.0,   3.0,   5,     7.5,   10,    15,    30   ]
KP_MULTIPLIERS = [250,   120,   65,    22,    9,     5.5,   3.5,   2.0,   1.0  ]
LOW_SPEED_Y =    [3.0,   2.8,   2.5,   2.5,   2.0,   1.8,   1.4,   1.0,   1.0  ]
LOW_SPEED_X =    [1,     1.5,   2.0,   3.0,   5,     7.5,   10,    15,    30   ]

# === Delay Compensation ===
LAT_ACCEL_REQUEST_BUFFER_SECONDS = 1.0
# Note: Initial lat_delay is calculated in __init__ as CP.steerActuatorDelay + 0.2 (matching lagd)

# === Unwind Detection (from StarPilot) ===
UNWIND_D_DES_THRESHOLD = -1.0      # Desired accel decreasing fast (m/s³)
UNWIND_LAT_ACCEL_NEAR_ZERO = 0.8   # Near straight (m/s²), compared against raw (unscaled) lat accel
UNWIND_FRAMES_ACTIVATE = 5         # Counter threshold to activate decay
UNWIND_COUNTER_MAX = 15            # Max counter value; once reached, needs 10 false frames to deactivate

# === Integrator Decay ===
UNWIND_MULTIPLIER = 1.0  # Disabled - unwind_detected handles turn exits instead of global decay

# === Straight-Stop Suppression ===
# Scales low_speed_factor toward 1.0 when near-straight and slow.
# Prevents friction snap and P-term ratcheting at stops without affecting turn behavior.
# Suppression activates below STRAIGHT_STOP_SPEED and fades out as desired_curvature
# rises above STRAIGHT_STOP_CURVATURE (~25m radius), restoring full low_speed_factor for turns.
STRAIGHT_STOP_SPEED = 3.0       # m/s (~6.7 mph)
STRAIGHT_STOP_CURVATURE = 0.04  # rad/m (~25m radius)

# === Friction Threshold (from StarPilot) ===
# Speed-interpolated: requires larger error before full friction fires.
# HKG needs higher thresholds than GM to prevent ticking/wobble.
# Effective real-world threshold = friction_threshold / LOW_SPEED_Y
#
# m/s:                          0.5    2.0    5.0    10.0   33.5
# mph:                          1.1    4.5    11.2   22.4   75.0
FRICTION_THRESHOLD_SPEEDS =    [0.5,   2.0,   5.0,   10.0,  33.5]
FRICTION_THRESHOLD_VALUES =    [0.15,  0.20,  0.38,  0.42,  0.42]


def get_friction_threshold(v_ego: float) -> float:
  """Returns speed-interpolated friction threshold."""
  return float(np.interp(v_ego, FRICTION_THRESHOLD_SPEEDS, FRICTION_THRESHOLD_VALUES))


class LatControlTorque(LatControl):
  def __init__(self, CP, CI):
    super().__init__(CP, CI)
    self.torque_params = CP.lateralTuning.torque.as_builder()
    self.torque_from_lateral_accel = CI.torque_from_lateral_accel()
    self.steering_angle_deadzone_deg = self.torque_params.steeringAngleDeadzoneDeg

    # Store current KP/KI for live update detection
    self.current_kp = 0.0
    self.current_ki = 0.0

    # Initialize PID (will be configured in update_pid_gains)
    self.pid = PIDController(DEFAULT_KP, DEFAULT_KI, k_f=1.0,
                             pos_limit=1e308, neg_limit=-1e308,
                             unwind_multiplier=UNWIND_MULTIPLIER)
    self.update_pid_gains()

    # Delay compensation buffer
    self.lat_accel_request_buffer_len = int(LAT_ACCEL_REQUEST_BUFFER_SECONDS / DT_CTRL)
    self.lat_accel_request_buffer = deque([0.] * self.lat_accel_request_buffer_len, maxlen=self.lat_accel_request_buffer_len)

    # Live lateral delay from lagd (updated by controlsd via update_live_delay)
    # Initial value: actuator delay + estimated processing delay (~0.06-0.1)
    # Will be replaced by lagd's learned value once available
    self.lat_delay = CP.steerActuatorDelay + 0.1

    # Unwind detection state
    self.prev_future_desired_lateral_accel = 0.0
    self.unwind_frames = 0

  def update_live_delay(self, lateral_delay):
    """Update lateral delay from lagd (called by controlsd when liveDelay updates)."""
    if lateral_delay > 0:
      self.lat_delay = lateral_delay

  def update_pid_gains(self, frogpilot_toggles=None):
    """Update PID gains from frogpilot_toggles (live) or torque_params (startup)."""
    # Try to get live values from frogpilot_toggles first
    if frogpilot_toggles is not None and hasattr(frogpilot_toggles, 'steerKp'):
      # steerKp is [[speeds], [values]] format, get the first (only) value
      base_kp = frogpilot_toggles.steerKp[1][0] if frogpilot_toggles.steerKp else DEFAULT_KP
      base_ki = frogpilot_toggles.steerKi if hasattr(frogpilot_toggles, 'steerKi') else DEFAULT_KI
    else:
      # Fall back to torque_params (startup values)
      base_kp = self.torque_params.kp if self.torque_params.kp > 0 else DEFAULT_KP
      base_ki = self.torque_params.ki if self.torque_params.ki > 0 else DEFAULT_KI

    # Only update if gains changed
    if base_kp != self.current_kp or base_ki != self.current_ki:
      self.current_kp = base_kp
      self.current_ki = base_ki

      # Scale KP multipliers by the base KP from settings
      kp_interp = [m * base_kp for m in KP_MULTIPLIERS]

      # Update PID gains
      self.pid._k_p = [INTERP_SPEEDS, kp_interp]
      self.pid._k_i = [[0], [base_ki]]

      # Calculate PID limits based on latAccelFactor
      # lat_accel = torque * latAccelFactor, so max_lat_accel = steer_max * latAccelFactor
      lat_accel_factor = self.torque_params.latAccelFactor if self.torque_params.latAccelFactor > 0 else 2.5
      max_lat_accel = self.steer_max * lat_accel_factor
      self.pid.pos_limit = max_lat_accel
      self.pid.neg_limit = -max_lat_accel

  def update_live_torque_params(self, latAccelFactor, latAccelOffset, friction):
    self.torque_params.latAccelFactor = latAccelFactor
    self.torque_params.latAccelOffset = latAccelOffset
    self.torque_params.friction = friction
    # Recalculate PID limits when latAccelFactor changes
    self.update_pid_gains()

  def update(self, active, CS, VM, params, steer_limited_by_controls, desired_curvature, curvature_limited, calibrated_pose, model_data, frogpilot_toggles):
    pid_log = log.ControlsState.LateralTorqueState.new_message()

    # Check for KP/KI changes from UI (allows live tuning)
    self.update_pid_gains(frogpilot_toggles)

    # Calculate current state
    measured_curvature = -VM.calc_curvature(math.radians(CS.steeringAngleDeg - params.angleOffsetDeg), CS.vEgo, params.roll)
    measurement = measured_curvature * CS.vEgo ** 2
    future_desired_lateral_accel = desired_curvature * CS.vEgo ** 2
    self.lat_accel_request_buffer.append(future_desired_lateral_accel)

    roll_compensation = params.roll * ACCELERATION_DUE_TO_GRAVITY
    curvature_deadzone = abs(VM.calc_curvature(math.radians(self.steering_angle_deadzone_deg), CS.vEgo, 0.0))
    lateral_accel_deadzone = curvature_deadzone * CS.vEgo ** 2

    # Use live lateral delay from lagd (updated via update_live_delay from controlsd)
    lat_delay = self.lat_delay

    # Delay compensation: compare against what we requested lat_delay ago
    delay_frames = int(np.clip(lat_delay / DT_CTRL + 1, 1, self.lat_accel_request_buffer_len))
    expected_lateral_accel = self.lat_accel_request_buffer[-delay_frames]

    # Jerk calculation: rate of change of desired lateral accel over the delay period
    desired_lateral_jerk = (future_desired_lateral_accel - expected_lateral_accel) / lat_delay

    # Delay-compensated setpoint: predict where the desired lateral accel will be
    # lat_delay into the future, so the controller leads the target instead of chasing it.
    # setpoint = current_desired + jerk * lat_delay = 2*future - expected
    setpoint = future_desired_lateral_accel + desired_lateral_jerk * lat_delay

    # Low speed factor: curvature-proportional boost for turns at low speeds
    # Works alongside KP_INTERP to help with tight turns at low speed
    low_speed_factor = float(np.interp(CS.vEgo, LOW_SPEED_X, LOW_SPEED_Y))

    # Straight-stop suppression: scale low_speed_factor toward 1.0 when near-straight and slow.
    # Prevents friction from snapping left/right at sub-6mph stops without affecting turns —
    # curve_scale rises from 0 → 1 as desired_curvature crosses STRAIGHT_STOP_CURVATURE,
    # so full low_speed_factor is restored by the time a real turn begins.
    if CS.vEgo < STRAIGHT_STOP_SPEED and low_speed_factor > 1.0:
      curve_scale = float(np.clip(abs(desired_curvature) / STRAIGHT_STOP_CURVATURE, 0.0, 1.0))
      low_speed_factor = 1.0 + (low_speed_factor - 1.0) * curve_scale

    setpoint *= low_speed_factor
    measurement *= low_speed_factor

    error = setpoint - measurement

    # Feedforward: gravity-adjusted desired lateral accel
    gravity_adjusted_future_lateral_accel = future_desired_lateral_accel - roll_compensation
    ff = gravity_adjusted_future_lateral_accel
    # latAccelOffset corrects roll compensation bias from device roll misalignment relative to car roll
    ff -= self.torque_params.latAccelOffset
    # Friction term (ACTS-HORIZON style: no jerk in friction, jerk is in setpoint instead)
    # Use speed-interpolated threshold: lower at low speed (helps turns), higher at highway (prevents ticking)
    friction_threshold = get_friction_threshold(CS.vEgo)
    ff += get_friction(error, lateral_accel_deadzone, friction_threshold, self.torque_params)

    # Unwind detection: use raw future_desired_lateral_accel (before jerk prediction and low_speed_factor
    # scaling) so the rate and near-zero checks are stable and in physical units (m/s²).
    desired_lateral_accel_rate = (future_desired_lateral_accel - self.prev_future_desired_lateral_accel) / DT_CTRL
    unwind_condition = (desired_lateral_accel_rate < UNWIND_D_DES_THRESHOLD and
                        abs(future_desired_lateral_accel) < UNWIND_LAT_ACCEL_NEAR_ZERO)
    self.prev_future_desired_lateral_accel = future_desired_lateral_accel
    # Hysteresis counter: builds at +1/frame when condition is met, drains at -1/frame when not.
    # Activates at UNWIND_FRAMES_ACTIVATE (5 frames), saturates at UNWIND_COUNTER_MAX (15 frames).
    # Once saturated, transient noise (< 10 false frames) can't break the detection.
    if unwind_condition:
      self.unwind_frames = min(self.unwind_frames + 1, UNWIND_COUNTER_MAX)
    else:
      self.unwind_frames = max(self.unwind_frames - 1, 0)
    unwind_detected = self.unwind_frames >= UNWIND_FRAMES_ACTIVATE

    if not active:
      output_torque = 0.0
      pid_log.active = False
      self.pid.reset()
      self.prev_future_desired_lateral_accel = 0.0
      self.unwind_frames = 0
      self.lat_accel_request_buffer = deque([0.] * self.lat_accel_request_buffer_len, maxlen=self.lat_accel_request_buffer_len)
    else:
      # Error correction in lateral acceleration space
      pid_log.error = float(error)

      # Freeze integrator conditions (unwind_detected NOT included — decay alone handles turn exit,
      # freezing creates a hard on/off boundary that causes "section" oscillation)
      freeze_integrator = steer_limited_by_controls or CS.steeringPressed or CS.vEgo < 1.5

      # PID update in lat accel space
      output_lataccel = self.pid.update(pid_log.error, speed=CS.vEgo, feedforward=ff, freeze_integrator=freeze_integrator)

      # Gently decay integrator during turn exit (not frozen — integrator still accumulates above,
      # so this is a net drain that smoothly bleeds off turn-exit buildup without hard on/off steps)
      if unwind_detected:
        self.pid.i *= 0.95

      # Convert to torque at the end
      output_torque = self.torque_from_lateral_accel(
        LatControlInputs(output_lataccel, roll_compensation, CS.vEgo, CS.aEgo),
        self.torque_params,
        gravity_adjusted=True
      )

      pid_log.active = True
      pid_log.p = float(self.pid.p)
      pid_log.i = float(self.pid.i)
      pid_log.d = float(self.pid.d)
      pid_log.f = float(self.pid.f)
      pid_log.output = float(-output_torque)
      pid_log.actualLateralAccel = float(measurement)
      pid_log.desiredLateralAccel = float(setpoint)
      pid_log.saturated = bool(self._check_saturation(self.steer_max - abs(output_torque) < 1e-3, CS, steer_limited_by_controls, curvature_limited))

    # TODO left is positive in this convention
    return -output_torque, 0.0, pid_log
