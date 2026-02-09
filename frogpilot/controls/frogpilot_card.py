#!/usr/bin/env python3
from opendbc.safety import ALTERNATIVE_EXPERIENCE
from openpilot.common.params import Params
from openpilot.selfdrive.car.cruise import CRUISE_LONG_PRESS, ButtonType
from openpilot.selfdrive.selfdrived.events import ET

from openpilot.frogpilot.common.frogpilot_utilities import is_FrogsGoMoo
from openpilot.frogpilot.common.frogpilot_variables import ERROR_LOGS_PATH, NON_DRIVING_GEARS
from openpilot.frogpilot.controls.lib.conditional_experimental_mode import CEStatus

class FrogPilotCard:
  def __init__(self, CP, FPCP):
    self.CP = CP

    self.params = Params(return_defaults=True)
    self.params_memory = Params(memory=True)

    self.accel_pressed = False
    self.always_on_lateral_allowed = False
    self.decel_pressed = False
    self.distance_button_pressed = False
    self.distancePressed_previously = False
    self.mode_button_pressed = False
    self.modePressed_previously = False
    self.custom_button_pressed = False
    self.customPressed_previously = False
    self.force_coast = False
    self.manual_stop_ahead = False
    self.pause_lateral = False
    self.pause_longitudinal = False
    self.traffic_mode_enabled = False

    self.manual_stop_ahead_timer = 0

    self.gap_counter = 0
    self.mode_counter = 0
    self.custom_counter = 0

    self.always_on_lateral_set = bool(FPCP.alternativeExperience & ALTERNATIVE_EXPERIENCE.ALWAYS_ON_LATERAL)
    self.frogs_go_moo = is_FrogsGoMoo()

    # Distance button press thresholds (at 100Hz)
    # Short press: < 30 cycles (< 0.3s)
    # Long press: 30-89 cycles (0.3-0.9s)
    # Very long press: >= 90 cycles (>= 0.9s)
    self.long_press_threshold = 30  # 0.3 seconds
    self.very_long_press_threshold = 90  # 0.9 seconds

    self.error_log = ERROR_LOGS_PATH / "error.txt"

  def handle_button_event(self, key, sm, frogpilot_toggles):
    if sm["carControl"].longActive and getattr(frogpilot_toggles, f"experimental_mode_via_{key}"):
      self.handle_experimental_mode(sm, frogpilot_toggles)
    elif sm["carControl"].longActive and getattr(frogpilot_toggles, f"force_coast_via_{key}"):
      self.force_coast = not self.force_coast
    elif getattr(frogpilot_toggles, f"pause_lateral_via_{key}"):
      self.pause_lateral = not self.pause_lateral
    elif sm["carControl"].longActive and getattr(frogpilot_toggles, f"pause_longitudinal_via_{key}"):
      self.pause_longitudinal = not self.pause_longitudinal
    elif sm["carControl"].longActive and getattr(frogpilot_toggles, f"traffic_mode_via_{key}"):
      self.traffic_mode_enabled = not self.traffic_mode_enabled
    elif sm["carControl"].longActive and getattr(frogpilot_toggles, f"manual_stop_ahead_via_{key}"):
      self.manual_stop_ahead = not self.manual_stop_ahead
      if self.manual_stop_ahead:
        self.manual_stop_ahead_timer = 0

  def handle_experimental_mode(self, sm, frogpilot_toggles):
    if frogpilot_toggles.conditional_experimental_mode:
      if self.params_memory.get("CEStatus") in (CEStatus["USER_DISABLED"], CEStatus["USER_OVERRIDDEN"]):
        override_value = CEStatus["OFF"]
      elif sm["selfdriveState"].experimentalMode:
        override_value = CEStatus["USER_DISABLED"]
      else:
        override_value = CEStatus["USER_OVERRIDDEN"]

      self.params_memory.put("CEStatus", override_value)
    else:
      self.params.put_bool_nonblocking("ExperimentalMode", not sm["selfdriveState"].experimentalMode)

  def update(self, carState, frogpilotCarState, sm, frogpilot_toggles):
    if self.CP.brand == "hyundai":
      for be in carState.buttonEvents:
        if be.type == ButtonType.lkas and be.pressed and frogpilot_toggles.always_on_lateral_lkas:
          self.always_on_lateral_allowed = not self.always_on_lateral_allowed
        elif be.type == ButtonType.mainCruise and be.pressed:
          if frogpilot_toggles.speed_limit_controller and sm["carControl"].longActive:
            self.params_memory.put_bool("SLCAdoptSpeedLimit", True)
    elif frogpilot_toggles.always_on_lateral_main:
      self.always_on_lateral_allowed = carState.cruiseState.available

    self.always_on_lateral_enabled = self.always_on_lateral_allowed and self.always_on_lateral_set
    self.always_on_lateral_enabled &= carState.gearShifter not in NON_DRIVING_GEARS
    self.always_on_lateral_enabled &= sm["frogpilotPlan"].lateralCheck
    self.always_on_lateral_enabled &= sm["liveCalibration"].calPerc >= 1
    self.always_on_lateral_enabled &= (ET.IMMEDIATE_DISABLE not in sm["selfdriveState"].alertType + sm["frogpilotSelfdriveState"].alertType) or self.frogs_go_moo
    self.always_on_lateral_enabled &= not (carState.brakePressed and carState.vEgo < frogpilot_toggles.always_on_lateral_pause_speed) or carState.standstill
    self.always_on_lateral_enabled &= not self.error_log.is_file() or self.frogs_go_moo

    if sm.updated["frogpilotPlan"] or any(be.type in (ButtonType.accelCruise, ButtonType.resumeCruise) for be in carState.buttonEvents):
      self.accel_pressed = any(be.type in (ButtonType.accelCruise, ButtonType.resumeCruise) for be in carState.buttonEvents)

    if sm.updated["frogpilotPlan"] or any(be.type == ButtonType.decelCruise for be in carState.buttonEvents):
      self.decel_pressed = any(be.type == ButtonType.decelCruise for be in carState.buttonEvents)

    # Track physical distance/gap button state
    for be in carState.buttonEvents:
      if be.type == ButtonType.gapAdjustCruise:
        self.distance_button_pressed = be.pressed

    # Combine physical button with onscreen button (OR logic)
    frogpilotCarState.distancePressed = self.distance_button_pressed or self.params_memory.get_bool("OnroadDistanceButtonPressed")

    if frogpilotCarState.distancePressed:
      self.gap_counter += 1
    elif not self.distancePressed_previously:
      self.gap_counter = 0

    self.distancePressed_previously = frogpilotCarState.distancePressed

    if not frogpilotCarState.distancePressed and 1 < self.gap_counter < self.long_press_threshold:
      self.handle_button_event("distance", sm, frogpilot_toggles)
    elif not frogpilotCarState.distancePressed and self.long_press_threshold <= self.gap_counter < self.very_long_press_threshold:
      self.handle_button_event("distance_long", sm, frogpilot_toggles)
    elif not frogpilotCarState.distancePressed and self.gap_counter >= self.very_long_press_threshold:
      self.handle_button_event("distance_very_long", sm, frogpilot_toggles)

    # Track mode button state from FrogPilotCarState (parsed in carstate.py for CAN FD Hyundai)
    self.mode_button_pressed = frogpilotCarState.modePressed

    if self.mode_button_pressed:
      self.mode_counter += 1
    elif not self.modePressed_previously:
      self.mode_counter = 0

    self.modePressed_previously = self.mode_button_pressed

    if not self.mode_button_pressed and 1 < self.mode_counter < self.long_press_threshold:
      self.handle_button_event("mode", sm, frogpilot_toggles)
    elif not self.mode_button_pressed and self.long_press_threshold <= self.mode_counter < self.very_long_press_threshold:
      self.handle_button_event("mode_long", sm, frogpilot_toggles)
    elif not self.mode_button_pressed and self.mode_counter >= self.very_long_press_threshold:
      self.handle_button_event("mode_very_long", sm, frogpilot_toggles)

    # Track custom button state from FrogPilotCarState (parsed in carstate.py for CAN FD Hyundai)
    self.custom_button_pressed = frogpilotCarState.customPressed

    if self.custom_button_pressed:
      self.custom_counter += 1
    elif not self.customPressed_previously:
      self.custom_counter = 0

    self.customPressed_previously = self.custom_button_pressed

    if not self.custom_button_pressed and 1 < self.custom_counter < self.long_press_threshold:
      self.handle_button_event("custom", sm, frogpilot_toggles)
    elif not self.custom_button_pressed and self.long_press_threshold <= self.custom_counter < self.very_long_press_threshold:
      self.handle_button_event("custom_long", sm, frogpilot_toggles)
    elif not self.custom_button_pressed and self.custom_counter >= self.very_long_press_threshold:
      self.handle_button_event("custom_very_long", sm, frogpilot_toggles)

    if any(be.pressed and be.type == ButtonType.lkas for be in carState.buttonEvents):
      self.handle_button_event("lkas", sm, frogpilot_toggles)

    self.force_coast &= not (carState.brakePressed or carState.gasPressed)

    # Manual Stop Ahead timeout and cancellation logic
    if self.manual_stop_ahead:
      self.manual_stop_ahead_timer += 1
      # Cancel on gas press
      if carState.gasPressed:
        self.manual_stop_ahead = False
      # Cancel on 25 second timeout (2500 cycles at 100Hz)
      elif self.manual_stop_ahead_timer >= 2500:
        self.manual_stop_ahead = False
      # Cancel when car reaches standstill (we've stopped)
      elif carState.standstill:
        self.manual_stop_ahead = False

    frogpilotCarState.accelPressed = self.accel_pressed
    frogpilotCarState.alwaysOnLateralAllowed = self.always_on_lateral_allowed
    frogpilotCarState.alwaysOnLateralEnabled = self.always_on_lateral_enabled
    frogpilotCarState.decelPressed = self.decel_pressed
    frogpilotCarState.distanceLongPressed = self.very_long_press_threshold > self.gap_counter >= self.long_press_threshold
    frogpilotCarState.distanceVeryLongPressed = self.gap_counter >= self.very_long_press_threshold
    frogpilotCarState.modeLongPressed = self.very_long_press_threshold > self.mode_counter >= self.long_press_threshold
    frogpilotCarState.modeVeryLongPressed = self.mode_counter >= self.very_long_press_threshold
    frogpilotCarState.customLongPressed = self.very_long_press_threshold > self.custom_counter >= self.long_press_threshold
    frogpilotCarState.customVeryLongPressed = self.custom_counter >= self.very_long_press_threshold
    frogpilotCarState.forceCoast = self.force_coast
    frogpilotCarState.manualStopAhead = self.manual_stop_ahead
    frogpilotCarState.pauseLateral = self.pause_lateral
    frogpilotCarState.pauseLongitudinal = self.pause_longitudinal
    frogpilotCarState.trafficModeEnabled = self.traffic_mode_enabled

    return frogpilotCarState
