#!/usr/bin/env python3
import os
import time

from openpilot.common.constants import CV
from openpilot.common.realtime import DT_MDL

from openpilot.frogpilot.common.frogpilot_variables import CRUISING_SPEED, PLANNER_TIME
from openpilot.frogpilot.controls.lib.curve_speed_controller import CurveSpeedController
from openpilot.frogpilot.controls.lib.speed_limit_controller import SpeedLimitController

OVERRIDE_FORCE_STOP_TIMER = 10
STOP_SIGN_LOG = "/data/stop_sign_overshoot.csv"
DASH_SIGNAL_LOG = "/data/dash_stop_sign_raw.csv"
M_TO_FT = 3.28084

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
    self.stop_sign_confirmed = False

    # Stop sign overshoot logging — simple continuous logger.
    # Logs every frame where dash stop sign OR force stop is active,
    # plus a 5-second tail after both go false to capture standstill.
    self._ss_log_file = None
    self._ss_log_tail = False
    self._ss_log_driving_timer = 0
    self._ss_init_log()

    # Raw dashboard stop sign signal logger — always-on, records every ON/OFF transition.
    # Used to verify the signal is firing vs what the event log captures.
    self._dash_raw_log_file = None
    self._dash_raw_prev = False
    self._dash_raw_init_log()

  def _dash_raw_init_log(self):
    try:
      with open(DASH_SIGNAL_LOG, "w") as f:
        f.write("time,event,speed_mph,model_ft\n")
    except Exception:
      pass

  def _dash_raw_log(self, v_ego, model_ft, dash_on):
    prev = self._dash_raw_prev
    if dash_on == prev:
      return
    self._dash_raw_prev = dash_on
    event = "ON" if dash_on else "OFF"
    speed_mph = v_ego * 2.237
    try:
      if self._dash_raw_log_file is None:
        self._dash_raw_log_file = open(DASH_SIGNAL_LOG, "a")
      self._dash_raw_log_file.write(f"{time.monotonic():.2f},{event},{speed_mph:.1f},{model_ft:.1f}\n")
      self._dash_raw_log_file.flush()
    except Exception:
      pass

  def _ss_init_log(self):
    if not os.path.exists(STOP_SIGN_LOG):
      with open(STOP_SIGN_LOG, "w") as f:
        f.write("time,speed_mph,tracked_ft,model_ft,dash,forcing,standstill,brake,confirmed,green_t,curve,force_t,cem,exp,lead\n")

  def _ss_log_frame(self, v_ego, sm):
    tracked_ft = self.tracked_model_length * M_TO_FT
    model_ft = self.frogpilot_planner.model_length * M_TO_FT
    speed_mph = v_ego * 2.237
    dash = 1 if sm["frogpilotCarState"].dashboardStopSign > 0 else 0
    forcing = 1 if self.forcing_stop else 0
    standstill = 1 if sm["carState"].standstill else 0
    brake = 1 if sm["carState"].brakePressed else 0
    confirmed = 1 if self.stop_sign_confirmed else 0
    green_t = self.green_light_timer
    curve = 1 if self.frogpilot_planner.driving_in_curve else 0
    force_t = self.force_stop_timer
    cem = 1 if self.frogpilot_planner.frogpilot_cem.stop_light_detected else 0
    exp = 1 if self.frogpilot_planner.frogpilot_cem.experimental_mode else 0
    lead = 1 if self.frogpilot_planner.tracking_lead else 0
    try:
      if self._ss_log_file is None:
        self._ss_log_file = open(STOP_SIGN_LOG, "a")
      self._ss_log_file.write(f"{time.monotonic():.2f},{speed_mph:.1f},{tracked_ft:.1f},{model_ft:.1f},{dash},{forcing},{standstill},{brake},{confirmed},{green_t:.2f},{curve},{force_t:.2f},{cem},{exp},{lead}\n")
      self._ss_log_file.flush()
    except Exception:
      pass

  def update(self, long_control_active, now, time_validated, v_cruise, v_ego, sm, frogpilot_toggles):
    # Normal force stop condition (requires toggle + model_stopped)
    force_stop = self.frogpilot_planner.frogpilot_cem.stop_light_detected and long_control_active and frogpilot_toggles.force_stops
    force_stop &= self.frogpilot_planner.model_stopped
    force_stop &= self.override_force_stop_timer <= 0
    # Don't activate force stop mid-turn — model trajectory shortens in curves, causing false triggers
    force_stop &= not self.frogpilot_planner.driving_in_curve

    # Manual Stop Ahead can trigger force stop when light is detected
    # (bypass model_stopped and force_stops toggle since user indicated stop ahead)
    # BUT only when close enough that Force Stop's decel profile won't be too aggressive.
    # When far away, Manual Stop's own v_cruise ramp handles the gentle coast/decel.
    # Gate: either model already predicts a stop (normal timing) OR kinematic check passes:
    #   comfortable decel threshold = v_ego² / (2 * model_length) ≤ 2.0 m/s²
    #   → model_length ≥ v_ego² / 4.0
    model_len = max(self.frogpilot_planner.model_length, 1.0)
    close_enough_for_force_stop = self.frogpilot_planner.model_stopped or (v_ego ** 2 / (2.0 * model_len)) <= 2.0
    manual_stop_force_stop = sm["frogpilotCarState"].manualStopAhead and self.frogpilot_planner.frogpilot_cem.stop_light_detected
    manual_stop_force_stop &= long_control_active
    manual_stop_force_stop &= self.override_force_stop_timer <= 0
    manual_stop_force_stop &= not self.frogpilot_planner.tracking_lead
    manual_stop_force_stop &= close_enough_for_force_stop

    # Dashboard stop sign confirmation from camera ECU (CAM_0x361 SIGN_TYPE == 15)
    dashboard_stop_sign = sm["frogpilotCarState"].dashboardStopSign > 0

    # Dashboard stop sign can trigger force stop directly — the ADAS camera has confirmed
    # a stop sign ahead on our road. No need to wait for model_stopped since the model
    # often sees through intersections at stop signs. The cap + decay handle the distance.
    dash_stop_sign_force_stop = dashboard_stop_sign and long_control_active and frogpilot_toggles.force_stops
    dash_stop_sign_force_stop &= self.override_force_stop_timer <= 0
    dash_stop_sign_force_stop &= not self.frogpilot_planner.driving_in_curve
    dash_stop_sign_force_stop &= not self.frogpilot_planner.tracking_lead

    # Latch stop_sign_confirmed as soon as dashboard sees the sign and force stop conditions
    # are met — don't wait for the 0.5s timer. The sign often leaves the camera FOV before
    # the timer completes, so waiting means CONFIRMED never gets set and the cap never applies.
    if dash_stop_sign_force_stop:
      self.stop_sign_confirmed = True

    # Gradual decay instead of instant reset — brief model flickers won't derail the timer.
    # Don't accumulate at standstill: CEM pauses there, keeping stop_light_detected stale,
    # which would peg the timer at max and prevent auto-release when the light turns green.
    if (force_stop or dash_stop_sign_force_stop) and not sm["carState"].standstill:
      # Dashboard confirmation doubles accumulation rate for faster commitment
      rate = DT_MDL * 2 if dashboard_stop_sign else DT_MDL
      self.force_stop_timer = min(self.force_stop_timer + rate, 2.0)
    else:
      self.force_stop_timer = max(self.force_stop_timer - (DT_MDL * 0.25), 0)

    # Manual Stop Ahead bypasses the 1-second timer for immediate handoff to Force Stop
    force_stop_enabled = self.force_stop_timer >= 0.5 or manual_stop_force_stop

    # Latch: stay committed to stopping until standstill, but release if stop condition
    # has been sustainedly cleared (light genuinely turned green, not a brief model flicker).
    # Stop signs don't turn green — when dashboard confirms a stop sign, skip the green
    # light release entirely and stay committed until standstill or driver override.
    stop_cleared = not self.frogpilot_planner.frogpilot_cem.stop_light_detected and not self.frogpilot_planner.model_stopped
    self.green_light_timer = self.green_light_timer + DT_MDL if stop_cleared and self.forcing_stop else 0
    green_confirmed = self.green_light_timer >= 1.5 and not self.stop_sign_confirmed
    # Latch: once committed to stopping, don't let a transient curve detection break it.
    # driving_in_curve guards the *activation* conditions above — not the latch.
    force_stop_enabled |= self.forcing_stop and not sm["carState"].standstill and not green_confirmed

    # At standstill: CEM pauses so stop_light_detected is stale — use model_stopped directly.
    # When the model sees the path clear ahead (light turned green), model_stopped goes False
    # and force stop releases, allowing the car to resume on its own.
    # Exception: stop signs don't turn green — keep force stop held until driver presses gas.
    # (The model typically sees through stop sign intersections, so model_stopped is unreliable here.)
    if sm["carState"].standstill and self.forcing_stop and not self.stop_sign_confirmed:
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

      # Latch: once dashboard confirms a stop sign during this force stop event,
      # keep the correction active even after the sign leaves the dashboard.
      # The sign passes out of camera view as you approach, but you still need to stop.
      # Resets when force stop ends (else branch below).
      if dashboard_stop_sign:
        self.stop_sign_confirmed = True

      # Dashboard stop sign: accelerate decay to correct overshoot.
      # Good stops (model revises down): the model clamp below is the binding constraint, so this doesn't matter.
      # Bad stops (model sees through intersection): kinematic decay is all we have, so the boost pulls the stop closer.
      decay_multiplier = 1.5 if self.stop_sign_confirmed else 1.0
      self.tracked_model_length = max(self.tracked_model_length - (v_ego * DT_MDL * decay_multiplier), 0)

      # Dashboard stop sign cap: cap tracked_model_length to a comfortable stopping
      # distance for the current speed. Data shows the model is bimodal at stop signs —
      # either nails the stop point (~2-5 ft) or completely sees through the intersection
      # (40-103 ft overshoot). The model clamp handles the good case. This cap handles
      # the bad case by limiting how far ahead the stop point can be.
      #   d = v² / (2 * a_comfortable)  — stopping distance at comfortable decel
      #   + 6m buffer so we don't clip the good stops that are already working
      # At 15 mph (6.7 m/s): cap = 6.7²/3.0 + 6 = 21m = 69 ft
      # At 25 mph (11.2 m/s): cap = 11.2²/3.0 + 6 = 48m = 157 ft
      # At 30 mph (13.4 m/s): cap = 13.4²/3.0 + 6 = 66m = 216 ft
      if self.stop_sign_confirmed:
        stop_sign_cap = (v_ego ** 2) / 3.0 + 6.0
        self.tracked_model_length = min(self.tracked_model_length, stop_sign_cap)

      # If the model refines its stop prediction to a closer point, follow it downward.
      # This fixes stop sign overshoot: the model initially predicts stopping at ~50m when
      # model_stopped triggers, but revises closer as the car approaches the intersection.
      # Without this, tracked_model_length stays anchored to the stale ~50m value and the
      # car stops past the model's updated (and more accurate) stop point.
      #
      # However, skip the model clamp when:
      # - A lead is present: model_length gets truncated by the lead's position
      #   (model predicts it can't travel past the stopped car), causing premature stops.
      # - Manual Stop is active: the pre-deceleration from Manual Stop causes the model
      #   to predict very short travel distances, so model_length is artificially low by
      #   the time force stop activates. The kinematic decay is more accurate here.
      # In both cases the kinematic decay (v_ego * DT_MDL) handles the countdown correctly.
      if not self.frogpilot_planner.tracking_lead and not sm["frogpilotCarState"].manualStopAhead:
        self.tracked_model_length = min(self.tracked_model_length, self.frogpilot_planner.model_length)
      if sm["carState"].standstill:
        self.tracked_model_length = 0
      # Kinematic velocity profile: v = sqrt(2 * a_comfort * d)
      # Unlike the old linear profile (d/10), this keeps speed high when far away and
      # progressively increases braking as the stop approaches — matching how a human
      # driver would brake. a_comfort = 1.2 m/s² gives a natural, non-alarming decel.
      # Hard stop command below 18m (60ft) to give the MPC enough runway to arrest the
      # car before the stop line. Raised from 15m after data showed car stopping slightly
      # past the line — commanding v=0 earlier pulls the stop point 5-10ft closer.
      FORCE_STOP_COMFORT_DECEL = 1.2
      if self.tracked_model_length < 18.0:
        force_stop_v = 0.0
      else:
        force_stop_v = (2.0 * FORCE_STOP_COMFORT_DECEL * self.tracked_model_length) ** 0.5
      v_cruise = min(force_stop_v, v_cruise)

    else:
      self.forcing_stop = False
      self.stop_sign_confirmed = False

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

    # --- Stop sign overshoot logging ---
    # Raw signal log — always-on, records every ON/OFF transition of dashboardStopSign.
    self._dash_raw_log(v_ego, self.frogpilot_planner.model_length * M_TO_FT, sm["frogpilotCarState"].dashboardStopSign > 0)

    # Log every frame where dashboard stop sign or force stop is active.
    # Always end at standstill — that's when the car is physically at the sign,
    # regardless of when the dashboard sign goes off (it's inconsistent).
    # If no standstill, end when clearly driving away (30mph for 2+ seconds).
    active = sm["frogpilotCarState"].dashboardStopSign > 0 or self.forcing_stop
    if active:
      self._ss_log_tail = True
      self._ss_log_driving_timer = 0
    if self._ss_log_tail:
      self._ss_log_frame(v_ego, sm)
      if sm["carState"].standstill:
        self._ss_log_tail = False  # standstill = at the sign, always end here
      elif not active:
        if v_ego > 13.4:  # above 30mph
          self._ss_log_driving_timer += DT_MDL
          if self._ss_log_driving_timer > 2.0:
            self._ss_log_tail = False  # clearly drove through
        else:
          self._ss_log_driving_timer = 0

    # Manual Stop Ahead: gradually reduce v_cruise continuously
    # Runs through force stop handoff - min() picks the tighter constraint
    # Works with or without a lead — when tracking a lead, the MPC will
    # use the lower v_cruise to decelerate more aggressively.
    if sm["frogpilotCarState"].manualStopAhead:
      self.manual_stop_ahead_v_cruise = getattr(self, 'manual_stop_ahead_v_cruise', v_ego)
      # Distance-aware decel: gentle coast when far away, firmer as we approach.
      # Ideal constant decel to stop at model_length: a = v² / (2d)
      # Clamp between 0.3 (gentle coast) and 2.5 (firm but comfortable)
      dist = max(self.frogpilot_planner.model_length, 5.0)
      ideal_decel = (self.manual_stop_ahead_v_cruise ** 2) / (2.0 * dist)
      decel_rate = max(0.3, min(ideal_decel, 2.5))
      self.manual_stop_ahead_v_cruise = max(self.manual_stop_ahead_v_cruise - (decel_rate * DT_MDL), 0)
      v_cruise = min(v_cruise, self.manual_stop_ahead_v_cruise)
    else:
      self.manual_stop_ahead_v_cruise = v_ego

    return v_cruise
