#!/usr/bin/env python3
import numpy as np

from openpilot.selfdrive.controls.lib.longitudinal_mpc_lib.long_mpc import COMFORT_BRAKE, LEAD_DANGER_FACTOR, STOP_DISTANCE, desired_follow_distance, get_jerk_factor, get_T_FOLLOW

from openpilot.frogpilot.common.frogpilot_variables import CITY_SPEED_LIMIT, MAX_T_FOLLOW

TRAFFIC_MODE_BP = [0., CITY_SPEED_LIMIT]

class FrogPilotFollowing:
  def __init__(self, FrogPilotPlanner):
    self.frogpilot_planner = FrogPilotPlanner

    self.disable_throttle = False
    self.following_lead = False

    self.acceleration_jerk = 0
    self.danger_jerk = 0
    self.desired_follow_distance = 0
    self.speed_jerk = 0
    self.t_follow = 0

  def update(self, long_control_active, v_ego, sm, frogpilot_toggles):
    if long_control_active and sm["frogpilotCarState"].trafficModeEnabled:
      if sm["carState"].aEgo >= 0:
        self.base_acceleration_jerk = np.interp(v_ego, TRAFFIC_MODE_BP, frogpilot_toggles.traffic_mode_jerk_acceleration)
        self.base_speed_jerk = np.interp(v_ego, TRAFFIC_MODE_BP, frogpilot_toggles.traffic_mode_jerk_speed)
      else:
        self.base_acceleration_jerk = np.interp(v_ego, TRAFFIC_MODE_BP, frogpilot_toggles.traffic_mode_jerk_deceleration)
        self.base_speed_jerk = np.interp(v_ego, TRAFFIC_MODE_BP, frogpilot_toggles.traffic_mode_jerk_speed_decrease)

      self.base_danger_jerk = np.interp(v_ego, TRAFFIC_MODE_BP, frogpilot_toggles.traffic_mode_jerk_danger)
      self.t_follow = np.interp(v_ego, TRAFFIC_MODE_BP, frogpilot_toggles.traffic_mode_follow)
    elif long_control_active:
      if sm["carState"].aEgo >= 0:
        self.base_acceleration_jerk, self.base_danger_jerk, self.base_speed_jerk = get_jerk_factor(
          frogpilot_toggles.aggressive_jerk_acceleration, frogpilot_toggles.aggressive_jerk_danger, frogpilot_toggles.aggressive_jerk_speed,
          frogpilot_toggles.standard_jerk_acceleration, frogpilot_toggles.standard_jerk_danger, frogpilot_toggles.standard_jerk_speed,
          frogpilot_toggles.relaxed_jerk_acceleration, frogpilot_toggles.relaxed_jerk_danger, frogpilot_toggles.relaxed_jerk_speed,
          frogpilot_toggles.custom_personalities, sm["selfdriveState"].personality
        )
      else:
        self.base_acceleration_jerk, self.base_danger_jerk, self.base_speed_jerk = get_jerk_factor(
          frogpilot_toggles.aggressive_jerk_deceleration, frogpilot_toggles.aggressive_jerk_danger, frogpilot_toggles.aggressive_jerk_speed_decrease,
          frogpilot_toggles.standard_jerk_deceleration, frogpilot_toggles.standard_jerk_danger, frogpilot_toggles.standard_jerk_speed_decrease,
          frogpilot_toggles.relaxed_jerk_deceleration, frogpilot_toggles.relaxed_jerk_danger, frogpilot_toggles.relaxed_jerk_speed_decrease,
          frogpilot_toggles.custom_personalities, sm["selfdriveState"].personality
        )

      self.t_follow = get_T_FOLLOW(
        frogpilot_toggles.aggressive_follow,
        frogpilot_toggles.standard_follow,
        frogpilot_toggles.relaxed_follow,
        frogpilot_toggles.custom_personalities, sm["selfdriveState"].personality
      )
    else:
      self.base_acceleration_jerk = 0
      self.base_danger_jerk = 0
      self.base_speed_jerk = 0
      self.t_follow = 0

    self.acceleration_jerk = self.base_acceleration_jerk
    self.danger_factor = LEAD_DANGER_FACTOR
    self.danger_jerk = self.base_danger_jerk
    self.speed_jerk = self.base_speed_jerk

    self.following_lead = self.frogpilot_planner.tracking_lead and self.frogpilot_planner.lead_one.dRel < (self.t_follow * 2) * v_ego

    if self.frogpilot_planner.frogpilot_weather.weather_id != 0:
      self.t_follow = min(self.t_follow + self.frogpilot_planner.frogpilot_weather.increase_following_distance, MAX_T_FOLLOW)

    if long_control_active and self.frogpilot_planner.tracking_lead:
      if not sm["frogpilotCarState"].trafficModeEnabled and frogpilot_toggles.human_following:
        self.update_follow_values(self.frogpilot_planner.lead_one.dRel, v_ego, self.frogpilot_planner.lead_one.vLead)
      self.desired_follow_distance = desired_follow_distance(v_ego, self.frogpilot_planner.lead_one.vLead, self.t_follow)
    else:
      self.desired_follow_distance = 0

    # Coast when approaching a slower lead that is still at a comfortable distance.
    # Cuts throttle early so we don't keep accelerating toward them and then brake hard.
    self.disable_throttle = False
    if self.frogpilot_planner.tracking_lead and self.frogpilot_planner.lead_one.status:
      lead_distance = self.frogpilot_planner.lead_one.dRel
      v_lead = self.frogpilot_planner.lead_one.vLead
      closing_speed = max(0.0, v_ego - v_lead)
      desired_gap = float(desired_follow_distance(v_ego, v_lead, self.t_follow))
      ttc = lead_distance / max(closing_speed, 1e-3) if closing_speed > 0.1 else 1e6

      # Coast window: far enough from desired gap to not need braking yet,
      # but close enough that continued acceleration would be wasteful.
      coast_window_open = lead_distance > desired_gap + max(4.0, 0.2 * v_ego)
      coast_window_far = lead_distance < desired_gap + max(18.0, 0.8 * v_ego)
      gentle_closing = closing_speed < max(2.0, 0.12 * v_ego)

      self.disable_throttle = (not self.following_lead and v_ego > 5.0 and coast_window_open and
                               coast_window_far and gentle_closing)
      # Never coast when we are entering a potentially late-braking scenario.
      self.disable_throttle &= ttc > 6.0 and lead_distance > desired_gap + 6.0

  def update_follow_values(self, lead_distance, v_ego, v_lead):
    # Offset by FrogAi for FrogPilot for a more natural approach to a faster lead
    if v_lead > v_ego:
      distance_factor = max(lead_distance - (v_ego * self.t_follow), 1)
      accelerating_offset = np.clip(STOP_DISTANCE - v_ego, 1, distance_factor)

      self.acceleration_jerk /= accelerating_offset
      self.danger_factor -= ((v_lead - v_ego) / 100)
      self.speed_jerk /= accelerating_offset
      self.t_follow /= accelerating_offset

    # Offset by FrogAi for FrogPilot for a more natural approach to a slower lead
    if v_lead < v_ego:
      closing_speed = v_ego - v_lead
      desired_gap = desired_follow_distance(v_ego, v_lead, self.t_follow)

      # Scale danger_factor based on how aggressively we're closing and how
      # close we are relative to the desired gap. This extends the MPC's
      # danger zone outward so it starts braking earlier.
      # Closing speed contribution: up to +0.25 at 8 m/s closing
      closing_danger = float(np.interp(closing_speed, [0.5, 2.0, 5.0, 8.0], [0.0, 0.05, 0.15, 0.25]))
      # Proximity contribution: ramps up as we get closer to desired gap
      gap_ratio = lead_distance / max(desired_gap, 1.0)
      proximity_danger = float(np.interp(gap_ratio, [1.0, 1.5, 2.0, 3.0], [0.20, 0.10, 0.03, 0.0]))
      self.danger_factor += closing_danger + proximity_danger

      # Boost speed_jerk (allows MPC to use more jerk for braking) when closing
      speed_jerk_boost = float(np.interp(closing_speed, [1.0, 3.0, 6.0], [1.0, 1.5, 2.5]))
      if gap_ratio < 2.0:
        self.speed_jerk *= speed_jerk_boost

      # Only reduce t_follow when we're already well within the desired gap and
      # closing slowly — i.e. settling into car-following, not still approaching.
      # The old logic would slash t_follow to near-zero when far from a slower lead,
      # which told the MPC "you don't need much gap" and delayed braking.
      already_following = lead_distance < desired_gap * 1.3 and closing_speed < 1.5

      if already_following:
        distance_factor = max(lead_distance - (v_lead * self.t_follow), 1)
        braking_offset = np.clip(min(closing_speed, v_lead) - COMFORT_BRAKE, 1, distance_factor)
        self.t_follow /= braking_offset
