#!/usr/bin/env python3
"""
Compare force stop tracked distance vs dashboard sign distance.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Shows side-by-side:
  - Dash: dashboard sign distance (BYTE22, any sign type)
  - Tracked: tracked_model_length (what force stop uses for deceleration)
  - Speed and force stop status

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging

def main():
  sm = messaging.SubMaster(["frogpilotPlan", "frogpilotCarState", "carState"])

  prev_line = ""

  print("Force stop distance comparison")
  print(f"  {'dash':>5s} {'tracked':>8s} {'speed':>6s}  status")
  print("-" * 45)

  while True:
    sm.update(200)

    if sm.updated["frogpilotPlan"]:
      plan = sm["frogpilotPlan"]
      car = sm["frogpilotCarState"]
      cs = sm["carState"]

      dash_dist = car.dashboardStopSign
      tracked = plan.forcingStopLength
      forcing = plan.forcingStop
      speed_mph = cs.vEgo * 2.237

      if forcing or dash_dist > 0:
        status = "FORCING" if forcing else "sign"
        line = (f"{dash_dist:>5d} {tracked:>8.1f} "
                f"{speed_mph:>5.1f}mph  {status}")
        if line != prev_line:
          print(f"  {line}")
          prev_line = line
      elif prev_line:
        print(f"  --- cleared ---")
        prev_line = ""

if __name__ == "__main__":
  main()
