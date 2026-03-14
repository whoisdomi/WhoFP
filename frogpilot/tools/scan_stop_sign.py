#!/usr/bin/env python3
"""
Scan 0x362 bytes to find the stop sign discriminator.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/scan_stop_sign.py

HOW TO USE:
  1. Drive normally until you see a stop sign icon on the dashboard
  2. Turn ON hazard lights to mark "stop sign visible"
  3. When the stop sign icon disappears, turn OFF hazard lights
  4. Repeat for multiple stop signs
  5. Ctrl+C to stop — prints summary of bytes that differ between ON/OFF

The script captures all 32 bytes of 0x362 during hazard-ON (stop sign visible)
and hazard-OFF (no stop sign) windows, then shows which bytes are unique to
each state.
"""
import cereal.messaging as messaging
import time
from collections import defaultdict

def main():
  sm = messaging.SubMaster(["can", "carState"])

  t0 = time.monotonic()
  hazard_on = False
  prev_hazard = False
  marking = False  # True when user has marked "stop sign visible"

  on_vals = defaultdict(set)   # byte values during stop sign visible
  off_vals = defaultdict(set)  # byte values during no stop sign
  on_count = 0
  off_count = 0
  mark_count = 0

  print("Stop sign byte scanner: use HAZARD LIGHTS to mark stop sign on dash")
  print("  Hazards ON  = stop sign visible on dashboard")
  print("  Hazards OFF = no stop sign on dashboard")
  print("  Ctrl+C      = stop and show results")
  print("-" * 60)

  while True:
    sm.update(100)

    # Check hazard state from carState
    if sm.updated["carState"]:
      hazard_on = sm["carState"].leftBlinker and sm["carState"].rightBlinker
      if hazard_on and not prev_hazard:
        marking = True
        mark_count += 1
        elapsed = time.monotonic() - t0
        print(f"  {elapsed:6.1f}s  >>> MARKING ON (stop sign #{mark_count} visible)")
      elif not hazard_on and prev_hazard and marking:
        marking = False
        elapsed = time.monotonic() - t0
        print(f"  {elapsed:6.1f}s  <<< MARKING OFF (stop sign gone)")
      prev_hazard = hazard_on

    # Capture 0x362 bytes
    if sm.updated["can"]:
      for msg in sm["can"]:
        if msg.address == 0x362 and msg.src == 2:
          data = bytes(msg.dat)
          elapsed = time.monotonic() - t0

          if marking:
            on_count += 1
            for i, val in enumerate(data):
              on_vals[i].add(val)
          else:
            off_count += 1
            for i, val in enumerate(data):
              off_vals[i].add(val)

          # Show B22 changes during marking for reference
          if marking and len(data) > 22:
            b22 = data[22]
            if on_count == 1 or on_count % 5 == 0:
              print(f"  {elapsed:6.1f}s    B22={b22}")

  # This runs on Ctrl+C via the except below

def print_results(on_vals, off_vals, on_count, off_count, mark_count):
  print(f"\n{'=' * 60}")
  print(f"RESULTS: {mark_count} stop sign(s) marked")
  print(f"  ON frames:  {on_count}")
  print(f"  OFF frames: {off_count}")
  print(f"{'=' * 60}")

  if on_count == 0:
    print("\nNo stop signs were marked! Drive past stop signs with hazards ON.")
    return

  if off_count == 0:
    print("\nNo OFF data collected! Need some driving without hazards too.")
    return

  print("\nBytes that DIFFER between stop sign ON vs OFF:")
  print(f"  {'Byte':>6s}  {'ON-only values':>30s}  {'OFF-only values':>30s}  Note")
  print("-" * 80)

  candidates = []
  for i in sorted(set(on_vals.keys()) | set(off_vals.keys())):
    sv = on_vals.get(i, set())
    ov = off_vals.get(i, set())
    on_only = sv - ov
    off_only = ov - sv

    if on_only or off_only:
      note = ""
      # Flag bytes where ON has values that never appear in OFF
      if on_only and not off_only:
        note = "<<< STOP-ONLY"
        candidates.append((i, on_only, "stop-only values"))
      elif off_only and not on_only:
        note = "(off-only)"
      else:
        note = "overlap"
        candidates.append((i, on_only, "different but overlapping"))

      print(f"  B[{i:>2d}]  {str(sorted(on_only)):>30s}  {str(sorted(off_only)):>30s}  {note}")

  if candidates:
    print(f"\n{'=' * 60}")
    print("BEST CANDIDATES (values only present during stop sign):")
    for i, vals, desc in candidates:
      print(f"  BYTE[{i}]: values {sorted(vals)} — {desc}")
  else:
    print("\nNo clear discriminator found. Try marking more stop signs.")


if __name__ == "__main__":
  _on_vals = defaultdict(set)
  _off_vals = defaultdict(set)
  _on_count = 0
  _off_count = 0
  _mark_count = 0

  sm = messaging.SubMaster(["can", "carState"])
  t0 = time.monotonic()
  hazard_on = False
  prev_hazard = False
  marking = False

  print("Stop sign byte scanner: use HAZARD LIGHTS to mark stop sign on dash")
  print("  Hazards ON  = stop sign visible on dashboard")
  print("  Hazards OFF = no stop sign on dashboard")
  print("  Ctrl+C      = stop and show results")
  print("-" * 60)

  try:
    while True:
      sm.update(100)

      if sm.updated["carState"]:
        hazard_on = sm["carState"].leftBlinker and sm["carState"].rightBlinker
        if hazard_on and not prev_hazard:
          marking = True
          _mark_count += 1
          elapsed = time.monotonic() - t0
          print(f"  {elapsed:6.1f}s  >>> MARKING ON (stop sign #{_mark_count} visible)")
        elif not hazard_on and prev_hazard and marking:
          marking = False
          elapsed = time.monotonic() - t0
          print(f"  {elapsed:6.1f}s  <<< MARKING OFF (stop sign gone)")
        prev_hazard = hazard_on

      if sm.updated["can"]:
        for msg in sm["can"]:
          if msg.address == 0x362 and msg.src == 2:
            data = bytes(msg.dat)
            elapsed = time.monotonic() - t0

            if marking:
              _on_count += 1
              for i, val in enumerate(data):
                _on_vals[i].add(val)
            else:
              _off_count += 1
              for i, val in enumerate(data):
                _off_vals[i].add(val)

            if marking and len(data) > 22:
              b22 = data[22]
              if _on_count == 1 or _on_count % 5 == 0:
                print(f"  {elapsed:6.1f}s    B22={b22}")

  except KeyboardInterrupt:
    print("\n")
    print_results(_on_vals, _off_vals, _on_count, _off_count, _mark_count)
    print("\nDone.")
