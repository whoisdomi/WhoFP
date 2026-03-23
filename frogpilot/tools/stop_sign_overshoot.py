#!/usr/bin/env python3
"""
Measure force stop overshoot at stop signs.

Records how many feet force stop thinks are remaining when:
  1. Dashboard stop sign first appears (initial offset)
  2. Each frame as you decelerate (approach profile)
  3. You reach standstill (final overshoot — the key number)

Run on comma device via SSH while driving:
  python3 /data/openpilot/frogpilot/tools/stop_sign_overshoot.py

Drive through several stop signs, then Ctrl+C to see the summary.
"""
import cereal.messaging as messaging
import time

M_TO_FT = 3.28084


def print_stop_summary(stop):
  print(f"\n  --- Stop #{stop['id']} Summary ---")
  print(f"  Speed when dash appeared: {stop['dash_on_speed_mph']:.0f} mph")
  print(f"  Force stop activated: {'Yes' if stop['force_stop_activated'] else 'No'}")

  if stop["dash_on_force_len_ft"] is not None:
    print(f"  Force stop length at dash-on: {stop['dash_on_force_len_ft']:.1f} ft")

  if stop["reached_standstill"]:
    if stop["standstill_force_len_ft"] is not None:
      print(f"  ** OVERSHOOT (force stop at standstill): {stop['standstill_force_len_ft']:.1f} ft **")
    else:
      print(f"  Force stop was not active at standstill")
    print(f"  Time to stop: {stop['standstill_time']:.1f}s")
  else:
    print(f"  Did not reach standstill (rolling stop or no force stop)")
  print()


def print_final_summary(stops):
  print("\n" + "=" * 65)
  print("FINAL SUMMARY")
  print("=" * 65)

  if not stops:
    print("No stops recorded.")
    return

  # Filter to stops where force stop was active and standstill was reached
  valid = [s for s in stops if s["force_stop_activated"] and s["reached_standstill"] and s["standstill_force_len_ft"] is not None]

  print(f"\nTotal stop signs detected: {len(stops)}")
  print(f"Stops with force stop + standstill: {len(valid)}")

  if not valid:
    print("No valid force-stop events to analyze.")
    return

  overshoots = [s["standstill_force_len_ft"] for s in valid]
  dash_on_lens = [s["dash_on_force_len_ft"] for s in valid if s["dash_on_force_len_ft"] is not None]

  print(f"\n{'Stop':>5s}  {'Speed':>6s}  {'Dash-On Len':>11s}  {'Overshoot':>10s}")
  print(f"{'#':>5s}  {'(mph)':>6s}  {'(ft)':>11s}  {'(ft)':>10s}")
  print(f"{'-'*5}  {'-'*6}  {'-'*11}  {'-'*10}")
  for s in valid:
    dash_str = f"{s['dash_on_force_len_ft']:.1f}" if s["dash_on_force_len_ft"] is not None else "N/A"
    print(f"{s['id']:>5d}  {s['dash_on_speed_mph']:>6.0f}  {dash_str:>11s}  {s['standstill_force_len_ft']:>10.1f}")

  print(f"\n  Overshoot (ft remaining at standstill):")
  print(f"    Min:     {min(overshoots):>6.1f} ft")
  print(f"    Max:     {max(overshoots):>6.1f} ft")
  print(f"    Average: {sum(overshoots)/len(overshoots):>6.1f} ft")

  if len(overshoots) > 1:
    mean = sum(overshoots) / len(overshoots)
    variance = sum((x - mean) ** 2 for x in overshoots) / (len(overshoots) - 1)
    std_dev = variance ** 0.5
    print(f"    Std Dev: {std_dev:>6.1f} ft")

  if dash_on_lens:
    print(f"\n  Force stop length when dash first detected stop sign:")
    print(f"    Min:     {min(dash_on_lens):>6.1f} ft")
    print(f"    Max:     {max(dash_on_lens):>6.1f} ft")
    print(f"    Average: {sum(dash_on_lens)/len(dash_on_lens):>6.1f} ft")


