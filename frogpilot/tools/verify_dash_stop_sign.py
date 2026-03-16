#!/usr/bin/env python3
"""
Verify stop sign detection via CAM_0x361 SIGN_TYPE + CAM_0x362 distance.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

SIGN_TYPE (0x361 B[26]): 15 = stop sign, 32 = no sign, 16 = other sign
Distance  (0x362 B[22]): counts down as car approaches sign

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import time

def main():
  sm = messaging.SubMaster(["can", "frogpilotPlan", "carState"])

  t0 = time.monotonic()
  active = False
  prev_sign_type = -1
  prev_dist = -1
  sign_type = 32
  sign_dist = 0

  print("Stop sign verifier: 0x361 SIGN_TYPE + 0x362 distance")
  print(f"  {'Time':>6s}  {'Type':>5s}  {'Dist':>4s}  {'Model':>7s}  {'Speed':>5s}  {'Force':>5s}")
  print("-" * 55)

  while True:
    sm.update(100)

    if sm.updated["can"]:
      for msg in sm["can"]:
        if msg.address == 0x361 and msg.src == 2:
          data = bytes(msg.dat)
          if len(data) > 26:
            sign_type = data[26]

        if msg.address == 0x362 and msg.src == 2:
          data = bytes(msg.dat)
          if len(data) > 22:
            sign_dist = data[22]

      elapsed = time.monotonic() - t0

      if sign_type == 15 and not active:
        print(f"  {elapsed:6.1f}  >>> STOP SIGN ON DASH (dist={sign_dist})")
        active = True

      if active and (sign_type != prev_sign_type or sign_dist != prev_dist):
        model_dist = sm["frogpilotPlan"].forcingStopLength
        speed_mph = sm["carState"].vEgo * 2.237
        forcing = sm["frogpilotPlan"].forcingStop
        label = {15: "STOP", 16: "OTHER", 32: "NONE"}.get(sign_type, f"?{sign_type}")
        print(f"  {elapsed:6.1f}  {label:>5s}  {sign_dist:>4d}  {model_dist:>6.1f}m  {speed_mph:4.0f}  {'YES' if forcing else ''}")

      if sign_type != 15 and active:
        print(f"  {elapsed:6.1f}  <<< STOP SIGN OFF")
        active = False

      prev_sign_type = sign_type
      prev_dist = sign_dist

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print("\nDone.")
