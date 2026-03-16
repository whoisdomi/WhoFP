#!/usr/bin/env python3
"""
Compare DashStop vs ModelStop distances during stop sign detection.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Only prints when 0x361 SIGN_TYPE == 15 (stop sign on dashboard).
After each stop sign, reports which source Force Stop would have used
(the closer/lower distance = earlier commitment to stopping).

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import time

def main():
  sm = messaging.SubMaster(["can", "frogpilotPlan", "carState"])

  t0 = time.monotonic()
  active = False
  sign_type = 32
  sign_dist = 0
  prev_sign_type = -1
  stop_count = 0

  # Track min distances during each stop sign event
  min_dash = None
  min_model = None
  dash_led = 0  # frames where dash was closer
  model_led = 0  # frames where model was closer

  print("DashStop vs ModelStop comparison")
  print("Only shows data when stop sign icon is on dashboard")
  print("-" * 60)

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
      model_dist = sm["frogpilotPlan"].forcingStopLength
      speed_mph = sm["carState"].vEgo * 2.237

      # Stop sign just appeared
      if sign_type == 15 and not active:
        stop_count += 1
        active = True
        min_dash = sign_dist
        min_model = model_dist
        dash_led = 0
        model_led = 0
        print(f"\n===STOP SIGN #{stop_count} ON DASH=== ({elapsed:.0f}s, {speed_mph:.0f}mph)")

      # While active: print every change and a bit after
      if active:
        d = sign_dist if sign_type == 15 else 0
        m = model_dist

        # Track which is closer (lower = more aggressive stop)
        if d > 0 and m > 0:
          if d < m:
            dash_led += 1
          elif m < d:
            model_led += 1
          min_dash = min(min_dash, d) if min_dash is not None else d
          min_model = min(min_model, m) if min_model is not None else m

        marker = ""
        if d > 0 and m > 0:
          marker = " <-- Dash closer" if d < m else (" <-- Model closer" if m < d else "")

        print(f"  DashStop {d:>3d}m  ModelStop {m:>5.0f}m  {speed_mph:>3.0f}mph{marker}")

        # End condition: sign off dash AND (both zero OR model also gone)
        if sign_type != 15:
          if prev_sign_type == 15:
            print(f"===STOP SIGN #{stop_count} OFF DASH===")

          if d == 0 and m <= 0:
            # Determine winner
            if dash_led > model_led:
              winner = "DashStop"
              reason = f"closer {dash_led}/{dash_led + model_led} frames"
            elif model_led > dash_led:
              winner = "ModelStop"
              reason = f"closer {model_led}/{dash_led + model_led} frames"
            else:
              winner = "Tie"
              reason = "both equal"

            print(f"===FORCE STOP would use: {winner} ({reason})===\n")
            active = False

      prev_sign_type = sign_type

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    if active:
      print("\n(interrupted during active stop sign)")
    print("\nDone.")
