#!/usr/bin/env python3
import time
import numpy as np

from openpilot.common.filter_simple import FirstOrderFilter
from openpilot.common.realtime import DT_MDL
from openpilot.common.constants import CV

from openpilot.frogpilot.common.frogpilot_variables import CRUISING_SPEED, THRESHOLD, scale_threshold

CEStatus = {
  "OFF": 0,
  "USER_DISABLED": 1,
  "USER_OVERRIDDEN": 2,
  "CURVATURE": 3,
  "LEAD": 4,
  "SIGNAL": 5,
  "SPEED": 6,
  "SPEED_LIMIT": 7,
  "STOP_LIGHT": 8
}

class ConditionalExperimentalMode:
  # ===== SPEED-BASED TUNING PARAMETERS =====
  # Speed ranges: [0, 35, 55, 70] mph

  # Filter time constants (Lower = More responsive, Higher = Smoother)
  FILTER_TIME_CURVES = [0.9, 0.8, 0.6, 0.5]
  FILTER_TIME_LEADS = [0.9, 0.8, 0.7, 0.5]
  FILTER_TIME_LIGHTS = [0.9, 0.8, 0.75, 0.55]

  # Light detection multipliers - boost detection range at certain speeds
  LIGHT_BOOSTS = [1.0, 1.2, 1.045, 1.0]
  LIGHT_MAX_TIME = 9

  # Minimum time (seconds) to hold experimental mode before allowing switch to chill
  EXPERIMENTAL_HOLD_TIME = 4.0

  # Default filter times
  FILTER_TIME_CURVE = 0.8
  FILTER_TIME_LEAD = 0.8
  FILTER_TIME_LIGHT = 0.8

  def __init__(self, FrogPilotPlanner):
    self.frogpilot_planner = FrogPilotPlanner

    self.curvature_filter = FirstOrderFilter(0, self.FILTER_TIME_CURVE, DT_MDL)
    self.slow_lead_filter = FirstOrderFilter(0, self.FILTER_TIME_LEAD, DT_MDL)
    self.stop_light_filter = FirstOrderFilter(0, self.FILTER_TIME_LIGHT, DT_MDL)

    self.curve_detected = False
    self.experimental_mode = False
    self.stop_light_detected = False
    self.slow_lead_detected = False
    self.prev_experimental_mode = False  # For hysteresis
    self.experimental_mode_since = 0.0  # Timestamp when experimental mode was last activated

  def update(self, v_ego, sm, frogpilot_toggles):
    if frogpilot_toggles.experimental_mode_via_press:
      self.status_value = self.frogpilot_planner.params_memory.get("CEStatus")
    else:
      self.status_value = CEStatus["OFF"]

    if self.status_value not in (CEStatus["USER_DISABLED"], CEStatus["USER_OVERRIDDEN"]) and not sm["carState"].standstill:
      self.update_conditions(v_ego, sm, frogpilot_toggles)
      new_experimental_mode = self.check_conditions(v_ego, sm, frogpilot_toggles)

      # Hysteresis to prevent rapid toggling
      if new_experimental_mode and not self.prev_experimental_mode:
        hysteresis_factor = 0.9  # Easier to turn on
      elif not new_experimental_mode and self.prev_experimental_mode:
        hysteresis_factor = 1.2  # Harder to turn off
      else:
        hysteresis_factor = 1.0

      # Apply hysteresis to key conditions
      if hysteresis_factor != 1.0:
        self.slow_lead_detected = self.slow_lead_filter.x >= scale_threshold(v_ego) * hysteresis_factor
        self.curve_detected = self.curvature_filter.x >= THRESHOLD * hysteresis_factor and v_ego > CRUISING_SPEED

      self.experimental_mode = self.check_conditions(v_ego, sm, frogpilot_toggles)

      # Asymmetric hold: experimental stays on for at least EXPERIMENTAL_HOLD_TIME seconds,
      # but switching TO experimental from chill is always instant
      if self.prev_experimental_mode and not self.experimental_mode:
        if (time.monotonic() - self.experimental_mode_since) < self.EXPERIMENTAL_HOLD_TIME:
          self.experimental_mode = True
      elif self.experimental_mode and not self.prev_experimental_mode:
        self.experimental_mode_since = time.monotonic()

      self.prev_experimental_mode = self.experimental_mode
      self.frogpilot_planner.params_memory.put("CEStatus", self.status_value if self.experimental_mode else CEStatus["OFF"])
    else:
      self.experimental_mode = sm["carState"].standstill and self.experimental_mode and self.frogpilot_planner.model_stopped
      self.experimental_mode |= self.status_value == CEStatus["USER_OVERRIDDEN"]
      self.experimental_mode &= self.status_value != CEStatus["USER_DISABLED"]

      self.stop_light_detected &= self.status_value not in (CEStatus["USER_DISABLED"], CEStatus["USER_OVERRIDDEN"])
      self.stop_light_filter.x = 0

  def check_conditions(self, v_ego, sm, frogpilot_toggles):
    if self.curve_detected and (not self.frogpilot_planner.frogpilot_following.following_lead or frogpilot_toggles.conditional_curves_lead) and frogpilot_toggles.conditional_curves:
      self.status_value = CEStatus["CURVATURE"]
      return True

    if self.slow_lead_detected and frogpilot_toggles.conditional_lead:
      self.status_value = CEStatus["LEAD"]
      return True

    desired_lane = self.frogpilot_planner.lane_width_left if sm["carState"].leftBlinker else self.frogpilot_planner.lane_width_right
    lane_available = desired_lane >= frogpilot_toggles.lane_detection_width or not frogpilot_toggles.conditional_signal_lane_detection
    if v_ego < frogpilot_toggles.conditional_signal and (sm["carState"].leftBlinker or sm["carState"].rightBlinker) and not lane_available:
      self.status_value = CEStatus["SIGNAL"]
      return True

    below_speed = not self.frogpilot_planner.frogpilot_following.following_lead and 1 <= v_ego < frogpilot_toggles.conditional_limit
    below_speed_with_lead = self.frogpilot_planner.frogpilot_following.following_lead and 1 <= v_ego < frogpilot_toggles.conditional_limit_lead
    if below_speed or below_speed_with_lead:
      self.status_value = CEStatus["SPEED"]
      return True

    if self.frogpilot_planner.frogpilot_vcruise.slc.experimental_mode:
      self.status_value = CEStatus["SPEED_LIMIT"]
      return True

    if self.stop_light_detected and frogpilot_toggles.conditional_model_stop_time != 0:
      self.status_value = CEStatus["STOP_LIGHT"]
      return True

    return False

  def update_conditions(self, v_ego, sm, frogpilot_toggles):
    self.curve_detection(v_ego, frogpilot_toggles)
    self.slow_lead(v_ego, frogpilot_toggles)
    self.stop_sign_and_light(v_ego, sm, frogpilot_toggles.conditional_model_stop_time)

  def curve_detection(self, v_ego, frogpilot_toggles):
    self.curvature_filter.update(self.frogpilot_planner.road_curvature_detected or self.frogpilot_planner.driving_in_curve)
    self.curve_detected = self.curvature_filter.x >= THRESHOLD and v_ego > CRUISING_SPEED

  def slow_lead(self, v_ego, frogpilot_toggles):
    if self.frogpilot_planner.tracking_lead:
      slower_lead = (v_ego - self.frogpilot_planner.lead_one.vLead) > CRUISING_SPEED and frogpilot_toggles.conditional_slower_lead
      stopped_lead = self.frogpilot_planner.lead_one.vLead < 1 and frogpilot_toggles.conditional_stopped_lead
      lead_threshold = scale_threshold(v_ego)

      # Adjust threshold based on lead probability for vision-only accuracy
      lead_prob = getattr(self.frogpilot_planner.lead_one, 'modelProb', 1.0)
      adjusted_threshold = lead_threshold * (1.0 + 0.2 * (1.0 - lead_prob))

      self.slow_lead_filter.update(slower_lead or stopped_lead)
      self.slow_lead_detected = self.slow_lead_filter.x >= adjusted_threshold
    else:
      self.slow_lead_filter.x = 0
      self.slow_lead_detected = False

  def stop_sign_and_light(self, v_ego, sm, model_time):
    if not sm["frogpilotCarState"].trafficModeEnabled:
      speed_mph = v_ego * CV.MS_TO_MPH

      # Smooth interpolation for 35-45 mph transition
      bp = [0, 35, 45]
      low_filter_time = 0.0
      tuned_filter_time_curves = self.FILTER_TIME_CURVES[1]
      tuned_filter_time_leads = self.FILTER_TIME_LEADS[1]
      tuned_filter_time_lights = self.FILTER_TIME_LIGHTS[1]
      low_boost = 1.0
      tuned_boost = self.LIGHT_BOOSTS[1]
      low_cap_factor = 0.0
      tuned_cap_factor = 1.0

      filter_time_curves = np.interp(speed_mph, bp, [low_filter_time, low_filter_time, tuned_filter_time_curves])
      filter_time_leads = np.interp(speed_mph, bp, [low_filter_time, low_filter_time, tuned_filter_time_leads])
      filter_time_lights = np.interp(speed_mph, bp, [low_filter_time, low_filter_time, tuned_filter_time_lights])
      light_boost = np.interp(speed_mph, bp, [low_boost, low_boost, tuned_boost])
      cap_factor = np.interp(speed_mph, bp, [low_cap_factor, low_cap_factor, tuned_cap_factor])

      # Manual Stop Ahead: flat 1.3x boost for light detection and model time
      if sm["frogpilotCarState"].manualStopAhead:
        light_boost = 1.3
        model_time *= 1.3

      # Update filter times dynamically
      self.curvature_filter = FirstOrderFilter(self.curvature_filter.x, filter_time_curves, DT_MDL)
      self.slow_lead_filter = FirstOrderFilter(self.slow_lead_filter.x, filter_time_leads, DT_MDL)
      self.stop_light_filter = FirstOrderFilter(self.stop_light_filter.x, filter_time_lights, DT_MDL)

      # Disable stoplight detection above 75 mph to prevent false positives
      if speed_mph > 75:
        self.stop_light_filter.x = 0
        self.stop_light_detected = False
        return

      # Boost model time for extended detection range
      adjusted_model_time = model_time * light_boost
      if cap_factor > 0:
        adjusted_model_time = min(adjusted_model_time, self.LIGHT_MAX_TIME * cap_factor + model_time * (1 - cap_factor))

      model_stopping = self.frogpilot_planner.model_length < v_ego * adjusted_model_time

      self.stop_light_filter.update(self.frogpilot_planner.model_stopped or model_stopping)
      self.stop_light_detected = bool(self.stop_light_filter.x >= THRESHOLD**2 and not self.frogpilot_planner.tracking_lead)
    else:
      self.stop_light_filter.x = 0
      self.stop_light_detected = False
