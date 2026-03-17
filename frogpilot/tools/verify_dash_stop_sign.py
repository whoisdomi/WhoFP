#!/usr/bin/env python3
"""
Compare DashStop (0x361 sign type) vs ModelStop distance during stop signs.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Only prints when stop sign icon is on dashboard (dashboardStopSign == 1).
Ends 3 seconds after stop sign leaves dashboard.
Only prints when values change to keep output clean.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import time

def main():
  # Use frogpilotCarState instead of raw CAN to avoid interfering with other processes
  sm = messaging.SubMaster(["frogpilotCarState", "frogpilotPlan", "carState"])

  t0 = time.monotonic()
  active = False
  stop_count = 0
  off_time = 0

  prev_line = ""
  dash_closer = 0

  print("DashStop vs ModelStop comparison")
  print("Only shows data when stop sign icon is on dashboard")
  print("-" * 55)

  while True:
    sm.update(100)

    if not sm.updated["frogpilotCarState"]:
      continue

    now = time.monotonic()
    elapsed = now - t0
    dash_on = sm["frogpilotCarState"].dashboardStopSign > 0
    model_dist = sm["frogpilotPlan"].forcingStopLength
    speed_mph = sm["carState"].vEgo * 2.237
    forcing = sm["frogpilotPlan"].forcingStop

    # Stop sign just appeared
    if dash_on and not active:
      stop_count += 1
      active = True
      dash_closer = 0
      off_time = 0
      print(f"\n===STOP SIGN #{stop_count} ON DASH=== ({elapsed:.0f}s, {speed_mph:.0f}mph)")

    # While active: print model distance and dash status
    if active:
      m = max(model_dist, 0)
      status = "DASH:ON " if dash_on else "DASH:OFF"
      force_str = " FORCING" if forcing else ""
      line = f"  {status}  ModelStop {m:>5.1f}m  {speed_mph:>3.0f}mph{force_str}"

      check = f"{status}_{m:.0f}_{speed_mph:.0f}_{forcing}"
      if check != prev_line:
        print(line)
        prev_line = check

      if dash_on and forcing:
        dash_closer += 1

    # Stop sign just went off
    if not dash_on and active and off_time == 0:
      print(f"===STOP SIGN #{stop_count} OFF DASH===")
      off_time = now

    # End tracking 3 seconds after sign goes off
    if active and off_time > 0 and now - off_time > 3.0:
      if dash_closer > 0:
        print(f"===Dash confirmed stop sign for {dash_closer} frames while force stop was active===\n")
      else:
        print(f"===Force stop was not active during this stop sign===\n")
      active = False
      off_time = 0
      prev_line = ""

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print("\nDone.")
