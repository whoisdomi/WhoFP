#!/usr/bin/env python3
from cereal import car
from opendbc.safety import ALTERNATIVE_EXPERIENCE
from openpilot.common.params import Params
from openpilot.selfdrive.car.cruise import CRUISE_LONG_PRESS
from openpilot.selfdrive.selfdrived.events import ET

from openpilot.frogpilot.common.frogpilot_variables import NON_DRIVING_GEARS
from openpilot.frogpilot.controls.lib.conditional_experimental_mode import CEStatus

ButtonType = car.CarState.ButtonEvent.Type

class FrogPilotCard:
  def __init__(self, CP, FPCP):
    self.CP = CP

    self.params = Params()
    self.params_memory = Params(memory=True)

    self.accel_pressed = False
    self.always_on_lateral_allowed = False
    self.decel_pressed = False
    self.force_coast = False
    self.pause_lateral = False
    self.pause_longitudinal = False
    self.prev_distance_button = False
    self.traffic_mode_enabled = False

    self.gap_counter = 0

    self.always_on_lateral_set = bool(FPCP.alternativeExperience & ALTERNATIVE_EXPERIENCE.ALWAYS_ON_LATERAL)

    self.long_press_threshold = CRUISE_LONG_PRESS * (1.5 if self.CP.brand == "gm" else 1)
    self.very_long_press_threshold = CRUISE_LONG_PRESS * 5

  def handle_experimental_mode(self, sm, frogpilot_toggles):
    if frogpilot_toggles.conditional_experimental_mode:
      status_value = self.params_memory.get("CEStatus")

      if status_value in (CEStatus["USER_DISABLED"], CEStatus["USER_OVERRIDDEN"]):
        override_value = CEStatus["OFF"]
      elif sm["selfdriveState"].experimentalMode:
        override_value = CEStatus["USER_OVERRIDDEN"]
      else:
        override_value = CEStatus["USER_DISABLED"]

      self.params_memory.put("CEStatus", override_value)
    else:
      self.params.put_bool_nonblocking("ExperimentalMode", not sm["selfdriveState"].experimentalMode)

  def update_distance_button(self, sm, frogpilot_toggles):
    if sm["carControl"].longActive and frogpilot_toggles.experimental_mode_via_distance:
      self.handle_experimental_mode(sm, frogpilot_toggles)
    elif frogpilot_toggles.force_coast_via_distance:
      self.force_coast = not self.force_coast
    elif frogpilot_toggles.pause_lateral_via_distance:
      self.pause_lateral = not self.pause_lateral
    elif frogpilot_toggles.pause_longitudinal_via_distance:
      self.pause_longitudinal = not self.pause_longitudinal
    elif sm["carControl"].longActive and frogpilot_toggles.traffic_mode_via_distance:
      self.traffic_mode_enabled = not self.traffic_mode_enabled

  def update_distance_button_long(self, sm, frogpilot_toggles):
    if sm["carControl"].longActive and frogpilot_toggles.experimental_mode_via_distance_long:
      self.handle_experimental_mode(sm, frogpilot_toggles)
    elif frogpilot_toggles.force_coast_via_distance_long:
      self.force_coast = not self.force_coast
    elif frogpilot_toggles.pause_lateral_via_distance_long:
      self.pause_lateral = not self.pause_lateral
    elif frogpilot_toggles.pause_longitudinal_via_distance_long:
      self.pause_longitudinal = not self.pause_longitudinal
    elif sm["carControl"].longActive and frogpilot_toggles.traffic_mode_via_distance_long:
      self.traffic_mode_enabled = not self.traffic_mode_enabled

  def update_distance_button_very_long(self, sm, frogpilot_toggles):
    self.update_distance_button_long(sm, frogpilot_toggles)

    if sm["carControl"].longActive and frogpilot_toggles.experimental_mode_via_distance_very_long:
      self.handle_experimental_mode(sm, frogpilot_toggles)
    elif frogpilot_toggles.force_coast_via_distance_very_long:
      self.force_coast = not self.force_coast
    elif frogpilot_toggles.pause_lateral_via_distance_very_long:
      self.pause_lateral = not self.pause_lateral
    elif frogpilot_toggles.pause_longitudinal_via_distance_very_long:
      self.pause_longitudinal = not self.pause_longitudinal
    elif sm["carControl"].longActive and frogpilot_toggles.traffic_mode_via_distance_very_long:
      self.traffic_mode_enabled = not self.traffic_mode_enabled

  def update_lkas_button(self, sm, frogpilot_toggles):
    if sm["carControl"].longActive and frogpilot_toggles.experimental_mode_via_lkas:
      self.handle_experimental_mode(sm, frogpilot_toggles)
    elif frogpilot_toggles.force_coast_via_lkas:
      self.force_coast = not self.force_coast
    elif frogpilot_toggles.pause_lateral_via_lkas:
      self.pause_lateral = not self.pause_lateral
    elif frogpilot_toggles.pause_longitudinal_via_lkas:
      self.pause_longitudinal = not self.pause_longitudinal
    elif sm["carControl"].longActive and frogpilot_toggles.traffic_mode_via_lkas:
      self.traffic_mode_enabled = not self.traffic_mode_enabled

  def update(self, carState, frogpilotCarState, sm, frogpilot_toggles):
    if self.CP.brand == "hyundai":
      for be in carState.buttonEvents:
        if be.type == ButtonType.lkas and be.pressed and frogpilot_toggles.always_on_lateral_lkas:
          self.always_on_lateral_allowed = not self.always_on_lateral_allowed
        elif be.type == ButtonType.mainCruise and be.pressed and frogpilot_toggles.always_on_lateral_main:
          self.always_on_lateral_allowed = not self.always_on_lateral_allowed
    elif frogpilot_toggles.always_on_lateral_main:
      self.always_on_lateral_allowed = carState.cruiseState.available
    else:
      self.always_on_lateral_allowed = carState.cruiseState.enabled

    self.always_on_lateral_enabled = self.always_on_lateral_allowed and self.always_on_lateral_set
    self.always_on_lateral_enabled &= carState.gearShifter not in NON_DRIVING_GEARS
    self.always_on_lateral_enabled &= sm["frogpilotPlan"].lateralCheck
    self.always_on_lateral_enabled &= sm["liveCalibration"].calPerc >= 1
    self.always_on_lateral_enabled &= sm["selfdriveState"].alertType != ET.IMMEDIATE_DISABLE or frogpilot_toggles.frogs_go_moo
    self.always_on_lateral_enabled &= not (carState.brakePressed and carState.vEgo < frogpilot_toggles.always_on_lateral_pause_speed or carState.standstill)

    if sm.updated["frogpilotPlan"] or any(be.type in (ButtonType.accelCruise, ButtonType.resumeCruise) for be in carState.buttonEvents):
      self.accel_pressed = any(be.type in (ButtonType.accelCruise, ButtonType.resumeCruise) for be in carState.buttonEvents)

    if sm.updated["frogpilotPlan"] or any(be.type == ButtonType.decelCruise for be in carState.buttonEvents):
      self.decel_pressed = any(be.type == ButtonType.decelCruise for be in carState.buttonEvents)

    self.force_coast &= not (carState.brakePressed or carState.gasPressed)

    frogpilotCarState.distancePressed |= self.params_memory.get_bool("OnroadDistanceButtonPressed")

    if frogpilotCarState.distancePressed:
      self.gap_counter += 1
    elif not self.prev_distance_button:
      self.gap_counter = 0

    if not frogpilotCarState.distancePressed and 1 < self.gap_counter < self.long_press_threshold:
      self.update_distance_button(sm, frogpilot_toggles)
    elif self.gap_counter == self.long_press_threshold:
      self.update_distance_button_long(sm, frogpilot_toggles)
    elif self.gap_counter == self.very_long_press_threshold:
      self.update_distance_button_very_long(sm, frogpilot_toggles)

    lkas_button = any(be.pressed and be.type == ButtonType.lkas for be in carState.buttonEvents)

    if lkas_button:
      self.update_lkas_button(sm, frogpilot_toggles)

    self.prev_distance_button = frogpilotCarState.distancePressed

    frogpilotCarState.accelPressed = self.accel_pressed
    frogpilotCarState.alwaysOnLateralEnabled = self.always_on_lateral_enabled
    frogpilotCarState.decelPressed = self.decel_pressed
    frogpilotCarState.distanceLongPressed = self.very_long_press_threshold > self.gap_counter >= self.long_press_threshold
    frogpilotCarState.distanceVeryLongPressed = self.gap_counter >= self.very_long_press_threshold
    frogpilotCarState.forceCoast = self.force_coast
    frogpilotCarState.pauseLateral = self.pause_lateral
    frogpilotCarState.pauseLongitudinal = self.pause_longitudinal
    frogpilotCarState.trafficModeEnabled = self.traffic_mode_enabled

    return frogpilotCarState
