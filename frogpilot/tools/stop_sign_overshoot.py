#!/usr/bin/env python3
"""
Read and summarize stop sign overshoot data from continuous frame log.

Run after driving through stop signs:
  python3 /data/openpilot/frogpilot/tools/stop_sign_overshoot.py

To clear the log and start fresh:
  python3 /data/openpilot/frogpilot/tools/stop_sign_overshoot.py --clear
"""
import csv
import sys

LOG_PATH = "/data/stop_sign_overshoot.csv"
GAP_THRESHOLD = 5.0  # seconds of silence = new stop event (logging stops at standstill or drive-away)


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

  # Split into stop events based on time gaps
  events = []
  current = []
  for row in rows:
    t = float(row["time"])
    if current and t - float(current[-1]["time"]) > GAP_THRESHOLD:
      events.append(current)
      current = []
    current.append(row)
  if current:
    events.append(current)

  print("=" * 70)
  print("STOP SIGN OVERSHOOT REPORT")
  print("=" * 70)

  valid = []

  for i, frames in enumerate(events):
    sid = i + 1
    print(f"\n--- Stop #{sid} ({len(frames)} frames) ---")

    # Find key moments
    has_dash = any(f["dash"] == "1" for f in frames)
    has_forcing = any(f["forcing"] == "1" for f in frames)
    standstill_frames = [f for f in frames if f["standstill"] == "1"]

    if not has_dash:
      print("  (no dashboard stop sign — likely a red light, skipping)")
      continue

    # First frame with dash on
    dash_frames = [f for f in frames if f["dash"] == "1"]
    first_dash = dash_frames[0]
    print(f"  Dash-on speed:       {float(first_dash['speed_mph']):>5.0f} mph")
    print(f"  Tracked at dash-on:  {float(first_dash['tracked_ft']):>6.1f} ft")

    # Max speed during event (approach speed)
    max_speed = max(float(f["speed_mph"]) for f in frames)
    print(f"  Max approach speed:  {max_speed:>5.0f} mph")

    # Show approach profile (a few samples)
    moving = [f for f in frames if f["standstill"] == "0"]
    if len(moving) > 2:
      samples = [moving[0], moving[len(moving)//2], moving[-1]]
      print(f"  Approach samples:")
      for f in samples:
        brake = " BRAKE" if f["brake"] == "1" else ""
        force = " FORCING" if f["forcing"] == "1" else ""
        dash = " DASH" if f["dash"] == "1" else ""
        print(f"    {float(f['speed_mph']):>5.1f} mph  tracked:{float(f['tracked_ft']):>6.1f} ft  model:{float(f['model_ft']):>6.1f} ft{force}{dash}{brake}")

    # Find overshoot: last moving frame's tracked_ft before standstill
    if standstill_frames:
      # Get the last moving frame right before first standstill
      first_standstill_time = float(standstill_frames[0]["time"])
      pre_stop_frames = [f for f in frames if f["standstill"] == "0" and float(f["time"]) < first_standstill_time]

      if pre_stop_frames:
        last_moving = pre_stop_frames[-1]
        overshoot = float(last_moving["tracked_ft"])
        model_at_stop = float(last_moving["model_ft"])
        speed_at_stop = float(last_moving["speed_mph"])
        brake = last_moving["brake"] == "1"
        forcing_at_stop = last_moving["forcing"] == "1"
      else:
        # Standstill from the start — use first standstill frame
        sf = standstill_frames[0]
        overshoot = float(sf["tracked_ft"])
        model_at_stop = float(sf["model_ft"])
        speed_at_stop = 0
        brake = sf["brake"] == "1"
        forcing_at_stop = sf["forcing"] == "1"

      stopped_by = "DRIVER (brake)" if brake else "FORCE STOP (auto)"
      print(f"  Stopped by:          {stopped_by}")
      print(f"  Force stop active:   {'Yes' if forcing_at_stop else 'No (dropped before stop)'}")
      print(f"  ** OVERSHOOT:        {overshoot:>6.1f} ft **")
      print(f"  Model len at stop:   {model_at_stop:>6.1f} ft")
      print(f"  Speed before stop:   {speed_at_stop:>5.1f} mph")

      valid.append({
        "id": sid,
        "approach_speed": max_speed,
        "overshoot_ft": overshoot,
        "model_at_stop_ft": model_at_stop,
        "driver_stopped": brake,
        "force_active": forcing_at_stop,
      })
    else:
      print(f"  (no standstill — rolling stop or drove through)")

  # Summary
  print(f"\n{'='*70}")
  print("SUMMARY")
  print(f"{'='*70}")
  print(f"Total events: {len(events)}")
  print(f"Events with standstill: {len(valid)}")

  if not valid:
    print("No overshoot data.")
    return

  os_vals = [s["overshoot_ft"] for s in valid]
  driver = [s for s in valid if s["driver_stopped"]]
  auto = [s for s in valid if not s["driver_stopped"]]

  print(f"\n{'Stop':>5s}  {'Speed':>6s}  {'Overshoot':>10s}  {'Model@Stop':>11s}  {'Force':>6s}  {'Stopped By'}")
  print(f"{'-'*5}  {'-'*6}  {'-'*10}  {'-'*11}  {'-'*6}  {'-'*14}")
  for s in valid:
    who = "DRIVER" if s["driver_stopped"] else "AUTO"
    force = "Yes" if s["force_active"] else "No"
    print(f"{s['id']:>5d}  {s['approach_speed']:>6.0f}  {s['overshoot_ft']:>10.1f}  {s['model_at_stop_ft']:>11.1f}  {force:>6s}  {who}")

  print(f"\n  All stops:")
  print(f"    Count:   {len(os_vals)}")
  print(f"    Min:     {min(os_vals):>6.1f} ft")
  print(f"    Max:     {max(os_vals):>6.1f} ft")
  print(f"    Average: {sum(os_vals)/len(os_vals):>6.1f} ft")
  if len(os_vals) > 1:
    mean = sum(os_vals) / len(os_vals)
    std = (sum((x - mean) ** 2 for x in os_vals) / (len(os_vals) - 1)) ** 0.5
    print(f"    Std Dev: {std:>6.1f} ft")

  if driver:
    d_vals = [s["overshoot_ft"] for s in driver]
    print(f"\n  Driver stops only:")
    print(f"    Count:   {len(d_vals)}")
    print(f"    Average: {sum(d_vals)/len(d_vals):>6.1f} ft")

  if auto:
    a_vals = [s["overshoot_ft"] for s in auto]
    print(f"\n  Auto (force stop) stops only:")
    print(f"    Count:   {len(a_vals)}")
    print(f"    Average: {sum(a_vals)/len(a_vals):>6.1f} ft")


if __name__ == "__main__":
  main()
