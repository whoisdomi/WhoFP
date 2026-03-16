#!/usr/bin/env python3
"""
Verify top stop sign candidates from full CAN scan.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_stop_candidates.py

Monitors the top candidates in real-time. Watch for which ones
change exactly when the dashboard stop sign icon appears/disappears.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import time

def main():
  sm = messaging.SubMaster(["can"])

  t0 = time.monotonic()

  # Track previous values for change detection
  prev = {}

  # Top candidates: (address, bus, byte_index, label)
  candidates = [
    (0x255, 1, 25, "0x255:B1:B25"),   # Best: only values 5/6
    (0x361, 2, 26, "0x361:B2:B26"),   # ON-only=15, All=[15,16,32]
    (0x363, 2, 18, "0x363:B2:B18"),   # ON-only=15, All=[15,16,32]
    (0x362, 2, 14, "0x362:B2:B14"),   # ON-only=[88,89,112]
    (0x362, 2, 16, "0x362:B2:B16"),   # ON-only=17
    (0x3A6, 1, 0,  "0x3A6:B1:B0"),    # ON-only=31
  ]

  print("Stop sign candidate verifier")
  print("Watch for changes when dashboard stop sign appears/disappears")
  print(f"  {'Time':>6s}", end="")
  for _, _, _, label in candidates:
    print(f"  {label:>14s}", end="")
  print("  Changes")
  print("-" * 120)

  while True:
    sm.update(100)

    if sm.updated["can"]:
      current = {}
      for msg in sm["can"]:
        for addr, bus, byte_idx, label in candidates:
          if msg.address == addr and msg.src == bus:
            data = bytes(msg.dat)
            if byte_idx < len(data):
              current[(addr, bus, byte_idx)] = data[byte_idx]

      # Check for changes
      changes = []
      for addr, bus, byte_idx, label in candidates:
        key = (addr, bus, byte_idx)
        val = current.get(key)
        if val is not None and key in prev and prev[key] != val:
          changes.append(f"{label}: {prev[key]}->{val}")

      if changes:
        elapsed = time.monotonic() - t0
        print(f"  {elapsed:6.1f}", end="")
        for addr, bus, byte_idx, label in candidates:
          key = (addr, bus, byte_idx)
          val = current.get(key, "?")
          print(f"  {val:>14}", end="")
        print(f"  {', '.join(changes)}")

      # Update prev
      for key, val in current.items():
        prev[key] = val

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print("\nDone.")
