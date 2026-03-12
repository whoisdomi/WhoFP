#!/usr/bin/env python3
"""
Raw CAN scanner for dashboard stop sign signal.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Step 1: Park near a stop sign where the dash symbol is OFF. Wait 5 seconds.
Step 2: Drive forward until the dash stop sign symbol appears.
Step 3: Press Ctrl+C to stop.

The script captures a "baseline" of all CAN values, then prints any byte
that changes — highlighting which message ID and bus carries the stop sign signal.
"""
import cereal.messaging as messaging

def main():
  sm = messaging.SubMaster(["can"])

  # Phase 1: Collect baseline (no stop sign on dash)
  print("Phase 1: Collecting baseline CAN data (no stop sign on dash)")
  print("         Wait 5 seconds, then drive toward a stop sign...")
  print()

  baseline = {}  # (bus, addr) -> last seen data bytes
  frames_collected = 0
  target_frames = 100  # ~5 seconds at 20Hz

  while frames_collected < target_frames:
    sm.update(100)
    if sm.updated["can"]:
      for msg in sm["can"]:
        key = (msg.src, msg.address)
        baseline[key] = bytes(msg.dat)
      frames_collected += 1

  print(f"Baseline captured: {len(baseline)} unique CAN messages")
  print()
  print("Phase 2: Monitoring for changes...")
  print("         Drive until the stop sign symbol appears on your dash.")
  print("         Changed bytes will print below.")
  print("-" * 70)

  # Phase 2: Watch for changes
  seen_changes = {}  # (bus, addr, byte_idx) -> set of values

  while True:
    sm.update(100)
    if sm.updated["can"]:
      for msg in sm["can"]:
        key = (msg.src, msg.address)
        data = bytes(msg.dat)

        if key in baseline and data != baseline[key]:
          old = baseline[key]
          for i in range(min(len(old), len(data))):
            if old[i] != data[i]:
              change_key = (msg.src, msg.address, i)
              if change_key not in seen_changes:
                seen_changes[change_key] = set()
              if data[i] not in seen_changes[change_key]:
                seen_changes[change_key].add(data[i])
                addr_hex = f"0x{msg.address:03X}"
                print(f"  Bus {msg.src}  Msg {addr_hex} ({msg.address:>4d})  "
                      f"Byte[{i}]: {old[i]:>3d} -> {data[i]:>3d}  "
                      f"(0x{old[i]:02X} -> 0x{data[i]:02X})")

if __name__ == "__main__":
  main()
