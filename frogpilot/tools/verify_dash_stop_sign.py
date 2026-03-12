#!/usr/bin/env python3
"""
Focused CAN scanner for dashboard stop sign signal.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Phase 1 (10 sec): Drive normally with NO stop sign on dash. Learns which
        bytes are "noisy" (counters/checksums) so they can be filtered out.
Phase 2: Only shows bytes that change AND weren't noisy in Phase 1.
         Drive toward a stop sign. The output should be very sparse —
         only the actual stop sign flag will appear.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging

def main():
  sm = messaging.SubMaster(["can"])

  # Phase 1: Learn which bytes change continuously (noisy)
  print("Phase 1: Learning noisy bytes (10 seconds)...")
  print("         Drive normally — NO stop sign on dash!")
  print()

  baseline = {}       # (bus, addr) -> bytearray of last seen data
  noisy_bytes = set()  # (bus, addr, byte_idx) — bytes that changed during baseline
  frames = 0
  target = 200  # ~10 seconds at 20Hz

  while frames < target:
    sm.update(100)
    if sm.updated["can"]:
      for msg in sm["can"]:
        key = (msg.src, msg.address)
        data = bytes(msg.dat)
        if key in baseline:
          old = baseline[key]
          for i in range(min(len(old), len(data))):
            if old[i] != data[i]:
              noisy_bytes.add((msg.src, msg.address, i))
        baseline[key] = data
      frames += 1

  print(f"Learned {len(noisy_bytes)} noisy byte positions (filtered out)")
  print()
  print("Phase 2: Monitoring for stop sign signal...")
  print("         Drive toward a stop sign. ONLY non-noisy changes will show.")
  print("-" * 70)

  # Phase 2: Watch for changes, filtering out noisy bytes
  while True:
    sm.update(100)
    if sm.updated["can"]:
      for msg in sm["can"]:
        key = (msg.src, msg.address)
        data = bytes(msg.dat)
        if key in baseline:
          old = baseline[key]
          for i in range(min(len(old), len(data))):
            change_key = (msg.src, msg.address, i)
            if old[i] != data[i] and change_key not in noisy_bytes:
              addr_hex = f"0x{msg.address:03X}"
              print(f"  Bus {msg.src:>3d}  Msg {addr_hex} ({msg.address:>4d})  "
                    f"Byte[{i:>2d}]: {old[i]:>3d} -> {data[i]:>3d}  "
                    f"(0x{old[i]:02X} -> 0x{data[i]:02X})")
              # Update baseline so we see the next transition too
              ba = bytearray(baseline[key])
              ba[i] = data[i]
              baseline[key] = bytes(ba)

if __name__ == "__main__":
  main()
