#!/usr/bin/env python3
"""
Identify the stop sign type byte from CAM_0x362.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Shows a compact status line with key byte values while any sign is detected.
Press ENTER to mark when dashboard stop sign icon appears/disappears.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import sys
import select
import termios
import tty

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

    prev_active = False
    dash_icon_on = False
    msg_362 = None
    detection_count = 0
    last_printed_dist = -1

    print("Sign type identifier — compact view")
    print("Press ENTER to mark dash icon ON/OFF")
    print("-" * 70)
    print("Columns: det# | dist | B8  B9  B12 B18 B19 B20 B21 B24 | dash")
    print("-" * 70)

    while True:
      sm.update(100)

      if check_keypress():
        dash_icon_on = not dash_icon_on
        tag = "DASH ON >>>" if dash_icon_on else "DASH OFF <<"
        d = msg_362[22] if msg_362 and len(msg_362) > 22 else 0
        print(f"  *** {tag} (BYTE22={d} at mark)")

      if sm.updated["can"]:
        for msg in sm["can"]:
          if msg.src != 2:
            continue
          if msg.address == 0x362:
            msg_362 = bytes(msg.dat)

        if msg_362 is None or len(msg_362) < 25:
          continue

        dist = msg_362[22]
        active = dist > 0

        if active and not prev_active:
          detection_count += 1
          last_printed_dist = -1

        # Print a line every time distance changes while active
        if active and dist != last_printed_dist:
          b8  = msg_362[8]
          b9  = msg_362[9]
          b12 = msg_362[12]
          b18 = msg_362[18]
          b19 = msg_362[19]
          b20 = msg_362[20]
          b21 = msg_362[21]
          b24 = msg_362[24]
          icon = " <DASH>" if dash_icon_on else ""
          print(f"  #{detection_count:<3d} d={dist:<3d} | "
                f"{b8:>3d} {b9:>3d} {b12:>3d} {b18:>3d} {b19:>3d} {b20:>3d} {b21:>3d} {b24:>3d} |{icon}")
          last_printed_dist = dist

        if not active and prev_active:
          print(f"  --- #{detection_count} cleared ---")

        prev_active = active

  finally:
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)

if __name__ == "__main__":
  main()
