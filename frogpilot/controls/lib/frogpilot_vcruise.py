#!/usr/bin/env python3
from openpilot.common.constants import CV
from openpilot.common.realtime import DT_MDL

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

    self.force_stop_timer = 0
    self.green_light_timer = 0
    self.override_force_stop_timer = 0
    self.tracked_model_length = 0

  def update(self, long_control_active, now, time_validated, v_cruise, v_ego, sm, frogpilot_toggles):
    # Normal force stop condition (requires toggle + model_stopped)
    force_stop = self.frogpilot_planner.frogpilot_cem.stop_light_detected and long_control_active and frogpilot_toggles.force_stops
    force_stop &= self.frogpilot_planner.model_stopped
    force_stop &= self.override_force_stop_timer <= 0
    # Don't activate force stop mid-turn — model trajectory shortens in curves, causing false triggers
    force_stop &= not self.frogpilot_planner.driving_in_curve

    # Manual Stop Ahead can trigger force stop immediately when light is detected
    # (bypass model_stopped and force_stops toggle since user indicated stop ahead)
    manual_stop_force_stop = sm["frogpilotCarState"].manualStopAhead and self.frogpilot_planner.frogpilot_cem.stop_light_detected
    manual_stop_force_stop &= long_control_active
    manual_stop_force_stop &= self.override_force_stop_timer <= 0
    manual_stop_force_stop &= not self.frogpilot_planner.tracking_lead

    # Gradual decay instead of instant reset — brief model flickers won't derail the timer.
    # Don't accumulate at standstill: CEM pauses there, keeping stop_light_detected stale,
    # which would peg the timer at max and prevent auto-release when the light turns green.
    if force_stop and not sm["carState"].standstill:
      self.force_stop_timer = min(self.force_stop_timer + DT_MDL, 2.0)
    else:
      self.force_stop_timer = max(self.force_stop_timer - (DT_MDL * 0.25), 0)

    # Dashboard stop sign failsafe: car's own camera confirms a stop sign — bypass the timer.
    # Uses model_stopped (weaker: path ends within ~50m) rather than stop_light_detected
    # (which requires the full filter to cross threshold) so it can act as an early trigger.
    dashboard_stop_force_stop = sm["frogpilotCarState"].dashboardStopSign and long_control_active and frogpilot_toggles.force_stops
    dashboard_stop_force_stop &= self.frogpilot_planner.model_stopped
    dashboard_stop_force_stop &= not self.frogpilot_planner.tracking_lead
    dashboard_stop_force_stop &= self.override_force_stop_timer <= 0

    # Manual Stop Ahead bypasses the 1-second timer for immediate handoff to Force Stop
    force_stop_enabled = self.force_stop_timer >= 0.5 or manual_stop_force_stop or dashboard_stop_force_stop

    # Latch: stay committed to stopping until standstill, but release if stop condition
    # has been sustainedly cleared (light genuinely turned green, not a brief model flicker)
    stop_cleared = not self.frogpilot_planner.frogpilot_cem.stop_light_detected and not self.frogpilot_planner.model_stopped
    self.green_light_timer = self.green_light_timer + DT_MDL if stop_cleared and self.forcing_stop else 0
    green_confirmed = self.green_light_timer >= 1.5
    force_stop_enabled |= self.forcing_stop and not sm["carState"].standstill and not green_confirmed

    # At standstill: CEM pauses so stop_light_detected is stale — use model_stopped directly.
    # When the model sees the path clear ahead (light turned green), model_stopped goes False
    # and force stop releases, allowing the car to resume on its own.
    if sm["carState"].standstill and self.forcing_stop:
      force_stop_enabled = self.frogpilot_planner.model_stopped

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
      # If the model refines its stop prediction to a closer point, follow it downward.
      # This fixes stop sign overshoot: the model initially predicts stopping at ~50m when
      # model_stopped triggers, but revises closer as the car approaches the intersection.
      # Without this, tracked_model_length stays anchored to the stale ~50m value and the
      # car stops past the model's updated (and more accurate) stop point.
      self.tracked_model_length = min(self.tracked_model_length, self.frogpilot_planner.model_length)
      # Dashboard camera stop sign distance: use the closer of model vs dashboard estimate.
      # BYTE22 counts down in meters as the car approaches the stop sign.
      dashboard_stop_dist = sm["frogpilotCarState"].dashboardStopSign
      if dashboard_stop_dist > 0:
        self.tracked_model_length = min(self.tracked_model_length, float(dashboard_stop_dist))
      if sm["carState"].standstill:
        self.tracked_model_length = 0
      # Floor division: when tracked_model_length < PLANNER_TIME (~10m), v_cruise becomes 0.0
      # exactly — giving the MPC a hard "stop now" command. Float division would approach 0
      # asymptotically and let the car roll slowly (v_ego ≈ v_cruise, MPC never brakes further).
      v_cruise = min(self.tracked_model_length // PLANNER_TIME, v_cruise)

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

    # Manual Stop Ahead: gradually reduce v_cruise continuously
    # Runs through force stop handoff - min() picks the tighter constraint
    if sm["frogpilotCarState"].manualStopAhead and not self.frogpilot_planner.tracking_lead:
      self.manual_stop_ahead_v_cruise = getattr(self, 'manual_stop_ahead_v_cruise', v_ego)
      self.manual_stop_ahead_v_cruise = max(self.manual_stop_ahead_v_cruise - (1.2 * DT_MDL), 0)
      v_cruise = min(v_cruise, self.manual_stop_ahead_v_cruise)
    else:
      self.manual_stop_ahead_v_cruise = v_ego

    return v_cruise
