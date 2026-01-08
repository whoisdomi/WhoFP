"""
Copyright (c) 2021-, James Vecellio, Haibin Wen, sunnypilot, and a number of other contributors.

This file is part of sunnypilot and is licensed under the MIT License.
See the LICENSE.md file in the root directory for more details.
"""
from math import degrees, sqrt
from numpy import interp
import threading

import cereal.messaging as messaging
from cereal import custom
from openpilot.common.params import Params
from openpilot.common.realtime import Ratekeeper

from openpilot.sunnypilot.navd.constants import NAV_CV
from openpilot.sunnypilot.navd.helpers import Coordinate, parse_banner_instructions
from openpilot.sunnypilot.navd.navigation_helpers.mapbox_integration import MapboxIntegration
from openpilot.sunnypilot.navd.navigation_helpers.nav_instructions import NavigationInstructions


class Navigationd:
  def __init__(self):
    self.params = Params()
    self.mapbox = MapboxIntegration()
    self.nav_instructions = NavigationInstructions()

    self.sm = messaging.SubMaster(['liveLocationKalman', 'gpsLocation'])
    self.pm = messaging.PubMaster(['navigationd'])
    self.rk = Ratekeeper(3) # 3 Hz

    self.route = None
    self.destination: str | None = None
    self.new_destination: str = ''

    self.allow_navigation: bool = False
    self.recompute_allowed: bool = False
    self.allow_recompute: bool = False
    self.reroute_counter: int = 0
    self.cancel_route_counter: int = 0

    self.frame: int = -1
    self.last_position: Coordinate | None = None
    self.last_bearing: float | None = None
    self.last_speed: float = 0.0
    self.valid: bool = False

    self._route_thread: threading.Thread | None = None
    self._route_lock = threading.Lock()
    self._route_computing: bool = False

  def _compute_route_async(self, destination: str, longitude: float, latitude: float, bearing: float | None):
    postvars = {'place_name': destination}
    postvars, valid_addr = self.mapbox.set_destination(postvars, longitude, latitude, bearing)

    with self._route_lock:
      if valid_addr:
        self.destination = destination
        self.nav_instructions.clear_route_cache()
        self.route = self.nav_instructions.get_current_route()
        self.cancel_route_counter = 0
        self.reroute_counter = 0
      self._route_computing = False

  def _update_params(self):
    if self.last_position is not None:
      self.frame += 1
      if self.frame % 15 == 0:
        self.allow_navigation = self.params.get('AllowNavigation', return_default=True)
        self.new_destination = self.params.get('MapboxRoute')
        self.recompute_allowed = self.params.get('MapboxRecompute', return_default=True)

      # Handle clearing the route when MapboxRoute is set to empty/null
      if (self.new_destination == '' or self.new_destination is None) and self.destination is not None and self.destination != '':
        with self._route_lock:
          self.destination = None
          self.nav_instructions.clear_route_cache()
          self.route = None
          self.cancel_route_counter = 0
          self.reroute_counter = 0

      self.allow_recompute: bool = (self.new_destination != self.destination and self.new_destination != '') or (
        self.recompute_allowed and self.reroute_counter > 9 and self.route)

      if self.allow_recompute and not self._route_computing:
        self._route_computing = True
        self._route_thread = threading.Thread(
          target=self._compute_route_async,
          args=(self.new_destination, self.last_position.longitude, self.last_position.latitude, self.last_bearing),
          daemon=True
        )
        self._route_thread.start()

      if self.cancel_route_counter == 30:
        self.cancel_route_counter = 0
        self.params.put_nonblocking("MapboxRoute", "")
        with self._route_lock:
          self.nav_instructions.clear_route_cache()
          self.route = None

      with self._route_lock:
        self.valid = self.route is not None

  def _update_navigation(self) -> tuple[str, dict | None, dict]:
    banner_instructions: str = ''
    nav_data: dict = {}

    with self._route_lock:
      route = self.route

    if self.allow_navigation and route and self.last_position is not None:
      if progress := self.nav_instructions.get_route_progress(self.last_position.latitude, self.last_position.longitude):
        v_ego = self.last_speed
        nav_data['upcoming_turn'] = self.nav_instructions.get_upcoming_turn_from_progress(progress, self.last_position.latitude,
                                                                                          self.last_position.longitude, v_ego)
        speed_limit, _ = progress['current_maxspeed']
        nav_data['current_speed_limit'] = speed_limit
        arrived = self.nav_instructions.arrived_at_destination(progress, v_ego)

        if progress['current_step']:
          if parsed := parse_banner_instructions(progress['current_step']['bannerInstructions'], progress['distance_to_end_of_step']):
            banner_instructions = parsed['maneuverPrimaryText']

        nav_data['distance_from_route'] = progress['distance_from_route']
        speed_breakpoints: list = [0.0, 5.0, 10.0, 20.0, 40.0]
        distance_list: list = [100.0, 125.0, 150.0, 200.0, 250.0]
        large_distance: bool = progress['distance_from_route'] > float(interp(v_ego, speed_breakpoints, distance_list))

        route_bearing_misalign: bool = self.nav_instructions.route_bearing_misalign(route, self.last_bearing, v_ego)

        if large_distance and not arrived:
          self.cancel_route_counter = self.cancel_route_counter + 1 if progress['distance_from_route'] > NAV_CV.QUARTER_MILE else 0
          if self.recompute_allowed:
            self.reroute_counter += 1
        elif arrived:
          self.cancel_route_counter += 1
          self.recompute_allowed = False
        elif route_bearing_misalign:
          self.cancel_route_counter += 1
          if self.recompute_allowed:
            self.reroute_counter += 1
        else:
          self.cancel_route_counter = 0
          self.reroute_counter = 0

        # Don't recompute in last segment to prevent reroute loops
        if progress['current_step_idx'] == len(route['steps']) - 1:
          self.recompute_allowed = False
          self.allow_navigation = False
    else:
      banner_instructions = ''
      progress = None
      nav_data = {}

    return banner_instructions, progress, nav_data

  def _build_navigation_message(self, banner_instructions: str, progress: dict | None, nav_data: dict, valid: bool):
    msg = messaging.new_message('navigationd')
    msg.valid = valid
    msg.navigationd.upcomingTurn = nav_data.get('upcoming_turn', 'none')
    msg.navigationd.currentSpeedLimit = nav_data.get('current_speed_limit', 0)
    msg.navigationd.bannerInstructions = banner_instructions
    msg.navigationd.distanceFromRoute = nav_data.get('distance_from_route', 0.0)
    msg.navigationd.valid = self.valid

    all_maneuvers = (
      [custom.Navigationd.Maneuver.new_message(distance=m['distance'], type=m['type'], modifier=m['modifier'],
                                               instruction=m['instruction'], exit=m.get('exit', 0)) for m in progress['all_maneuvers']]
      if progress
      else []
    )
    msg.navigationd.allManeuvers = all_maneuvers
    return msg

  def run(self):
    while True:
      self.sm.update(0)

      # Try liveLocationKalman first, fall back to gpsLocation
      location = self.sm['liveLocationKalman']
      localizer_valid = location.positionGeodetic.valid if location else False

      if localizer_valid:
        self.last_bearing = degrees(location.calibratedOrientationNED.value[2])
        self.last_position = Coordinate(location.positionGeodetic.value[0], location.positionGeodetic.value[1])
        vel = location.velocityCalibrated.value
        self.last_speed = sqrt(vel[0]**2 + vel[1]**2 + vel[2]**2)
      else:
        gps = self.sm['gpsLocation']
        if gps and gps.hasFix:
          self.last_bearing = gps.bearingDeg
          self.last_position = Coordinate(gps.latitude, gps.longitude)
          self.last_speed = gps.speed
          localizer_valid = True

      self._update_params()
      banner_instructions, progress, nav_data = self._update_navigation()

      # Only publish when we have valid location data to avoid triggering commIssue
      if localizer_valid:
        msg = self._build_navigation_message(banner_instructions, progress, nav_data, valid=localizer_valid)
        self.pm.send('navigationd', msg)

      self.rk.keep_time()


def main():
  nav = Navigationd()
  nav.run()
