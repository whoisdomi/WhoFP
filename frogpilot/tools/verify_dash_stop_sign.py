#!/usr/bin/env python3
"""
Verify stop sign detection: BYTE24 distance vs model distance.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Only prints when BYTE24 > 0 (stop sign detected).
Shows dash distance vs model tracked distance side by side.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import time

def main():
  sm = messaging.SubMaster(["can", "frogpilotPlan", "carState"])

  prev_b24 = -1
  t0 = time.monotonic()
  active = False

  print("Stop sign verifier: BYTE24 (stop-only) vs model distance")
  print(f"  {'Time':>6s}  {'B22':>4s}  {'B24':>4s}  {'Model':>7s}  {'Speed':>5s}")
  print("-" * 45)

  while True:
    sm.update(100)

    if sm.updated["can"]:
      for msg in sm["can"]:
        if msg.address == 0x362 and msg.src == 2:
          data = bytes(msg.dat)
          if len(data) > 24:
            b22 = data[22]
            b24 = data[24]

            if b24 > 0 and not active:
              elapsed = time.monotonic() - t0
              print(f"  {elapsed:6.1f}  >>> STOP SIGN DETECTED")
              active = True

            if b24 != prev_b24 and active:
              elapsed = time.monotonic() - t0
              model_dist = sm["frogpilotPlan"].forcingStopLength
              speed_mph = sm["carState"].vEgo * 2.237
              print(f"  {elapsed:6.1f}  {b22:>4d}  {b24:>4d}  {model_dist:>6.1f}m  {speed_mph:4.0f}")

            if b24 == 0 and active:
              elapsed = time.monotonic() - t0
              print(f"  {elapsed:6.1f}  <<< STOP SIGN GONE")
              active = False

            prev_b24 = b24

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print("\nDone.")
