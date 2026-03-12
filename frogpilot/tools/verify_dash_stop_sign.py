#!/usr/bin/env python3
"""
Verify dashboard stop sign signal via raw CAN.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Shows CAM_0x362 BYTE22 (distance) and CAM_0x363 BYTE4 (possible sign type)
side by side so we can figure out which value means "stop sign."

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging

def main():
  sm = messaging.SubMaster(["can"])

  prev_362_b22 = None
  prev_363_b4 = None

  print("Monitoring CAM_0x362 and CAM_0x363 for stop sign signals...")
  print("  Watch for which 0x363 BYTE4 value correlates with the")
  print("  dashboard stop sign icon appearing.")
  print("-" * 70)

  while True:
    sm.update(100)

    if sm.updated["can"]:
      for msg in sm["can"]:
        if msg.src != 2:
          continue
        data = bytes(msg.dat)

        if msg.address == 0x362 and len(data) > 22:
          b22 = data[22]
          if b22 != prev_362_b22:
            print(f"  0x362 BYTE22 (dist): {b22:>3d}  (0x{b22:02X})")
            prev_362_b22 = b22

        if msg.address == 0x363 and len(data) > 4:
          b4 = data[4]
          if b4 != prev_363_b4:
            print(f"  0x363 BYTE4  (type): {b4:>3d}  (0x{b4:02X})")
            prev_363_b4 = b4

if __name__ == "__main__":
  main()
