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

# Speed-interpolated KP: aggressive at low speed, tapering to base KP at highway
# The actual base KP (rightmost value) is set dynamically from torque_params in __init__
INTERP_SPEEDS = [1, 1.5, 2.0, 3.0, 5, 7.5, 10, 15, 30]
# Multipliers relative to base KP (last value = 1.0x base)
KP_MULTIPLIERS = [250, 120, 65, 30, 11.5, 5.5, 3.5, 2.0, 1.0]

# === Delay Compensation ===
LAT_ACCEL_REQUEST_BUFFER_SECONDS = 1.0
# Note: Initial lat_delay is calculated in __init__ as CP.steerActuatorDelay + 0.2 (matching lagd)

# === Unwind Detection (from StarPilot) ===
UNWIND_D_DES_THRESHOLD = -1.0      # Desired accel decreasing fast (m/s³)
UNWIND_LAT_ACCEL_NEAR_ZERO = 0.3   # Near straight (m/s²)

# === Integrator Decay ===
UNWIND_MULTIPLIER = 0.85  # Integrator decay when unwinding (0.85 = 15% decay per cycle)

# === Low Speed Factor (curvature-based boost for turns) ===
# Disabled - KP_INTERP alone handles low-speed boost (ACTS-HORIZON approach)
LOW_SPEED_X = [0, 10, 20, 30]  # m/s breakpoints
LOW_SPEED_Y = [2.0, 1.5, 1.0, 1.0]

# === Friction Threshold (from StarPilot) ===
# Speed-interpolated: lower at low speed (friction kicks in sooner for turns),
# higher at highway (friction needs bigger error to kick in, prevents ticking)
FRICTION_THRESHOLD_SPEEDS = [0.5, 33.5]  # m/s (approx 1 mph to 75 mph)
FRICTION_THRESHOLD_VALUES = [0.12, 0.2]   # threshold values


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
    self.pid = PIDController(DEFAULT_KP, DEFAULT_KI,
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
    self.prev_desired_lateral_accel = 0.0

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

    # Jerk calculation (ACTS-HORIZON style: simple difference / delay)
    desired_lateral_jerk = (future_desired_lateral_accel - expected_lateral_accel) / lat_delay

    # Setpoint with jerk prediction (ACTS-HORIZON formula)
    # Predicts where lateral accel will be after the delay period
    setpoint = lat_delay * desired_lateral_jerk + expected_lateral_accel

    # Low speed factor: curvature-proportional boost for turns at low speeds
    # Works alongside KP_INTERP to help with tight turns at low speed
    low_speed_factor = float(np.interp(CS.vEgo, LOW_SPEED_X, LOW_SPEED_Y))
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
