#!/usr/bin/env python3
"""
Scan ALL CAN messages to find the stop sign dashboard indicator.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/scan_stop_sign.py

HOW TO USE:
  1. Drive normally for ~30s first (builds baseline of "no stop sign" values)
  2. When stop sign icon appears on dashboard — turn ON hazard lights
  3. When the stop sign icon disappears — turn OFF hazard lights
  4. Repeat for 2+ stop signs (more = better)
  5. Ctrl+C to stop — prints bytes that ONLY appear during stop sign

NOTE: When you press hazards ON, the script automatically includes the
previous 2 seconds of CAN data as "ON" (since the stop sign was likely
already visible before you could react and press the button).

Scans ALL addresses on ALL buses. Results show which (address, bus, byte)
combinations have values unique to the stop sign ON state.
"""
import cereal.messaging as messaging
import time
from collections import defaultdict, deque

LOOKBACK = 2.0  # seconds of data to include before hazard ON press

def main():
  sm = messaging.SubMaster(["can", "carState"])

  t0 = time.monotonic()
  prev_hazard = False
  marking = False
  mark_count = 0

  # Track values per (address, bus, byte_index)
  on_vals = defaultdict(set)
  off_vals = defaultdict(set)
  on_addrs = set()
  off_addrs = set()

  # Rolling buffer: list of (timestamp, [(addr, bus, data), ...]) for lookback
  buffer = deque()

  print("Full CAN scanner: use HAZARD LIGHTS to mark stop sign on dash")
  print("  Hazards ON  = stop sign visible on dashboard")
  print("  Hazards OFF = no stop sign on dashboard")
  print(f"  Includes {LOOKBACK:.0f}s of data BEFORE hazard press (reaction time)")
  print("  Drive ~30s before first marking to build baseline")
  print("  Ctrl+C      = stop and show results")
  print("-" * 60)

  try:
    while True:
      sm.update(100)

      if sm.updated["carState"]:
        hazard_on = sm["carState"].leftBlinker and sm["carState"].rightBlinker
        if hazard_on and not prev_hazard:
          marking = True
          mark_count += 1
          elapsed = time.monotonic() - t0
          print(f"  {elapsed:6.1f}s  >>> MARKING ON (stop sign #{mark_count} visible)")

          # Retroactively move buffered data from OFF to ON
          now = time.monotonic()
          moved = 0
          for buf_time, buf_msgs in buffer:
            if now - buf_time <= LOOKBACK:
              for addr, bus, data in buf_msgs:
                on_addrs.add((addr, bus))
                for i, val in enumerate(data):
                  key = (addr, bus, i)
                  on_vals[key].add(val)
                  # Remove from off_vals is too expensive/complex,
                  # but since we're looking for ON-only values,
                  # adding to ON is what matters
              moved += 1
          if moved > 0:
            print(f"          (included {moved} buffered frames from before press)")

        elif not hazard_on and prev_hazard and marking:
          marking = False
          elapsed = time.monotonic() - t0
          print(f"  {elapsed:6.1f}s  <<< MARKING OFF (stop sign gone)")
        prev_hazard = hazard_on

      if sm.updated["can"]:
        frame_msgs = []
        now = time.monotonic()

        for msg in sm["can"]:
          addr = msg.address
          bus = msg.src
          data = bytes(msg.dat)

          if marking:
            on_addrs.add((addr, bus))
            for i, val in enumerate(data):
              on_vals[(addr, bus, i)].add(val)
          else:
            off_addrs.add((addr, bus))
            for i, val in enumerate(data):
              off_vals[(addr, bus, i)].add(val)
            frame_msgs.append((addr, bus, data))

        # Buffer non-marking frames for lookback
        if not marking and frame_msgs:
          buffer.append((now, frame_msgs))
          # Trim buffer to keep only recent data
          while buffer and now - buffer[0][0] > LOOKBACK + 1:
            buffer.popleft()

  except KeyboardInterrupt:
    pass

  print(f"\n{'=' * 70}")
  print(f"RESULTS: {mark_count} stop sign(s) marked")
  print(f"  ON addresses:  {len(on_addrs)}")
  print(f"  OFF addresses: {len(off_addrs)}")
  print(f"{'=' * 70}")

  if mark_count == 0:
    print("\nNo stop signs marked! Use hazard lights to mark.")
    return

  # Find bytes with ON-only values (values that NEVER appear during OFF)
  candidates = []
  for key in sorted(on_vals.keys()):
    addr, bus, byte_idx = key
    sv = on_vals[key]
    ov = off_vals.get(key, set())
    on_only = sv - ov

    if on_only:
      # Skip bytes that are just noisy counters/checksums (too many unique values)
      if len(sv) > 100:
        continue
      candidates.append((addr, bus, byte_idx, sorted(on_only), sorted(sv), sorted(ov)))

  if candidates:
    print(f"\nBytes with STOP-SIGN-ONLY values ({len(candidates)} found):")
    print(f"  {'Addr':>6s}  {'Bus':>3s}  {'Byte':>4s}  {'ON-only':>30s}  {'All ON':>30s}")
    print("-" * 80)
    for addr, bus, byte_idx, on_only, all_on, all_off in candidates:
      # Highlight strong candidates: few ON-only values, not too many total
      strength = ""
      if len(all_on) <= 10:
        strength = " <<< STRONG"
      elif len(all_on) <= 30:
        strength = " << moderate"
      print(f"  0x{addr:03X}  {bus:>3d}  B[{byte_idx:>2d}]  {str(on_only):>30s}  {str(all_on):>30s}{strength}")
  else:
    print("\nNo bytes found with stop-sign-only values.")
    print("Try marking more stop signs or driving longer between them.")

  # Also show addresses that only appear during ON (rare but possible)
  on_only_addrs = on_addrs - off_addrs
  if on_only_addrs:
    print(f"\nAddresses that ONLY appeared during stop sign marking:")
    for addr, bus in sorted(on_only_addrs):
      print(f"  0x{addr:03X} bus {bus}")

  print("\nDone.")

if __name__ == "__main__":
  main()
