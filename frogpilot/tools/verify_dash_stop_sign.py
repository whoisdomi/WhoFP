#!/usr/bin/env python3
"""
Monitor dashboardStopSign ON/OFF transitions, raw SIGN_TYPE values, and signal sources.

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
  prev_adas = False
  prev_cam = False

  print("Dashboard stop sign monitor — sources: ADAS=0x380[B10b3]  CAM=0x361[B26==15]  (B30 diagnostic only)")
  print("-" * 70)

  while True:
    sm.update(100)

    if not sm.updated["frogpilotCarState"]:
      continue

    fps = sm["frogpilotCarState"]
    dash_on   = fps.dashboardStopSign > 0
    adas      = fps.adasStopSign
    cam       = fps.camStopSign
    sign_type = fps.dashboardSignType
    speed_mph = sm["carState"].vEgo * 2.237
    elapsed   = time.monotonic() - t0

    # Log every unique SIGN_TYPE change
    if sign_type != prev_sign_type:
      print(f"[{elapsed:>6.1f}s] SIGN_TYPE  {prev_sign_type:>3d} → {sign_type:<3d}  {speed_mph:.0f} mph")
      prev_sign_type = sign_type

    # Log source-level changes even without a full ON/OFF edge
    if adas != prev_adas or cam != prev_cam:
      sources = []
      if adas: sources.append("ADAS")
      if cam:  sources.append("CAM")
      state = ", ".join(sources) if sources else "none"
      print(f"[{elapsed:>6.1f}s] sources    {state}  (adas={int(adas)} cam={int(cam)})  {speed_mph:.0f} mph")
      prev_adas = adas
      prev_cam  = cam

    if dash_on and not prev_dash:
      stop_count += 1
      sources = []
      if adas: sources.append("ADAS")
      if cam:  sources.append("CAM")
      print(f"[{elapsed:>6.1f}s] STOP #{stop_count:>2d}  ON   {speed_mph:.0f} mph  type={sign_type}  [{', '.join(sources)}]")

    if not dash_on and prev_dash:
      print(f"[{elapsed:>6.1f}s] STOP #{stop_count:>2d}  OFF  {speed_mph:.0f} mph")

    prev_dash = dash_on

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print(f"\nDone.")
