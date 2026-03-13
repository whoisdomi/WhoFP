#!/usr/bin/env python3
"""
Triple-check stop sign signal identifier.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Monitors three things simultaneously:
  1. CCNC_0x162 on ALL buses (the known stop sign message)
  2. 0x1B5 (FR_CMR_03) bytes 12-23 (only changed during stop sign display)
  3. 0x362 BYTE[22] (sign distance)

Press ENTER to mark dash stop sign ON/OFF.
Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import sys
import select
import termios
import tty

SIGN_NAMES = {
  0: "HIDDEN", 1: "PED_CROSSING", 2: "SCHOOL",
  8: "STOP", 9: "YIELD", 16: "DO_NOT_PASS", 19: "DO_NOT_ENTER",
  24: "ROUNDABOUT", 26: "R_CURVE", 27: "L_CURVE",
}

def check_keypress():
  if select.select([sys.stdin], [], [], 0)[0]:
    sys.stdin.read(1)
    return True
  return False

def main():
  old_settings = termios.tcgetattr(sys.stdin)
  tty.setcbreak(sys.stdin.fileno())

  try:
    sm = messaging.SubMaster(["can"])

    dash_on = False
    found_162 = set()
    prev_362_dist = -1
    prev_1b5_vals = None

    print("Triple-check stop sign signal identifier")
    print("Press ENTER to mark dash stop sign ON/OFF")
    print("-" * 60)

    while True:
      sm.update(100)

      if check_keypress():
        dash_on = not dash_on
        tag = ">>> DASH ON" if dash_on else "<<< DASH OFF"
        print(f"  {tag}")

      if sm.updated["can"]:
        for msg in sm["can"]:
          data = bytes(msg.dat)

          # 1. Check for CCNC_0x162 on ANY bus
          if msg.address == 0x162:
            if msg.src not in found_162:
              found_162.add(msg.src)
              print(f"  *** FOUND 0x162 on Bus {msg.src}! ***")
            if len(data) > 5:
              signs = data[5]
              name = SIGN_NAMES.get(signs, f"?({signs})")
              print(f"  0x162 Bus {msg.src}: SIGNS={signs} ({name})  "
                    f"raw={data[:8].hex()}")

          # 2. Monitor 0x1B5 bytes 12-23 (bus 1 only)
          if msg.address == 0x1B5 and msg.src == 1 and len(data) > 23:
            vals = tuple(data[12:24])
            if vals != prev_1b5_vals:
              # Only print if any byte is non-zero (skip all-zero noise)
              if any(v != 0 for v in vals):
                icon = " <DASH>" if dash_on else ""
                hex_str = " ".join(f"{v:02X}" for v in vals)
                print(f"  0x1B5 B[12-23]: {hex_str}{icon}")
              prev_1b5_vals = vals

          # 3. Monitor 0x362 BYTE[22] distance (bus 2)
          if msg.address == 0x362 and msg.src == 2 and len(data) > 22:
            dist = data[22]
            if dist != prev_362_dist:
              if dist > 0 or prev_362_dist > 0:
                icon = " <DASH>" if dash_on else ""
                print(f"  0x362 dist={dist:<3d}{icon}")
              prev_362_dist = dist

  except KeyboardInterrupt:
    if not found_162:
      print("\n  0x162 was NOT found on any bus.")
  finally:
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)

if __name__ == "__main__":
  main()
