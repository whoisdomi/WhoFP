#!/usr/bin/env python3
"""
Verify stop sign detection: B22 AND B24 both > 0 vs model distance.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Uses a 0.5s hold timer to ignore brief single-frame dropouts
where B22 or B24 momentarily goes to 0.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import time

HOLD_TIME = 0.5  # seconds to hold detection through brief dropouts

def main():
  sm = messaging.SubMaster(["can", "frogpilotPlan", "carState"])

  t0 = time.monotonic()
  active = False
  last_valid_time = 0.0  # last time both B22 and B24 were > 0
  prev_b22 = 0
  prev_b24 = 0

  print("Stop sign verifier: B22+B24 both > 0 = stop sign (0.5s hold)")
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
            now = time.monotonic()
            elapsed = now - t0
            is_stop = b22 > 0 and b24 > 0

            if is_stop:
              last_valid_time = now

            # Active if currently valid OR was valid within hold time
            should_be_active = is_stop or (now - last_valid_time < HOLD_TIME)

            if should_be_active and not active:
              print(f"  {elapsed:6.1f}  >>> STOP SIGN DETECTED (B22={b22} B24={b24})")
              active = True

            if active and (b22 != prev_b22 or b24 != prev_b24):
              model_dist = sm["frogpilotPlan"].forcingStopLength
              speed_mph = sm["carState"].vEgo * 2.237
              note = ""
              if not is_stop and should_be_active:
                note = " (holding)"
              print(f"  {elapsed:6.1f}  {b22:>4d}  {b24:>4d}  {model_dist:>6.1f}m  {speed_mph:4.0f}  {note}")

            if not should_be_active and active:
              print(f"  {elapsed:6.1f}  <<< STOP SIGN GONE")
              active = False

            # Log false positive pattern (B24 alone) for debugging
            if b24 > 0 and b22 == 0 and prev_b24 == 0 and not active:
              print(f"  {elapsed:6.1f}  --- B24-only={b24} (filtered out, no B22)")

            prev_b22 = b22
            prev_b24 = b24

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print("\nDone.")
