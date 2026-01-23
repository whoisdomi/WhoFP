import numpy as np
from openpilot.common.constants import ACCELERATION_DUE_TO_GRAVITY
from openpilot.common.realtime import DT_CTRL, DT_MDL

MIN_SPEED = 0.3
CONTROL_N = 17
CAR_ROTATION_RADIUS = 0.0
# Increased to 0.4 to allow very tight turns (2.5m minimum radius)
MAX_CURVATURE = 0.4
MAX_VEL_ERR = 5.0  # m/s

# Modified for sportier driving - 33% above ISO comfort standard
MAX_LATERAL_JERK = 5.0  # m/s^3
MAX_LATERAL_ACCEL_NO_ROLL = 4.0  # m/s^2


def clamp(val, min_val, max_val):
  clamped_val = float(np.clip(val, min_val, max_val))
  return clamped_val, clamped_val != val

def smooth_value(val, prev_val, tau, dt=DT_MDL):
  alpha = 1 - np.exp(-dt/tau) if tau > 0 else 1
  return alpha * val + (1 - alpha) * prev_val

def clip_curvature(v_ego, prev_curvature, new_curvature, roll, jerk_factor=1.0, lat_accel_factor=1.0):
  # This function respects ISO lateral jerk and acceleration limits + a max curvature
  # jerk_factor: 0.1-1.0 (lower = smoother steering transitions)
  # lat_accel_factor: 0.1-1.0 (lower = longer lane changes)
  v_ego = max(v_ego, MIN_SPEED)

  # Reduce jerk limit for smoother steering transitions
  effective_jerk = MAX_LATERAL_JERK * jerk_factor
  max_curvature_rate = effective_jerk / (v_ego ** 2)  # inexact calculation, check https://github.com/commaai/openpilot/pull/24755

  # Symmetric rate limits for winding and unwinding (removed 10x unwind multiplier
  # to prevent steering from "letting go" during sharp turns)
  new_curvature = np.clip(new_curvature,
                          prev_curvature - max_curvature_rate * DT_CTRL,
                          prev_curvature + max_curvature_rate * DT_CTRL)

  # Reduce lateral acceleration limit for longer lane changes
  effective_lat_accel = MAX_LATERAL_ACCEL_NO_ROLL * lat_accel_factor
  roll_compensation = roll * ACCELERATION_DUE_TO_GRAVITY
  max_lat_accel = effective_lat_accel + roll_compensation
  min_lat_accel = -effective_lat_accel + roll_compensation
  new_curvature, limited_accel = clamp(new_curvature, min_lat_accel / v_ego ** 2, max_lat_accel / v_ego ** 2)

  new_curvature, limited_max_curv = clamp(new_curvature, -MAX_CURVATURE, MAX_CURVATURE)
  return float(new_curvature), limited_accel or limited_max_curv


def get_accel_from_plan(speeds, accels, t_idxs, action_t=DT_MDL, vEgoStopping=0.05):
  if len(speeds) == len(t_idxs):
    v_now = speeds[0]
    a_now = accels[0]
    v_target = np.interp(action_t, t_idxs, speeds)
    a_target = 2 * (v_target - v_now) / (action_t) - a_now
    v_target_1sec = np.interp(action_t + 1.0, t_idxs, speeds)
  else:
    v_target = 0.0
    v_target_1sec = 0.0
    a_target = 0.0
  should_stop = (v_target < vEgoStopping and
                 v_target_1sec < vEgoStopping)
  return a_target, should_stop

def curv_from_psis(psi_target, psi_rate, vego, action_t):
  vego = np.clip(vego, MIN_SPEED, np.inf)
  curv_from_psi = psi_target / (vego * action_t)
  return 2*curv_from_psi - psi_rate / vego

def get_curvature_from_plan(yaws, yaw_rates, t_idxs, vego, action_t):
  psi_target = np.interp(action_t, t_idxs, yaws)
  psi_rate = yaw_rates[0]
  return curv_from_psis(psi_target, psi_rate, vego, action_t)
