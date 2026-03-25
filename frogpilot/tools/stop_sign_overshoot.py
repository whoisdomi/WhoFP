#!/usr/bin/env python3
"""
Read and summarize stop sign overshoot data logged by frogpilot_vcruise.py.

Run after driving through stop signs:
  python3 /data/openpilot/frogpilot/tools/stop_sign_overshoot.py

To clear the log and start fresh:
  python3 /data/openpilot/frogpilot/tools/stop_sign_overshoot.py --clear
"""
import csv
import sys

LOG_PATH = "/data/stop_sign_overshoot.csv"

def main():
  if "--clear" in sys.argv:
    import os
    if os.path.exists(LOG_PATH):
      os.remove(LOG_PATH)
      print("Log cleared.")
    else:
      print("No log to clear.")
    return

  try:
    with open(LOG_PATH) as f:
      reader = csv.DictReader(f)
      rows = list(reader)
  except FileNotFoundError:
    print(f"No log file found at {LOG_PATH}")
    print("Drive through some stop signs first — data is logged automatically.")
    return

  if not rows:
    print("Log file is empty. Drive through some stop signs.")
    return

  # Group by stop_id
  stops = {}
  for row in rows:
    sid = int(row["stop_id"])
    if sid not in stops:
      stops[sid] = []
    stops[sid].append(row)

  print("=" * 70)
  print("STOP SIGN OVERSHOOT REPORT")
  print("=" * 70)

  valid_overshoots = []

  for sid in sorted(stops.keys()):
    events = stops[sid]
    dash_on = [e for e in events if e["event"] == "DASH_ON"]
    standstills = [e for e in events if e["event"] == "STANDSTILL"]
    approaches = [e for e in events if e["event"] == "APPROACH"]

    print(f"\n--- Stop #{sid} ---")

    if dash_on:
      e = dash_on[0]
      print(f"  Dash-on speed:       {float(e['speed_mph']):>5.0f} mph")
      print(f"  Force len at dash-on:{float(e['force_len_ft']):>6.1f} ft")
      print(f"  Model len at dash-on:{float(e['model_len_ft']):>6.1f} ft")

    if approaches:
      # Show a few approach frames to see the countdown
      print(f"  Approach frames:     {len(approaches)}")
      # Show first, middle, last
      samples = [approaches[0]]
      if len(approaches) > 2:
        samples.append(approaches[len(approaches)//2])
      if len(approaches) > 1:
        samples.append(approaches[-1])
      for e in samples:
        print(f"    {float(e['speed_mph']):>5.1f} mph  force:{float(e['force_len_ft']):>6.1f} ft  model:{float(e['model_len_ft']):>6.1f} ft")

    if standstills:
      e = standstills[0]
      overshoot = float(e["force_len_ft"])
      model_at_stop = float(e["model_len_ft"])
      print(f"  ** OVERSHOOT:        {overshoot:>6.1f} ft **")
      print(f"  Model len at stop:   {model_at_stop:>6.1f} ft")
      valid_overshoots.append({
        "id": sid,
        "speed": float(dash_on[0]["speed_mph"]) if dash_on else 0,
        "dash_on_ft": float(dash_on[0]["force_len_ft"]) if dash_on else 0,
        "overshoot_ft": overshoot,
        "model_at_stop_ft": model_at_stop,
      })
    else:
      print(f"  (no standstill recorded)")

  # Final summary
  print(f"\n{'='*70}")
  print("SUMMARY")
  print(f"{'='*70}")
  print(f"Total stop signs logged: {len(stops)}")
  print(f"Stops with standstill:   {len(valid_overshoots)}")

  if not valid_overshoots:
    print("No valid overshoot data.")
    return

  os_vals = [s["overshoot_ft"] for s in valid_overshoots]
  model_vals = [s["model_at_stop_ft"] for s in valid_overshoots]

  print(f"\n{'Stop':>5s}  {'Speed':>6s}  {'Dash-On':>8s}  {'Overshoot':>10s}  {'Model@Stop':>11s}")
  print(f"{'#':>5s}  {'(mph)':>6s}  {'(ft)':>8s}  {'(ft)':>10s}  {'(ft)':>11s}")
  print(f"{'-'*5}  {'-'*6}  {'-'*8}  {'-'*10}  {'-'*11}")
  for s in valid_overshoots:
    print(f"{s['id']:>5d}  {s['speed']:>6.0f}  {s['dash_on_ft']:>8.1f}  {s['overshoot_ft']:>10.1f}  {s['model_at_stop_ft']:>11.1f}")

  print(f"\n  Overshoot (force stop ft remaining at standstill):")
  print(f"    Min:     {min(os_vals):>6.1f} ft")
  print(f"    Max:     {max(os_vals):>6.1f} ft")
  print(f"    Average: {sum(os_vals)/len(os_vals):>6.1f} ft")

  if len(os_vals) > 1:
    mean = sum(os_vals) / len(os_vals)
    variance = sum((x - mean) ** 2 for x in os_vals) / (len(os_vals) - 1)
    print(f"    Std Dev: {variance**0.5:>6.1f} ft")

  print(f"\n  Model length at standstill:")
  print(f"    Min:     {min(model_vals):>6.1f} ft")
  print(f"    Max:     {max(model_vals):>6.1f} ft")
  print(f"    Average: {sum(model_vals)/len(model_vals):>6.1f} ft")

if __name__ == "__main__":
  main()
