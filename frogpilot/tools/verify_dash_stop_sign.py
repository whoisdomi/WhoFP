#!/usr/bin/env python3
"""
Monitor dashboardStopSign ON/OFF transitions and raw SIGN_TYPE values.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import time

def main():
  sm = messaging.SubMaster(["frogpilotCarState", "carState"])

  t0 = time.monotonic()
  stop_count = 0
  prev_dash = False
  prev_sign_type = -1

  print("Dashboard stop sign monitor — ON/OFF + raw SIGN_TYPE")
  print("-" * 50)

  while True:
    sm.update(100)

    if not sm.updated["frogpilotCarState"]:
      continue

    dash_on = sm["frogpilotCarState"].dashboardStopSign > 0
    sign_type = sm["frogpilotCarState"].dashboardSignType
    speed_mph = sm["carState"].vEgo * 2.237
    elapsed = time.monotonic() - t0

    # Log every unique SIGN_TYPE change so we can spot unknown values
    if sign_type != prev_sign_type:
      print(f"[{elapsed:>6.1f}s] SIGN_TYPE changed: {prev_sign_type} → {sign_type}  {speed_mph:.0f} mph")
      prev_sign_type = sign_type

    if dash_on and not prev_dash:
      stop_count += 1
      print(f"[{elapsed:>6.1f}s] STOP #{stop_count:>2d}  ON   {speed_mph:.0f} mph  (type={sign_type})")

    if not dash_on and prev_dash:
      print(f"[{elapsed:>6.1f}s] STOP #{stop_count:>2d}  OFF  {speed_mph:.0f} mph  (type={sign_type})")

    prev_dash = dash_on

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print(f"\nDone.")
