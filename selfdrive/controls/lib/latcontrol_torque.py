import csv
import math
import os
import time
import numpy as np
from collections import deque

from cereal import log
from opendbc.car.lateral import get_friction
from opendbc.car.hyundai.hyundaicanfd import DAMP_FACTOR_SPEED, DAMP_FACTOR, DAMP_UNWIND_BOOST_SPEED, DAMP_UNWIND_BOOST
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
KP_MULTIPLIERS = [250,   120,   65,    22,    7,     4.0,   2.5,   1.0,   0.7  ]
LOW_SPEED_Y =    [3.0,   2.8,   2.5,   2.5,   1.6,   1.4,   1.2,   1.0,   1.0  ]
LOW_SPEED_X =    [1,     1.5,   2.0,   3.0,   5,     7.5,   10,    15,    30   ]

# === Delay Compensation ===
LAT_ACCEL_REQUEST_BUFFER_SECONDS = 1.0
# Note: Initial lat_delay is calculated in __init__ as CP.steerActuatorDelay + 0.2 (matching lagd)

# === Unwind Detection (steering-angle-based, mirrors carcontroller.py) ===
UNWIND_FRAMES_ACTIVATE = 5         # Counter threshold to activate decay
UNWIND_COUNTER_MAX = 15            # Max counter value; once reached, needs 10 false frames to deactivate

