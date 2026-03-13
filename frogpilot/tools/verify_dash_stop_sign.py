#!/usr/bin/env python3
"""
Focused test on 0x360 — candidate stop sign type message.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Shows 0x360 bytes continuously while 0x362 BYTE22 is active (any sign detected).
Press ENTER to mark when dash stop sign icon is ON/OFF.

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

def fmt_hex(data, start, end):
  return " ".join(f"{data[i]:02X}" for i in range(start, min(end, len(data))))

def main():
  old_settings = termios.tcgetattr(sys.stdin)
  tty.setcbreak(sys.stdin.fileno())

  try:
    sm = messaging.SubMaster(["can"])

    msg_360 = None
    msg_362 = None
    prev_active = False
    dash_on = False
    detection_count = 0
    detections = []  # (det#, dist, 0x360_snap, tagged_as_stop)

    print("0x360 focused test")
    print("Press ENTER to mark dash stop sign ON/OFF")
    print("-" * 70)

    while True:
      sm.update(100)

      if check_keypress():
        dash_on = not dash_on
        tag = "DASH ON" if dash_on else "DASH OFF"
        print(f"  >>> {tag}")
        # Tag the current/most recent detection
        if dash_on and detections:
          detections[-1] = (*detections[-1][:3], True)
          print(f"      (tagged #{detections[-1][0]} as STOP SIGN)")

      if sm.updated["can"]:
        for msg in sm["can"]:
          if msg.src != 2:
            continue
          data = bytes(msg.dat)
          if msg.address == 0x360:
            msg_360 = data
          elif msg.address == 0x362:
            msg_362 = data

        if msg_360 is None or msg_362 is None:
          continue

        dist = msg_362[22] if len(msg_362) > 22 else 0
        active = dist > 0

        if active and not prev_active:
          detection_count += 1
          snap = bytes(msg_360)
          detections.append((detection_count, dist, snap, False))
          icon = " <DASH>" if dash_on else ""
          print(f"\n=== Det #{detection_count} (dist={dist}){icon} ===")
          # Show key bytes from 0x360: 5,6,12,13,20,21,22,28,29,30
          print(f"  0x360: {fmt_hex(snap, 0, 16)}")
          print(f"         {fmt_hex(snap, 16, 32)}")

        if not active and prev_active:
          print(f"  --- #{detection_count} cleared ---")

        prev_active = active

  except KeyboardInterrupt:
    pass
  finally:
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)

  # Summary
  print("\n" + "=" * 60)
  print("SUMMARY")
  print("=" * 60)

  stop_dets = [d for d in detections if d[3]]
  other_dets = [d for d in detections if not d[3]]

  print(f"\nKey bytes from 0x360 (bytes 5,6,12,13,20,21,22,28,29,30):")
  print(f"{'#':>3s} {'type':>5s} {'dist':>4s} | B5  B6  B12 B13 B20 B21 B22 B28 B29 B30")
  print("-" * 70)
  for det_num, dist, snap, is_stop in detections:
    label = "STOP" if is_stop else "other"
    b = snap
    if len(b) >= 31:
      print(f"#{det_num:<3d} {label:>5s} d={dist:<3d} | "
            f"{b[5]:>3d} {b[6]:>3d} {b[12]:>3d} {b[13]:>3d} "
            f"{b[20]:>3d} {b[21]:>3d} {b[22]:>3d} {b[28]:>3d} {b[29]:>3d} {b[30]:>3d}")

  # Highlight differences
  if stop_dets and other_dets:
    print(f"\n--- Byte-by-byte comparison ---")
    for i in range(min(32, len(stop_dets[0][2]))):
      stop_vals = set(d[2][i] for d in stop_dets if i < len(d[2]))
      other_vals = set(d[2][i] for d in other_dets if i < len(d[2]))
      if stop_vals and other_vals and not stop_vals & other_vals:
        print(f"  BYTE[{i:>2d}]: STOP={stop_vals}  others={other_vals}")

if __name__ == "__main__":
  main()
