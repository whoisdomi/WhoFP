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

    had_force_stop = has_forcing
    mode = "FORCE STOP" if had_force_stop else "MANUAL (lat only)"

    # First frame with dash on
    dash_frames = [f for f in frames if f["dash"] == "1"]
    first_dash = dash_frames[0]
    print(f"  Mode:                {mode}")
    print(f"  Dash-on speed:       {float(first_dash['speed_mph']):>5.0f} mph")

    # Max speed during event (approach speed)
    max_speed = max(float(f["speed_mph"]) for f in frames)
    print(f"  Max approach speed:  {max_speed:>5.0f} mph")

    if had_force_stop:
      print(f"  Tracked at dash-on:  {float(first_dash['tracked_ft']):>6.1f} ft")

    # Show approach profile (a few samples)
    has_diag = "confirmed" in frames[0]  # new diagnostic fields present
    moving = [f for f in frames if f["standstill"] == "0"]
    if len(moving) > 2:
      samples = [moving[0], moving[len(moving)//4], moving[len(moving)//2], moving[3*len(moving)//4], moving[-1]]
      # deduplicate in case of very short events
      seen = set()
      unique_samples = []
      for s in samples:
        key = id(s)
        if key not in seen:
          seen.add(key)
          unique_samples.append(s)
      samples = unique_samples
      print(f"  Approach profile:")
      for f in samples:
        flags = ""
        if f["forcing"] == "1": flags += " FORCING"
        if f["dash"] == "1": flags += " DASH"
        if f["brake"] == "1": flags += " BRAKE"
        if has_diag:
          if f.get("confirmed") == "1": flags += " CONFIRMED"
          if f.get("curve") == "1": flags += " CURVE"
          if f.get("cem") == "1": flags += " CEM"
          if f.get("exp") == "1": flags += " EXP"
          else: flags += " CHILL"
          green_t = float(f.get("green_t", 0))
          force_t = float(f.get("force_t", 0))
          print(f"    {float(f['speed_mph']):>5.1f} mph  trk:{float(f['tracked_ft']):>6.1f} ft  mdl:{float(f['model_ft']):>6.1f} ft  grn:{green_t:.1f}s  frc:{force_t:.1f}s{flags}")
        else:
          print(f"    {float(f['speed_mph']):>5.1f} mph  model:{float(f['model_ft']):>6.1f} ft{flags}")

    # Find overshoot: last moving frame before standstill
    if standstill_frames:
      first_standstill_time = float(standstill_frames[0]["time"])
      pre_stop_frames = [f for f in frames if f["standstill"] == "0" and float(f["time"]) < first_standstill_time]

      if pre_stop_frames:
        last_moving = pre_stop_frames[-1]
        tracked_at_stop = float(last_moving["tracked_ft"])
        model_at_stop = float(last_moving["model_ft"])
        speed_at_stop = float(last_moving["speed_mph"])
        brake = last_moving["brake"] == "1"
        forcing_at_stop = last_moving["forcing"] == "1"
      else:
        sf = standstill_frames[0]
        tracked_at_stop = float(sf["tracked_ft"])
        model_at_stop = float(sf["model_ft"])
        speed_at_stop = 0
        brake = sf["brake"] == "1"
        forcing_at_stop = sf["forcing"] == "1"

      # Calculate distance traveled from dash-on to standstill
      # (integrate speed over time using trapezoidal approximation)
      dash_on_time = float(first_dash["time"])
      dist_traveled = 0
      prev_t = None
      prev_speed = None
      for f in frames:
        t = float(f["time"])
        speed_ms = float(f["speed_mph"]) / 2.237  # back to m/s
        if t >= dash_on_time and t <= first_standstill_time:
          if prev_t is not None:
            dt = t - prev_t
            avg_speed = (speed_ms + prev_speed) / 2
            dist_traveled += avg_speed * dt
          prev_t = t
          prev_speed = speed_ms
      dist_traveled_ft = dist_traveled * 3.28084

      stopped_by = "DRIVER (brake)" if brake else "FORCE STOP (auto)"
      print(f"  Stopped by:          {stopped_by}")
      if had_force_stop:
        print(f"  Force stop active:   {'Yes' if forcing_at_stop else 'No (dropped before stop)'}")
        # Diagnose WHY force stop dropped (if it did)
        if not forcing_at_stop and has_diag:
          # Find last frame where forcing was active
          forcing_frames = [f for f in frames if f["forcing"] == "1"]
          if forcing_frames:
            last_forcing = forcing_frames[-1]
            drop_speed = float(last_forcing["speed_mph"])
            drop_tracked = float(last_forcing["tracked_ft"])
            drop_green = float(last_forcing.get("green_t", 0))
            drop_curve = last_forcing.get("curve") == "1"
            drop_confirmed = last_forcing.get("confirmed") == "1"
            drop_cem = last_forcing.get("cem") == "1"
            print(f"  Force drop at:       {drop_speed:>5.1f} mph, tracked:{drop_tracked:.1f} ft")
            drop_exp = last_forcing.get("exp") == "1"
            print(f"    confirmed={drop_confirmed} green_t={drop_green:.2f}s curve={drop_curve} cem={drop_cem} exp={drop_exp}")
            # Show frame right after forcing dropped
            last_forcing_time = float(last_forcing["time"])
            after_drop = [f for f in frames if f["forcing"] == "0" and float(f["time"]) > last_forcing_time]
            if after_drop:
              af = after_drop[0]
              print(f"    After drop: confirmed={af.get('confirmed','?')} green_t={float(af.get('green_t',0)):.2f}s curve={af.get('curve','?')} cem={af.get('cem','?')} exp={af.get('exp','?')}")
        print(f"  ** TRACKED OVERSHOOT:{tracked_at_stop:>6.1f} ft **")
      print(f"  ** MODEL OVERSHOOT:  {model_at_stop:>6.1f} ft **")
      print(f"  Dist dash→stop:      {dist_traveled_ft:>6.1f} ft")
      print(f"  Speed before stop:   {speed_at_stop:>5.1f} mph")

      valid.append({
        "id": sid,
        "approach_speed": max_speed,
        "tracked_at_stop_ft": tracked_at_stop,
        "model_at_stop_ft": model_at_stop,
        "dist_dash_to_stop_ft": dist_traveled_ft,
        "driver_stopped": brake,
        "force_active": forcing_at_stop,
        "had_force_stop": had_force_stop,
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

  model_vals = [s["model_at_stop_ft"] for s in valid]
  force_stops = [s for s in valid if s["had_force_stop"]]
  manual_stops = [s for s in valid if not s["had_force_stop"]]

  print(f"\n{'Stop':>5s}  {'Speed':>6s}  {'Tracked':>8s}  {'Model':>8s}  {'Dash→Stop':>10s}  {'Mode'}")
  print(f"{'-'*5}  {'-'*6}  {'-'*8}  {'-'*8}  {'-'*10}  {'-'*18}")
  for s in valid:
    who = "DRIVER" if s["driver_stopped"] else "AUTO"
    mode = "FORCE" if s["had_force_stop"] else "MANUAL"
    tracked = f"{s['tracked_at_stop_ft']:>8.1f}" if s["had_force_stop"] else f"{'---':>8s}"
    print(f"{s['id']:>5d}  {s['approach_speed']:>6.0f}  {tracked}  {s['model_at_stop_ft']:>8.1f}  {s['dist_dash_to_stop_ft']:>10.1f}  {mode} ({who})")

  print(f"\n  Model overshoot (model_ft at standstill — lower is better):")
  print(f"    Count:   {len(model_vals)}")
  print(f"    Min:     {min(model_vals):>6.1f} ft")
  print(f"    Max:     {max(model_vals):>6.1f} ft")
  print(f"    Average: {sum(model_vals)/len(model_vals):>6.1f} ft")
  if len(model_vals) > 1:
    mean = sum(model_vals) / len(model_vals)
    std = (sum((x - mean) ** 2 for x in model_vals) / (len(model_vals) - 1)) ** 0.5
    print(f"    Std Dev: {std:>6.1f} ft")

  if force_stops:
    t_vals = [s["tracked_at_stop_ft"] for s in force_stops]
    print(f"\n  Force stop tracked overshoot:")
    print(f"    Count:   {len(t_vals)}")
    print(f"    Min:     {min(t_vals):>6.1f} ft")
    print(f"    Max:     {max(t_vals):>6.1f} ft")
    print(f"    Average: {sum(t_vals)/len(t_vals):>6.1f} ft")

  if manual_stops:
    m_vals = [s["model_at_stop_ft"] for s in manual_stops]
    d_vals = [s["dist_dash_to_stop_ft"] for s in manual_stops]
    print(f"\n  Manual (lat only) stops — ground truth:")
    print(f"    Count:   {len(m_vals)}")
    print(f"    Model overshoot avg: {sum(m_vals)/len(m_vals):>6.1f} ft")
    print(f"    Dash→stop dist avg:  {sum(d_vals)/len(d_vals):>6.1f} ft")


if __name__ == "__main__":
  main()