# === Integrator Decay ===
UNWIND_MULTIPLIER = 0.95  # PID built-in: decays integrator when error opposes it (centering after turns)


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

    # Store current KP/KI/KF for live update detection
    self.current_kp = 0.0
    self.current_ki = 0.0
    self.current_kf = 1.0

    # Initialize PID (will be configured in update_pid_gains)
    # ki_deadband=0.05: stop integrator accumulation when lateral accel error < 0.05 m/s²
    # This prevents slow I-term drift from tiny noise at highway speed without affecting turns
    # (real turns produce errors of 0.2+ m/s², well above the deadband)
    self.pid = PIDController(DEFAULT_KP, DEFAULT_KI, k_f=1.0,
                             pos_limit=1e308, neg_limit=-1e308,
                             unwind_multiplier=UNWIND_MULTIPLIER,
                             ki_deadband=0.05)
    self.update_pid_gains()

    # Delay compensation buffer
    self.lat_accel_request_buffer_len = int(LAT_ACCEL_REQUEST_BUFFER_SECONDS / DT_CTRL)
    self.lat_accel_request_buffer = deque([0.] * self.lat_accel_request_buffer_len, maxlen=self.lat_accel_request_buffer_len)

    # Live lateral delay from lagd (updated by controlsd via update_live_delay)
    # Initial value: actuator delay + estimated processing delay (~0.06-0.1)
    # Will be replaced by lagd's learned value once available
    self.lat_delay = CP.steerActuatorDelay + 0.1

    # Unwind detection state (steering-angle-based, mirrors carcontroller.py)
    self.unwind_peak_angle = 0.0
    self.unwind_last_angle = 0.0
    self.unwind_frames = 0
    self.unwind_hold_timer = 0

    # Mirror of carcontroller.py's DAMP_UNWIND_BOOST detection (for logging only)
    self._damp_peak_angle = 0.0
    self._damp_last_angle = 0.0
    self._damp_unwind_frames = 0
    self._damp_hold_timer = 0
    self._damp_boost_active = False

    # Low-pass filter state for measured lateral accel
    self.filtered_measurement = 0.0
    self.low_pass_alpha = 1.0  # 1.0 = no filtering (off)

    # Feedforward low-pass filter: smooths the FF path to prevent model noise
    # from spiking torque, while allowing full FF amplitude for sustained turns
    self.filtered_ff = 0.0
    self.ff_filter_alpha = 0.15  # ~67ms time constant at 100Hz

    # Unwind diagnostic logging state
    self._unwind_log_active = False
    self._unwind_log_file = None
    self._unwind_log_writer = None
    self._unwind_log_path = None
    self._unwind_log_driver_touched = False
    self._unwind_log_start_time = 0.0
    self._unwind_log_last_above = 0.0


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
      base_kf = frogpilot_toggles.steerKf if hasattr(frogpilot_toggles, 'steerKf') else 1.0
      base_ki = frogpilot_toggles.steerKi if hasattr(frogpilot_toggles, 'steerKi') else DEFAULT_KI
      # Low-pass filter alpha: 0 = off (use 1.0 passthrough), otherwise use the set value
      alpha = getattr(frogpilot_toggles, 'lowPassFilterAlpha', 0.0)
      self.low_pass_alpha = 1.0 if alpha == 0.0 else alpha
    else:
      # Fall back to torque_params (startup values)
      base_kp = self.torque_params.kp if self.torque_params.kp > 0 else DEFAULT_KP
      base_kf = 1.0
      base_ki = self.torque_params.ki if self.torque_params.ki > 0 else DEFAULT_KI

    # Only update if gains changed
    if base_kp != self.current_kp or base_kf != self.current_kf or base_ki != self.current_ki:
      self.current_kp = base_kp
      self.current_kf = base_kf
      self.current_ki = base_ki

      # Scale KP multipliers by the base KP from settings
      kp_interp = [m * base_kp for m in KP_MULTIPLIERS]

      # Update PID gains
      self.pid._k_p = [INTERP_SPEEDS, kp_interp]
      self.pid.k_f = base_kf
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

    # Unwind detection: steering-angle-based (mirrors carcontroller.py)
    # Detects when the wheel is returning toward center from a turn
    steering_angle = CS.steeringAngleDeg
    abs_steer = abs(steering_angle)
    # Track peak angle during a turn
    if abs_steer > self.unwind_peak_angle:
      self.unwind_peak_angle = abs_steer
    # Unwind condition: angle decreasing from a real turn (>5 deg peak), same sign
    unwind_condition = (self.unwind_peak_angle > 5.0 and
                        abs_steer < abs(self.unwind_last_angle) and
                        (np.sign(steering_angle) == np.sign(self.unwind_last_angle) if self.unwind_last_angle != 0 else False))
    # Hysteresis counter
    if unwind_condition:
      self.unwind_frames = min(self.unwind_frames + 1, UNWIND_COUNTER_MAX)
    else:
      self.unwind_frames = max(self.unwind_frames - 1, 0)
    unwind_active = self.unwind_frames >= UNWIND_FRAMES_ACTIVATE
    # Winding up (turn entry): cancel immediately
    winding_up = abs_steer > abs(self.unwind_last_angle) + 0.5 and abs_steer > 5.0
    if winding_up:
      self.unwind_hold_timer = 0
      self.unwind_frames = 0
    elif unwind_active:
      self.unwind_hold_timer = int(3.0 / DT_CTRL)  # 3 second hold
    elif self.unwind_hold_timer > 0:
      self.unwind_hold_timer -= 1
    # Reset peak near center
    if abs_steer < 2.0:
      self.unwind_peak_angle = 0.0
    unwind_detected = self.unwind_hold_timer > 0
    self.unwind_last_angle = steering_angle

    # Calculate current state
    measured_curvature = -VM.calc_curvature(math.radians(CS.steeringAngleDeg - params.angleOffsetDeg), CS.vEgo, params.roll)
    raw_measurement = measured_curvature * CS.vEgo ** 2
    # Low-pass filter: alpha=1.0 is passthrough (off), lower alpha = stronger smoothing
    self.filtered_measurement += self.low_pass_alpha * (raw_measurement - self.filtered_measurement)
    measurement = self.filtered_measurement
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
    # Clamped so the jerk offset can never exceed |future_desired|:
    #   - Turn entry: prediction can at most double the setpoint (good anticipation)
    #   - Turn exit: prediction can at most zero the setpoint (no sign reversal / overshoot)
    #   - Straights: with future ≈ 0, jerk offset is clamped to ≈ 0 (no noise amplification)
    # Faded out below 8 m/s (18 mph) where high KP already amplifies noise — jerk just adds wobble
    jerk_offset = desired_lateral_jerk * lat_delay
    # Clamp to ±50% of |future|: turn entry gets up to 1.5× anticipation (still fast),
    # turn exit setpoint can only drop to 0.5× future (never reaches zero prematurely,
    # so the integrator doesn't build opposite-sign energy that causes centering wobble)
    jerk_offset = float(np.clip(jerk_offset, -abs(future_desired_lateral_accel) * 0.5, abs(future_desired_lateral_accel) * 0.5))
    jerk_fade = float(np.clip((CS.vEgo - 3.0) / 5.0, 0.0, 1.0))  # 0 below 3 m/s, 1 above 8 m/s
    jerk_offset *= jerk_fade
    setpoint = future_desired_lateral_accel + jerk_offset

    # Low speed factor: curvature-proportional boost for turns at low speeds
    # Works alongside KP_INTERP to help with tight turns at low speed
    low_speed_factor = float(np.interp(CS.vEgo, LOW_SPEED_X, LOW_SPEED_Y))

    # Straight-stop suppression: scale low_speed_factor toward 1.0 when near-straight and slow.
    # Prevents friction from snapping left/right at sub-6mph stops without affecting turns —
    # curve_scale rises from 0 → 1 as desired_curvature crosses STRAIGHT_STOP_CURVATURE,
    # so full low_speed_factor is restored by the time a real turn begins.
    # Use max of rate-limited and raw model curvature so that departing a stop into a turn
    # isn't suppressed while desired_curvature is still ramping up from zero.
    if CS.vEgo < STRAIGHT_STOP_SPEED and low_speed_factor > 1.0:
      model_curvature = abs(model_data.action.desiredCurvature) if model_data is not None else 0.0
      effective_curvature = max(abs(desired_curvature), model_curvature)
      curve_scale = float(np.clip(effective_curvature / STRAIGHT_STOP_CURVATURE, 0.0, 1.0))
      low_speed_factor = 1.0 + (low_speed_factor - 1.0) * curve_scale

    setpoint *= low_speed_factor
    measurement *= low_speed_factor

    error = setpoint - measurement

    # Feedforward: gravity-adjusted desired lateral accel
    gravity_adjusted_future_lateral_accel = future_desired_lateral_accel - roll_compensation
    ff_raw = gravity_adjusted_future_lateral_accel
    # latAccelOffset corrects roll compensation bias from device roll misalignment relative to car roll
    ff_raw -= self.torque_params.latAccelOffset
    # Low-pass filter on FF path: smooths model noise spikes while allowing full FF amplitude
    # for sustained turns. Only applied above 40 mph where model oscillation causes highway weave.
    highway_ff_fade = float(np.clip((CS.vEgo - 15.6) / 2.2, 0.0, 1.0))  # 0 below 35mph, 1 above 40mph
    if highway_ff_fade > 0.0:
      self.filtered_ff += self.ff_filter_alpha * (ff_raw - self.filtered_ff)
      ff = highway_ff_fade * self.filtered_ff + (1.0 - highway_ff_fade) * ff_raw
    else:
      ff = ff_raw
      self.filtered_ff = ff_raw  # Track raw value so filter doesn't lag when fading in
    # Friction term (ACTS-HORIZON style: no jerk in friction, jerk is in setpoint instead)
    # Use speed-interpolated threshold: lower at low speed (helps turns), higher at highway (prevents ticking)
    friction_threshold = get_friction_threshold(CS.vEgo)
    ff += get_friction(error, lateral_accel_deadzone, friction_threshold, self.torque_params)

    if not active:
      output_torque = 0.0
      pid_log.active = False
      self.pid.reset()
      self.unwind_frames = 0
      self.unwind_hold_timer = 0
      self.unwind_peak_angle = 0.0
      self.unwind_last_angle = 0.0
      self.filtered_measurement = 0.0
      self.filtered_ff = 0.0
      self.lat_accel_request_buffer = deque([0.] * self.lat_accel_request_buffer_len, maxlen=self.lat_accel_request_buffer_len)
    else:
      # Error correction in lateral acceleration space
      pid_log.error = float(error)

      # Freeze integrator conditions (unwind_detected NOT included — decay alone handles turn exit,
      # freezing creates a hard on/off boundary that causes "section" oscillation)
      freeze_integrator = steer_limited_by_controls or CS.steeringPressed or CS.vEgo < 1.5

      # PID update in lat accel space
      output_lataccel = self.pid.update(pid_log.error, speed=CS.vEgo, feedforward=ff, freeze_integrator=freeze_integrator)

      if unwind_detected:
        # Fade out desired curvature during unwind at low speeds
        # 0 mph -> 0.0 (full fade), 15 mph -> 0.4, 33 mph -> 1.0 (no fade)
        unwind_fade = float(np.interp(CS.vEgo, [0.0, 6.7, 14.7], [0.0, 0.4, 1.0]))
        output_lataccel *= unwind_fade

        # Gently decay integrator during turn exit (not frozen — integrator still accumulates above,
        # so this is a net drain that smoothly bleeds off turn-exit buildup without hard on/off steps)
        # Faster decay at low speed (turns complete faster, integrator needs to drain quicker)
        # 0.88 at 0 m/s → 0.95 at 15 m/s (33 mph)
        unwind_decay = float(np.interp(CS.vEgo, [0, 15], [0.88, 0.95]))
        self.pid.i *= unwind_decay

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

    # --- Unwind diagnostic logging (turns > 120° steering) ---
    try:
      abs_angle = abs(CS.steeringAngleDeg)
      now = time.monotonic()

      # Mirror carcontroller.py's DAMP_UNWIND_BOOST detection for logging
      steer_ang = CS.steeringAngleDeg
      abs_sa = abs(steer_ang)
      if abs_sa > self._damp_peak_angle:
        self._damp_peak_angle = abs_sa
      damp_unwind_cond = (self._damp_peak_angle > 5.0 and
                          abs_sa < abs(self._damp_last_angle) and
                          (np.sign(steer_ang) == np.sign(self._damp_last_angle) if self._damp_last_angle != 0 else False))
      if damp_unwind_cond:
        self._damp_unwind_frames = min(self._damp_unwind_frames + 1, 15)
      else:
        self._damp_unwind_frames = max(self._damp_unwind_frames - 1, 0)
      damp_unwind_active = self._damp_unwind_frames >= 5
      damp_winding_up = abs_sa > abs(self._damp_last_angle) + 0.5 and abs_sa > 5.0
      if damp_winding_up:
        self._damp_hold_timer = 0
        self._damp_unwind_frames = 0
      elif damp_unwind_active:
        self._damp_hold_timer = int(3.0 / DT_CTRL)
      elif self._damp_hold_timer > 0:
        self._damp_hold_timer -= 1
      if abs_sa < 2.0:
        self._damp_peak_angle = 0.0
      self._damp_boost_active = self._damp_hold_timer > 0
      self._damp_last_angle = steer_ang

      # Start logging when steering exceeds 120°
      if not self._unwind_log_active and abs_angle > 120.0:
        self._unwind_log_active = True
        self._unwind_log_driver_touched = False
        self._unwind_log_start_time = now
        self._unwind_log_last_above = now
        self._unwind_log_path = os.path.join("/data/media", f"unwind_{int(now)}.csv")
        self._unwind_log_file = open(self._unwind_log_path, 'w', newline='')
        self._unwind_log_writer = csv.writer(self._unwind_log_file)
        self._unwind_log_writer.writerow([
          'time_s', 'steering_angle_deg', 'speed_mph', 'torque_request',
          'pid_p', 'pid_i', 'pid_f', 'error', 'setpoint', 'measurement',
          'desired_curvature', 'unwind_detected', 'unwind_decay', 'damp_boost_active', 'damp_factor',
        ])

      # Write a row each frame while logging
      if self._unwind_log_active and self._unwind_log_writer is not None:
        if CS.steeringPressed:
          self._unwind_log_driver_touched = True

        # Reset hold timer while angle is still large
        if abs_angle > 120.0:
          self._unwind_log_last_above = now

        decay_val = float(np.interp(CS.vEgo, [0, 15], [0.88, 0.95])) if unwind_detected else 1.0
        base_damp = int(np.interp(CS.vEgo, DAMP_FACTOR_SPEED, DAMP_FACTOR))
        boost_damp = int(np.interp(CS.vEgo, DAMP_UNWIND_BOOST_SPEED, DAMP_UNWIND_BOOST)) if self._damp_boost_active else 0
        computed_damp = min(base_damp + boost_damp, 200)
        self._unwind_log_writer.writerow([
          f"{now:.3f}",
          f"{CS.steeringAngleDeg:.2f}",
          f"{CS.vEgo * 2.237:.1f}",
          f"{output_torque:.4f}",
          f"{self.pid.p:.4f}",
          f"{self.pid.i:.4f}",
          f"{self.pid.f:.4f}",
          f"{error:.4f}",
          f"{setpoint:.4f}",
          f"{measurement:.4f}",
          f"{desired_curvature:.6f}",
          int(unwind_detected),
          f"{decay_val:.3f}",
          int(self._damp_boost_active),
          computed_damp,
        ])
        self._unwind_log_file.flush()

        # Close: 5 seconds after angle drops below 120°, or 60s max safety timeout
        hold_expired = (now - self._unwind_log_last_above) > 5.0
        timed_out = (now - self._unwind_log_start_time) > 60.0

        if hold_expired or timed_out:
          self._unwind_log_active = False
          self._unwind_log_file.close()
          self._unwind_log_file = None
          self._unwind_log_writer = None
          # TODO: re-enable deletion after debugging
          # if self._unwind_log_driver_touched and self._unwind_log_path:
          #   os.remove(self._unwind_log_path)
          self._unwind_log_path = None
    except Exception:
      pass  # Never let logging crash the control loop

    # TODO left is positive in this convention
    return -output_torque, 0.0, pid_log
