#!/usr/bin/env python3
"""
Verify stop sign detection: dash distance vs model distance.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Only prints when a stop sign is detected (0x120 SIGN_TYPE==8)
and dash distance is available (0x362 BYTE22 > 0).

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import time

def main():
  sm = messaging.SubMaster(["can", "frogpilotPlan", "carState"])

  sign_type = 0
  prev_362_dist = -1
  stop_active = False
  t0 = time.monotonic()

  print("Stop sign: dash distance vs model distance")
  print(f"  {'Time':>6s}  {'Dash':>5s}  {'Model':>7s}  {'Diff':>6s}  {'Speed':>5s}")
  print("-" * 50)

  while True:
    sm.update(100)

    if sm.updated["can"]:
      for msg in sm["can"]:
        data = bytes(msg.dat)

        # Track current sign type from 0x120 Bus 1 BYTE[4]
        if msg.address == 0x120 and msg.src == 1 and len(data) > 4:
          new_type = data[4]
          if new_type != sign_type:
            if new_type == 8 and sign_type != 8:
              elapsed = time.monotonic() - t0
              print(f"  {elapsed:6.1f}  >>> STOP SIGN DETECTED")
              stop_active = True
            elif sign_type == 8 and new_type != 8:
              elapsed = time.monotonic() - t0
              print(f"  {elapsed:6.1f}  <<< STOP SIGN GONE")
              stop_active = False
            sign_type = new_type

        # Show dash vs model distance only during stop sign detection
        if msg.address == 0x362 and msg.src == 2 and len(data) > 22:
          dist = data[22]
          if stop_active and dist != prev_362_dist and dist > 0:
            elapsed = time.monotonic() - t0
            model_dist = sm["frogpilotPlan"].forcingStopLength
            speed_mph = sm["carState"].vEgo * 2.237
            diff = dist - model_dist
            print(f"  {elapsed:6.1f}  {dist:>4d}m  {model_dist:>6.1f}m  {diff:>+5.0f}m  {speed_mph:4.0f}")
          prev_362_dist = dist

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print("\nDone.")
