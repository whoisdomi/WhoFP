import math
import numpy as np
from collections import deque

from cereal import log
from opendbc.car.lateral import FRICTION_THRESHOLD, get_friction
from opendbc.car.interfaces import LatControlInputs
from openpilot.common.constants import ACCELERATION_DUE_TO_GRAVITY
from openpilot.common.filter_simple import FirstOrderFilter
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
DEFAULT_KI = 0.15

# Speed-interpolated KP: aggressive at low speed, tapering to base KP at highway
# The actual base KP (rightmost value) is set dynamically from torque_params in __init__
INTERP_SPEEDS = [1, 1.5, 2.0, 3.0, 5, 7.5, 10, 15, 30]
# Multipliers relative to base KP (last value = 1.0x base)
KP_MULTIPLIERS = [250, 120, 65, 30, 11.5, 5.5, 3.5, 2.0, 1.0]

# === Jerk Filtering (from stock comma) ===
LP_FILTER_CUTOFF_HZ = 1.2        # Low-pass filter for jerk smoothing
JERK_LOOKAHEAD_SECONDS = 0.19   # How far ahead to look for jerk calculation
JERK_GAIN = 0.3                  # Weight of jerk term in friction calculation

# === Delay Compensation (from stock comma) ===
LAT_ACCEL_REQUEST_BUFFER_SECONDS = 1.0
LAT_DELAY = 0.1  # Default lateral delay if not provided (seconds)

# === Unwind Detection (from StarPilot) ===
UNWIND_D_DES_THRESHOLD = -1.0      # Desired accel decreasing fast (m/s³)
UNWIND_LAT_ACCEL_NEAR_ZERO = 0.3   # Near straight (m/s²)

# === Integrator Decay ===
UNWIND_MULTIPLIER = 0.85  # Integrator decay when unwinding (0.85 = 15% decay per cycle)

# === Low Speed Factor (DISABLED - using KP interpolation instead) ===
# Uncomment to re-enable if KP interp alone isn't enough for low-speed response
# LOW_SPEED_X = [0, 15, 25, 35]
# LOW_SPEED_Y = [25, 10, 4, 2]  # Squared in calculation


class LatControlTorque(LatControl):
  def __init__(self, CP, CI):
    super().__init__(CP, CI)
    self.torque_params = CP.lateralTuning.torque.as_builder()
    self.torque_from_lateral_accel = CI.torque_from_lateral_accel()
    self.steering_angle_deadzone_deg = self.torque_params.steeringAngleDeadzoneDeg

    # Use KP/KI from torque_params (UI settings) or defaults
    base_kp = self.torque_params.kp if self.torque_params.kp > 0 else DEFAULT_KP
    base_ki = self.torque_params.ki if self.torque_params.ki > 0 else DEFAULT_KI

    # Scale KP multipliers by the base KP from settings
    # At 30 m/s you get base_kp, at 1 m/s you get 250 * base_kp
    kp_interp = [m * base_kp for m in KP_MULTIPLIERS]

    # PID with speed-interpolated KP
    self.pid = PIDController([INTERP_SPEEDS, kp_interp], base_ki,
                             pos_limit=1.0, neg_limit=-1.0,
                             unwind_multiplier=UNWIND_MULTIPLIER)

    # Delay compensation buffer
    self.lat_accel_request_buffer_len = int(LAT_ACCEL_REQUEST_BUFFER_SECONDS / DT_CTRL)
    self.lat_accel_request_buffer = deque([0.] * self.lat_accel_request_buffer_len, maxlen=self.lat_accel_request_buffer_len)
    self.lookahead_frames = int(JERK_LOOKAHEAD_SECONDS / DT_CTRL)

    # Jerk filter
    self.jerk_filter = FirstOrderFilter(0.0, 1 / (2 * np.pi * LP_FILTER_CUTOFF_HZ), DT_CTRL)

    # Unwind detection state
    self.prev_desired_lateral_accel = 0.0

  def update_live_torque_params(self, latAccelFactor, latAccelOffset, friction):
    self.torque_params.latAccelFactor = latAccelFactor
    self.torque_params.latAccelOffset = latAccelOffset
    self.torque_params.friction = friction

  def update(self, active, CS, VM, params, steer_limited_by_controls, desired_curvature, curvature_limited, calibrated_pose, model_data, frogpilot_toggles):
    pid_log = log.ControlsState.LateralTorqueState.new_message()

    # Calculate current state
    measured_curvature = -VM.calc_curvature(math.radians(CS.steeringAngleDeg - params.angleOffsetDeg), CS.vEgo, params.roll)
    measurement = measured_curvature * CS.vEgo ** 2
    future_desired_lateral_accel = desired_curvature * CS.vEgo ** 2
    self.lat_accel_request_buffer.append(future_desired_lateral_accel)

    roll_compensation = params.roll * ACCELERATION_DUE_TO_GRAVITY
    curvature_deadzone = abs(VM.calc_curvature(math.radians(self.steering_angle_deadzone_deg), CS.vEgo, 0.0))
    lateral_accel_deadzone = curvature_deadzone * CS.vEgo ** 2

    # Delay compensation: compare against what we requested lat_delay ago
    delay_frames = int(np.clip(LAT_DELAY / DT_CTRL + 1, 1, self.lat_accel_request_buffer_len))
    expected_lateral_accel = self.lat_accel_request_buffer[-delay_frames]
    setpoint = expected_lateral_accel
    error = setpoint - measurement

    # Jerk calculation with lookahead and filtering
    lookahead_idx = int(np.clip(-delay_frames + self.lookahead_frames, -self.lat_accel_request_buffer_len + 1, -2))
    raw_lateral_jerk = (self.lat_accel_request_buffer[lookahead_idx + 1] - self.lat_accel_request_buffer[lookahead_idx - 1]) / (2 * DT_CTRL)
    desired_lateral_jerk = self.jerk_filter.update(raw_lateral_jerk)

    # Feedforward: gravity-adjusted desired lateral accel
    gravity_adjusted_future_lateral_accel = future_desired_lateral_accel - roll_compensation
    ff = gravity_adjusted_future_lateral_accel
    # latAccelOffset corrects roll compensation bias from device roll misalignment relative to car roll
    ff -= self.torque_params.latAccelOffset
    # Friction term with jerk gain (anticipates needed friction based on rate of change)
    ff += get_friction(error + JERK_GAIN * desired_lateral_jerk, lateral_accel_deadzone, FRICTION_THRESHOLD, self.torque_params)

    # StarPilot unwind detection: freeze integrator when exiting a turn
    desired_lateral_accel_rate = (setpoint - self.prev_desired_lateral_accel) / DT_CTRL
    unwind_detected = (desired_lateral_accel_rate < UNWIND_D_DES_THRESHOLD and
                       abs(setpoint) < UNWIND_LAT_ACCEL_NEAR_ZERO)
    self.prev_desired_lateral_accel = setpoint

    if not active:
      output_torque = 0.0
      pid_log.active = False
      self.pid.reset()
      self.prev_desired_lateral_accel = 0.0
      self.lat_accel_request_buffer = deque([0.] * self.lat_accel_request_buffer_len, maxlen=self.lat_accel_request_buffer_len)
    else:
      # Error correction in lateral acceleration space
      pid_log.error = float(error)

      # Freeze integrator conditions
      freeze_integrator = steer_limited_by_controls or CS.steeringPressed or CS.vEgo < 5 or unwind_detected

      # PID update in lat accel space
      output_lataccel = self.pid.update(pid_log.error, speed=CS.vEgo, feedforward=ff, freeze_integrator=freeze_integrator)

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
