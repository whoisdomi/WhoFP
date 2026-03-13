#!/usr/bin/env python3
"""
Check if CCNC_0x162 (message 354) exists on any bus, and monitor its SIGNS byte.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Also checks if the SIGNS encoding (BYTE[5]) lines up with 0x362 data.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import time

SIGN_NAMES = {
  0: "HIDDEN", 1: "PEDESTRIAN_CROSSING", 2: "SCHOOL_CROSSWALK",
  8: "STOP", 9: "YIELD", 16: "DO_NOT_PASS", 19: "DO_NOT_ENTER",
  24: "ROUNDABOUT", 26: "RIGHT_CURVE_AHEAD", 27: "LEFT_CURVE_AHEAD",
  28: "SLIGHT_RIGHT_CURVE_AHEAD", 29: "SLIGHT_LEFT_CURVE_AHEAD",
}

def main():
  sm = messaging.SubMaster(["can"])

  seen_162 = set()  # buses where 0x162 was seen
  seen_362 = {}     # bus -> last data
  start = time.monotonic()

  print("Scanning for CCNC_0x162 (msg 354) on all buses...")
  print("Also monitoring 0x362 BYTE[5] for SIGNS mapping")
  print("-" * 60)

  while True:
    sm.update(200)

    if sm.updated["can"]:
      for msg in sm["can"]:
        # Check for CCNC_0x162
        if msg.address == 0x162:
          data = bytes(msg.dat)
          if msg.src not in seen_162:
            seen_162.add(msg.src)
            print(f"\n  *** FOUND 0x162 on Bus {msg.src}! ***")
          if len(data) > 5:
            signs_val = data[5]
            sign_name = SIGN_NAMES.get(signs_val, f"UNKNOWN({signs_val})")
            speed = data[4] if len(data) > 4 else 0
            print(f"  0x162 Bus {msg.src}: SIGNS={signs_val} ({sign_name})  "
                  f"SPEED={speed}  raw={data[:8].hex()}")

        # Monitor 0x362 for comparison
        if msg.address == 0x362 and msg.src == 2:
          data = bytes(msg.dat)
          if len(data) > 22:
            dist = data[22]
            prev = seen_362.get(msg.src)
            if dist > 0 and (prev is None or prev != dist):
              b5 = data[5]
              sign_name = SIGN_NAMES.get(b5, f"?({b5})")
              print(f"  0x362 Bus {msg.src}: dist={dist:>3d}  "
                    f"BYTE[5]={b5} ({sign_name})  "
                    f"BYTE[3]={data[3]}  BYTE[4]={data[4]}")
            seen_362[msg.src] = dist

    # Periodic status
    elapsed = time.monotonic() - start
    if int(elapsed) % 15 == 0 and int(elapsed) > 0 and int(elapsed * 5) % 5 == 0:
      if not seen_162:
        pass  # silently continue scanning

if __name__ == "__main__":
  main()
