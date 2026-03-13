#!/usr/bin/env python3
"""
Find the real stop sign signal.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Press ENTER when dash stop sign appears/disappears.
Shows 0x120 bytes + 0x362 distance on each change.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import sys
import select
import termios
import tty
import time

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
    prev_120 = None
    prev_362_dist = -1
    t0 = time.monotonic()

    print("Press ENTER when dash stop sign appears/disappears")
    print("-" * 70)

    while True:
      sm.update(100)

      if check_keypress():
        dash_on = not dash_on
        elapsed = time.monotonic() - t0
        tag = ">>> DASH ON" if dash_on else "<<< DASH OFF"
        print(f"  {elapsed:6.1f}  {tag}")

      if sm.updated["can"]:
        for msg in sm["can"]:
          data = bytes(msg.dat)

          # Show all 0x120 bytes when any change
          if msg.address == 0x120 and msg.src == 1 and len(data) >= 8:
            current = tuple(data[:8])
            if current != prev_120:
              elapsed = time.monotonic() - t0
              hex_str = " ".join(f"{b:02X}" for b in data[:8])
              icon = " <DASH>" if dash_on else ""
              print(f"  {elapsed:6.1f}  0x120: {hex_str}{icon}")
              prev_120 = current

          # Show 0x362 distance changes
          if msg.address == 0x362 and msg.src == 2 and len(data) > 22:
            dist = data[22]
            if dist != prev_362_dist:
              if dist > 0 or prev_362_dist > 0:
                elapsed = time.monotonic() - t0
                icon = " <DASH>" if dash_on else ""
                print(f"  {elapsed:6.1f}  0x362 dist={dist:<3d}{icon}")
              prev_362_dist = dist

  except KeyboardInterrupt:
    pass
  finally:
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)

if __name__ == "__main__":
  main()
