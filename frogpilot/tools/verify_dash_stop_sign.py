#!/usr/bin/env python3
"""
Verify sign distance (B22) vs model distance during force stop.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Shows B22 (dashboard sign distance) alongside model distance.
Since force stop only activates when the model detects a stop sign,
B22 just needs to provide accurate distance — not identify sign type.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import time

def main():
  sm = messaging.SubMaster(["can", "frogpilotPlan", "carState"])

  t0 = time.monotonic()
  prev_b22 = 0
  sign_active = False

  print("Sign distance verifier: B22 (any sign) vs model distance")
  print(f"  {'Time':>6s}  {'B22':>4s}  {'Model':>7s}  {'Speed':>5s}  {'ForceStop':>9s}")
  print("-" * 55)

  while True:
    sm.update(100)

    if sm.updated["can"]:
      for msg in sm["can"]:
        if msg.address == 0x362 and msg.src == 2:
          data = bytes(msg.dat)
          if len(data) > 24:
            b22 = data[22]
            elapsed = time.monotonic() - t0

            if b22 > 0 and not sign_active:
              print(f"  {elapsed:6.1f}  >>> SIGN DETECTED (B22={b22})")
              sign_active = True

            if sign_active and b22 != prev_b22:
              model_dist = sm["frogpilotPlan"].forcingStopLength
              speed_mph = sm["carState"].vEgo * 2.237
              forcing = sm["frogpilotPlan"].forcingStop
              print(f"  {elapsed:6.1f}  {b22:>4d}  {model_dist:>6.1f}m  {speed_mph:4.0f}  {'YES' if forcing else ''}")

            if b22 == 0 and sign_active:
              print(f"  {elapsed:6.1f}  <<< SIGN GONE")
              sign_active = False

            prev_b22 = b22

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print("\nDone.")
