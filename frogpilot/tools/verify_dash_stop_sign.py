#!/usr/bin/env python3
"""
Compare model distance vs dashboard distance for force stop.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Shows side-by-side:
  - Model: model_length (how far the model sees the path extending)
  - Dash: dashboard sign distance (BYTE22, any sign type)
  - Tracked: tracked_model_length (what force stop uses for deceleration)
  - Speed and force stop status

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging

def main():
  sm = messaging.SubMaster(["modelV2", "frogpilotPlan", "frogpilotCarState", "carState"])

  prev_line = ""

  print("Model vs Dashboard distance comparison")
  print(f"  {'model':>6s} {'dash':>5s} {'tracked':>8s} {'speed':>6s}  status")
  print("-" * 55)

  while True:
    sm.update(100)

    if sm.updated["modelV2"]:
      plan = sm["frogpilotPlan"]
      car = sm["frogpilotCarState"]
      cs = sm["carState"]
      model = sm["modelV2"]

      model_len = model.position.x[-1] if len(model.position.x) > 0 else 0
      dash_dist = car.dashboardStopSign
      tracked = plan.forcingStopLength
      forcing = plan.forcingStop
      v_ego = cs.vEgo
      speed_mph = v_ego * 2.237

      if forcing or dash_dist > 0:
        status = "FORCING" if forcing else "sign"
        line = (f"{model_len:>6.1f} {dash_dist:>5d} {tracked:>8.1f} "
                f"{speed_mph:>5.1f}mph  {status}")
        if line != prev_line:
          print(f"  {line}")
          prev_line = line
      elif prev_line:
        print(f"  --- cleared ---")
        prev_line = ""

if __name__ == "__main__":
  main()
