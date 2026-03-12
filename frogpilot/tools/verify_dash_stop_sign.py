#!/usr/bin/env python3
"""
Verify dashboard stop sign distance from frogpilotCarState.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Shows sustained stop sign detections only (filters out brief flickers).
Requires 3 consecutive non-zero readings to show ON, and 3 consecutive
zero readings to show OFF — matching how force stop would actually use it.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging

CONFIRM_FRAMES = 3  # consecutive frames needed to confirm ON or OFF

def main():
  sm = messaging.SubMaster(["frogpilotCarState"])

  active = False
  on_count = 0
  off_count = 0
  prev_dist = -1

  print("Dashboard stop sign monitor (flicker-filtered)")
  print("-" * 50)

  while True:
    sm.update(100)

    if sm.updated["frogpilotCarState"]:
      dist = sm["frogpilotCarState"].dashboardStopSign

      if dist > 0:
        on_count += 1
        off_count = 0
      else:
        off_count += 1
        on_count = 0

      if not active and on_count >= CONFIRM_FRAMES:
        active = True
        print(f"  STOP SIGN DETECTED  dist={dist}m")
        prev_dist = dist

      elif active and off_count >= CONFIRM_FRAMES:
        active = False
        prev_dist = -1
        print(f"  STOP SIGN CLEARED")

      elif active and dist > 0 and dist != prev_dist:
        print(f"  dist={dist}m")
        prev_dist = dist

if __name__ == "__main__":
  main()
