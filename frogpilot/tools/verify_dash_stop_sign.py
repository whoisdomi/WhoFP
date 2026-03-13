#!/usr/bin/env python3
"""
Verify stop sign detection: B22 AND B24 both > 0 vs model distance.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Theory: B24 alone has false positives. Requiring BOTH B22 > 0 AND B24 > 0
should match only actual dashboard stop sign appearances.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import time

def main():
  sm = messaging.SubMaster(["can", "frogpilotPlan", "carState"])

  t0 = time.monotonic()
  active = False
  prev_b22 = 0
  prev_b24 = 0

  print("Stop sign verifier: B22+B24 both > 0 = stop sign")
  print(f"  {'Time':>6s}  {'B22':>4s}  {'B24':>4s}  {'Model':>7s}  {'Speed':>5s}  Note")
  print("-" * 65)

  while True:
    sm.update(100)

    if sm.updated["can"]:
      for msg in sm["can"]:
        if msg.address == 0x362 and msg.src == 2:
          data = bytes(msg.dat)
          if len(data) > 24:
            b22 = data[22]
            b24 = data[24]
            is_stop = b22 > 0 and b24 > 0
            was_stop = prev_b22 > 0 and prev_b24 > 0
            elapsed = time.monotonic() - t0

            if is_stop and not active:
              print(f"  {elapsed:6.1f}  >>> STOP SIGN DETECTED (B22={b22} B24={b24})")
              active = True

            if active and (b22 != prev_b22 or b24 != prev_b24):
              model_dist = sm["frogpilotPlan"].forcingStopLength
              speed_mph = sm["carState"].vEgo * 2.237
              print(f"  {elapsed:6.1f}  {b22:>4d}  {b24:>4d}  {model_dist:>6.1f}m  {speed_mph:4.0f}")

            if not is_stop and active:
              print(f"  {elapsed:6.1f}  <<< STOP SIGN GONE")
              active = False

            # Log false positive pattern (B24 alone) for debugging
            if b24 > 0 and b22 == 0 and prev_b24 == 0:
              print(f"  {elapsed:6.1f}  --- B24-only={b24} (filtered out, no B22)")

            prev_b22 = b22
            prev_b24 = b24

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print("\nDone.")
