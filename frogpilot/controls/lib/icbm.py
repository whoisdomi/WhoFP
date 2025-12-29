#!/usr/bin/env python3
"""
ICBM - Intelligent Cruise Button Management

Manages cruise control set speed via simulated button presses for vehicles
without openpilot longitudinal control. Compares FrogPilot's target speed
against the cluster's set speed and sends button commands to match them.

For Hyundai/Kia/Genesis CAN-FD vehicles (non-ALT_BUTTONS).
"""
from openpilot.common.constants import CV


class ICBMController:
  """Intelligent Cruise Button Management - manages cruise set speed via button presses"""

  # States
  INACTIVE = 0
  PRE_ACTIVE = 1
  INCREASING = 2
  DECREASING = 3
  HOLDING = 4

  # Button outputs
  NONE = 0
  INCREASE = 1
  DECREASE = 2

  # Minimum set speed (display units)
  MIN_SPEED_KPH = 30
  MIN_SPEED_MPH = 20

  # Pre-active delay (frames at 20Hz = 8 frames = 400ms)
  PRE_ACTIVE_FRAMES = 8

  def __init__(self):
    self.state = self.INACTIVE
    self.pre_active_counter = 0
    self.v_target = 0
    self.v_cluster = 0
    self.is_ready = False
    self.is_ready_prev = False

  def update(self, enabled, override, v_cruise_ms, v_cruise_cluster_ms, is_metric):
    """
    Update ICBM state and return button command.

    Args:
        enabled: Whether cruise control is active
        override: Whether driver is overriding (gas/brake)
        v_cruise_ms: Target cruise speed from FrogPilot (m/s)
        v_cruise_cluster_ms: Current cluster set speed (m/s)
        is_metric: Whether to use metric units

    Returns:
        Button command: NONE, INCREASE, or DECREASE
    """
    # Convert to display units
    conv = CV.MS_TO_KPH if is_metric else CV.MS_TO_MPH
    self.v_target = round(v_cruise_ms * conv)
    self.v_cluster = round(v_cruise_cluster_ms * conv)
    v_min = self.MIN_SPEED_KPH if is_metric else self.MIN_SPEED_MPH

    # Check readiness
    self.is_ready = enabled and not override

    button = self._update_state_machine(v_min)

    self.is_ready_prev = self.is_ready
    return button

  def _update_state_machine(self, v_min):
    """State machine logic"""

    # Decrement pre-active counter
    if self.pre_active_counter > 0:
      self.pre_active_counter -= 1

    # INACTIVE -> PRE_ACTIVE on rising edge of is_ready
    if self.state == self.INACTIVE:
      if self.is_ready and not self.is_ready_prev:
        self.state = self.PRE_ACTIVE
        self.pre_active_counter = self.PRE_ACTIVE_FRAMES
      return self.NONE

    # Any state -> INACTIVE if not ready
    if not self.is_ready:
      self.state = self.INACTIVE
      return self.NONE

    # PRE_ACTIVE -> determine next state after delay
    if self.state == self.PRE_ACTIVE:
      if self.pre_active_counter <= 0:
        if self.v_target == self.v_cluster:
          self.state = self.HOLDING
        elif self.v_target > self.v_cluster:
          self.state = self.INCREASING
        elif self.v_target < self.v_cluster and self.v_cluster > v_min:
          self.state = self.DECREASING
        else:
          self.state = self.HOLDING
      return self.NONE

    # HOLDING -> check if target changed
    if self.state == self.HOLDING:
      if self.v_target != self.v_cluster:
        self.state = self.PRE_ACTIVE
        self.pre_active_counter = 0  # No delay for transitions from HOLDING
      return self.NONE

    # INCREASING -> send increase, check if done
    if self.state == self.INCREASING:
      if self.v_target <= self.v_cluster:
        self.state = self.HOLDING
        return self.NONE
      return self.INCREASE

    # DECREASING -> send decrease, check if done
    if self.state == self.DECREASING:
      if self.v_target >= self.v_cluster or self.v_cluster <= v_min:
        self.state = self.HOLDING
        return self.NONE
      return self.DECREASE

    return self.NONE
