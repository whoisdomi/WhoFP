#!/usr/bin/env python3
from openpilot.common.constants import CV
from openpilot.common.realtime import DT_MDL
from openpilot.selfdrive.controls.lib.longitudinal_mpc_lib.long_mpc import COMFORT_BRAKE

from openpilot.frogpilot.common.frogpilot_variables import CRUISING_SPEED, PLANNER_TIME
from openpilot.frogpilot.controls.lib.curve_speed_controller import CurveSpeedController
from openpilot.frogpilot.controls.lib.speed_limit_controller import SpeedLimitController

OVERRIDE_FORCE_STOP_TIMER = 10

class FrogPilotVCruise:
  def __init__(self, FrogPilotPlanner):
    self.frogpilot_planner = FrogPilotPlanner

    self.csc = CurveSpeedController(self)
    self.slc = SpeedLimitController(self)

    self.forcing_stop = False
    self.override_force_stop = False

    self.override_force_stop_timer = 0

  def update(self, long_control_active, now, time_validated, v_cruise, v_ego, sm, frogpilot_toggles):
    # Normal force stop condition (requires toggle + model_stopped)
    force_stop = self.frogpilot_planner.frogpilot_cem.stop_light_detected and long_control_active and frogpilot_toggles.force_stops
    force_stop &= self.frogpilot_planner.model_stopped
    force_stop &= self.override_force_stop_timer <= 0

    # Manual Stop Ahead can trigger force stop immediately when light is detected
    # (bypass model_stopped and force_stops toggle since user indicated stop ahead)
    manual_stop_force_stop = sm["frogpilotCarState"].manualStopAhead and self.frogpilot_planner.frogpilot_cem.stop_light_detected
    manual_stop_force_stop &= long_control_active
    manual_stop_force_stop &= self.override_force_stop_timer <= 0
    manual_stop_force_stop &= not self.frogpilot_planner.tracking_lead

    self.force_stop_timer = self.force_stop_timer + DT_MDL if force_stop else 0

    # Manual Stop Ahead bypasses the 1-second timer for immediate handoff to Force Stop
    force_stop_enabled = self.force_stop_timer >= 1 or manual_stop_force_stop
    # Latch: once committed to stopping, stay committed until standstill
    force_stop_enabled |= self.forcing_stop and not sm["carState"].standstill

    self.override_force_stop |= sm["carState"].gasPressed
    self.override_force_stop |= sm["frogpilotCarState"].accelPressed
    self.override_force_stop &= force_stop_enabled

    if self.override_force_stop:
      self.override_force_stop_timer = OVERRIDE_FORCE_STOP_TIMER
    elif self.override_force_stop_timer > 0:
      self.override_force_stop_timer -= DT_MDL

    v_cruise_cluster = max(sm["carState"].vCruiseCluster * CV.KPH_TO_MS, v_cruise)
    v_cruise_diff = v_cruise_cluster - v_cruise

    v_ego_cluster = max(sm["carState"].vEgoCluster, v_ego)
    v_ego_diff = v_ego_cluster - v_ego

    # For CSC/SLC to work with ICBM, we need them to run when either:
    # 1. openpilot has longitudinal control (long_control_active), OR
    # 2. ICBM is enabled and cruise is engaged (use cruiseState for non-longitudinal cars)
    speed_control_active = long_control_active or (frogpilot_toggles.icbm_enabled and sm["carState"].cruiseState.enabled)

    # FrogsGoMoo's Curve Speed Controller
    if speed_control_active and v_ego > CRUISING_SPEED and self.frogpilot_planner.road_curvature_detected and frogpilot_toggles.curve_speed_controller:
      self.csc.update_target(v_ego)

      self.csc_controlling_speed = True

      self.csc_target = self.csc.target
    else:
      self.csc.log_data(speed_control_active, v_ego, sm)

      self.csc_controlling_speed = False
      self.csc.target_set = False

      self.csc_target = v_cruise

    # Pfeiferj's Speed Limit Controller
    self.slc.frogpilot_toggles = frogpilot_toggles

    if frogpilot_toggles.speed_limit_controller:
      self.slc.update_limits(sm["frogpilotCarState"].dashboardSpeedLimit, now, time_validated, v_cruise, v_ego, sm)
      self.slc.update_override(v_cruise, v_cruise_diff, v_ego, v_ego_diff, sm)

      self.slc_offset = self.slc.offset
      self.slc_target = self.slc.target
    elif frogpilot_toggles.show_speed_limits:
      self.slc.update_limits(sm["frogpilotCarState"].dashboardSpeedLimit, now, time_validated, v_cruise, v_ego, sm)

      self.slc_offset = 0
      self.slc_target = self.slc.target
    else:
      self.slc_offset = 0
      self.slc_target = 0

    if force_stop_enabled and not self.override_force_stop:
      self.forcing_stop |= not sm["carState"].standstill

      self.tracked_model_length = max(self.tracked_model_length - (v_ego * DT_MDL), 0)
      v_cruise = min((self.tracked_model_length // PLANNER_TIME), v_cruise)

    else:
      self.forcing_stop = False

      self.tracked_model_length = self.frogpilot_planner.model_length

      # For ICBM, we need the actual target speed (not clamped to current set speed)
      # because ICBM can both increase AND decrease the set speed
      if frogpilot_toggles.icbm_enabled:
        # Start with current set speed as default
        icbm_target = v_cruise

        # Use SLC target if available and valid
        # Note: For ICBM, we don't use overridden_speed because we want to allow
        # both increasing AND decreasing speed based on actual speed limits
        if frogpilot_toggles.speed_limit_controller and self.slc_target > 0:
          icbm_target = self.slc_target + self.slc_offset

        # Use CSC target if it's actively controlling and lower than current target
        if self.csc_controlling_speed and self.csc_target >= CRUISING_SPEED:
          icbm_target = min(icbm_target, self.csc_target)

        v_cruise = icbm_target
      else:
        # Original logic for openpilot longitudinal (takes minimum to slow down)
        targets = [self.csc_target, v_cruise]
        if frogpilot_toggles.speed_limit_controller:
          targets.append(max(self.slc.overridden_speed, self.slc_target + self.slc_offset) - v_ego_diff)
        v_cruise = min([target if target >= CRUISING_SPEED else v_cruise for target in targets])

    # Manual Stop Ahead: gradually reduce v_cruise until Force Stop takes over
    # Once stop_light_detected, Force Stop handles deceleration (via manual_stop_force_stop above)
    if sm["frogpilotCarState"].manualStopAhead and not self.frogpilot_planner.tracking_lead:
      if not self.frogpilot_planner.frogpilot_cem.stop_light_detected:
        # Model hasn't detected stop yet - do gradual deceleration
        self.manual_stop_ahead_v_cruise = getattr(self, 'manual_stop_ahead_v_cruise', v_ego)
        self.manual_stop_ahead_v_cruise = max(self.manual_stop_ahead_v_cruise - (1.2 * DT_MDL), 0)
        v_cruise = min(v_cruise, self.manual_stop_ahead_v_cruise)
      # else: Force Stop is handling it via force_stop_enabled block above
    else:
      self.manual_stop_ahead_v_cruise = v_ego

    return v_cruise