if __name__ == "__main__":
  # Use a mutable container so main() can build it up and we can
  # access it after Ctrl+C interrupts the loop
  all_stops = []

  def _main_wrapper():
    # Only subscribe to frogpilot messages and carState — do NOT subscribe to
    # modelV2 or other core messages, as that steals them from the longitudinal
    # planner and causes "Communication Issue Between Processes" errors.
    sm = messaging.SubMaster(["frogpilotCarState", "frogpilotPlan", "carState"])

    current = None
    stop_count = 0

    print("Stop Sign Overshoot Tracker")
    print("Drive through stop signs normally. Ctrl+C for summary.")
    print("=" * 65)

    while True:
      sm.update(100)
      if not sm.updated["frogpilotCarState"]:
        continue

      now = time.monotonic()
      dash_on = sm["frogpilotCarState"].dashboardStopSign > 0
      forcing = sm["frogpilotPlan"].forcingStop
      force_len_m = sm["frogpilotPlan"].forcingStopLength
      v_ego = sm["carState"].vEgo
      speed_mph = v_ego * 2.237
      standstill = sm["carState"].standstill
      red_light = sm["frogpilotPlan"].redLight

      if dash_on and current is None:
        stop_count += 1
        current = {
          "id": stop_count,
          "start_time": now,
          "dash_on_force_len_ft": force_len_m * M_TO_FT if forcing else None,
          "dash_on_speed_mph": speed_mph,
          "dash_on_forcing": forcing,
          "dash_on_red_light": red_light,
          "frames": [],
          "standstill_force_len_ft": None,
          "standstill_time": None,
          "reached_standstill": False,
          "force_stop_activated": forcing,
        }
        print(f"\n{'='*65}")
        print(f"STOP #{stop_count} — Dash stop sign ON at {speed_mph:.0f} mph")
        if forcing:
          print(f"  Force stop already active: {force_len_m * M_TO_FT:.1f} ft remaining")
        else:
          print(f"  Force stop NOT yet active")
        print(f"  {'Time':>6s}  {'Speed':>6s}  {'ForceLen':>9s}  {'Status'}")
        print(f"  {'(s)':>6s}  {'(mph)':>6s}  {'(ft)':>9s}")
        print(f"  {'-'*6}  {'-'*6}  {'-'*9}  {'-'*15}")

      if current is not None:
        elapsed = now - current["start_time"]
        force_ft = force_len_m * M_TO_FT

        if forcing:
          current["force_stop_activated"] = True

        status_parts = []
        if forcing:
          status_parts.append("FORCING")
        if dash_on:
          status_parts.append("DASH")
        if red_light:
          status_parts.append("RED")
        status = " ".join(status_parts) or "---"

        frame = {
          "elapsed": elapsed,
          "speed_mph": speed_mph,
          "force_len_ft": force_ft if forcing else None,
          "forcing": forcing,
          "dash_on": dash_on,
        }
        current["frames"].append(frame)

        if len(current["frames"]) % 10 == 1:
          force_str = f"{force_ft:9.1f}" if forcing else f"{'---':>9s}"
          print(f"  {elapsed:6.1f}  {speed_mph:6.1f}  {force_str}  {status}")

        if standstill and not current["reached_standstill"]:
          current["reached_standstill"] = True
          current["standstill_time"] = elapsed
          current["standstill_force_len_ft"] = force_ft if forcing else None
          force_str = f"{force_ft:.1f}" if forcing else "N/A"
          print(f"  >>> STANDSTILL at {elapsed:.1f}s — Force stop remaining: {force_str} ft <<<")

        end_event = False
        if current["reached_standstill"] and not standstill and elapsed - current["standstill_time"] > 4:
          end_event = True
        if not dash_on and not forcing and elapsed > 6:
          end_event = True

        if end_event:
          all_stops.append(current)
          print_stop_summary(current)
          current = None

  try:
    _main_wrapper()
  except KeyboardInterrupt:
    pass

  print_final_summary(all_stops)
  print("\nDone.")
