#!/usr/bin/env python3
"""
Verify stop sign detection: 0x120 SIGN_TYPE + 0x362 distance + model distance.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Monitors:
  1. 0x120 Bus 1 BYTE[4] = SIGN_TYPE (8=STOP)
  2. 0x362 Bus 2 BYTE[22] = sign distance (meters countdown)
  3. frogpilotPlan.forcingStopLength = model's tracked stop distance

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging

SIGN_NAMES = {
  0: "HIDDEN", 1: "PED_CROSSING", 2: "SCHOOL",
  8: "STOP", 9: "YIELD", 16: "DO_NOT_PASS", 19: "DO_NOT_ENTER",
  24: "ROUNDABOUT", 26: "R_CURVE", 27: "L_CURVE",
}

def main():
  sm = messaging.SubMaster(["can", "frogpilotPlan"])

  prev_sign_type = -1
  prev_362_dist = -1
  prev_model_dist = -1

  print("Stop sign signal verifier (0x120 + 0x362 + model)")
  print(f"{'':>6s}  {'Signal':<30s}  {'Dash':>5s}  {'Model':>6s}")
  print("-" * 60)

  while True:
    sm.update(100)

    # Get model's tracked stop distance
    model_dist = 0.0
    if sm.updated["frogpilotPlan"]:
      model_dist = sm["frogpilotPlan"].forcingStopLength
      model_int = int(model_dist)
      if model_int != prev_model_dist and (model_dist > 0 or prev_model_dist > 0):
        print(f"        model dist={model_dist:>6.1f}m")
        prev_model_dist = model_int

    if sm.updated["can"]:
      for msg in sm["can"]:
        data = bytes(msg.dat)

        # 0x120 Bus 1 BYTE[4] = SIGN_TYPE
        if msg.address == 0x120 and msg.src == 1 and len(data) > 4:
          sign_type = data[4]
          if sign_type != prev_sign_type:
            name = SIGN_NAMES.get(sign_type, f"?({sign_type})")
            is_stop = " *** STOP SIGN ***" if sign_type == 8 else ""
            print(f"  0x120 SIGN_TYPE={sign_type:>3d} ({name}){is_stop}")
            prev_sign_type = sign_type

        # 0x362 Bus 2 BYTE[22] = sign distance
        if msg.address == 0x362 and msg.src == 2 and len(data) > 22:
          dist = data[22]
          if dist != prev_362_dist:
            if dist > 0 or prev_362_dist > 0:
              is_stop = "STOP" if prev_sign_type == 8 else "other"
              dash_val = dist if prev_sign_type == 8 else 0
              model_val = sm["frogpilotPlan"].forcingStopLength
              print(f"  0x362 dist={dist:<3d} [{is_stop}]  dash={dash_val:<3d}  model={model_val:>6.1f}m")
            prev_362_dist = dist

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print("\nDone.")
