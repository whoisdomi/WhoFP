#!/usr/bin/env python3
"""
Identify the stop sign type byte — full hex dump version.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Prints a full hex dump of 0x362 at the START of each detection.
After Ctrl+C, prints a summary table for easy comparison.

Press ENTER to tag the current/most recent detection as "STOP SIGN".

Press Ctrl+C to stop and see summary.
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

def fmt_hex_row(data, start, end):
  return " ".join(f"{data[i]:02X}" for i in range(start, min(end, len(data))))

def main():
  old_settings = termios.tcgetattr(sys.stdin)
  tty.setcbreak(sys.stdin.fileno())

  try:
    sm = messaging.SubMaster(["can"])

    prev_active = False
    msg_362 = None
    msg_363 = None
    detection_count = 0
    detections = []  # list of (det#, dist, msg_362_bytes, msg_363_bytes, tagged)

    print("Full hex dump — tag stop signs with ENTER")
    print("Press Ctrl+C when done to see comparison summary")
    print("-" * 70)

    while True:
      sm.update(100)

      if check_keypress():
        if detections:
          detections[-1] = (*detections[-1][:4], True)
          det = detections[-1]
          print(f"  >>> Tagged detection #{det[0]} as STOP SIGN")

      if sm.updated["can"]:
        for msg in sm["can"]:
          if msg.src != 2:
            continue
          data = bytes(msg.dat)
          if msg.address == 0x362:
            msg_362 = data
          elif msg.address == 0x363:
            msg_363 = data

        if msg_362 is None or len(msg_362) < 25:
          continue

        dist = msg_362[22]
        active = dist > 0

        if active and not prev_active:
          detection_count += 1
          snap_362 = bytes(msg_362)
          snap_363 = bytes(msg_363) if msg_363 else b""
          detections.append((detection_count, dist, snap_362, snap_363, False))
          print(f"\n=== Detection #{detection_count}  (dist={dist}) ===")
          print(f"  0x362: {fmt_hex_row(snap_362, 0, 16)}")
          print(f"         {fmt_hex_row(snap_362, 16, 32)}")
          if snap_363:
            print(f"  0x363: {fmt_hex_row(snap_363, 0, 16)}")
            print(f"         {fmt_hex_row(snap_363, 16, 32)}")

        if not active and prev_active:
          print(f"  --- #{detection_count} cleared ---")

        prev_active = active

  except KeyboardInterrupt:
    pass
  finally:
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)

  # Print summary
  if not detections:
    print("\nNo detections recorded.")
    return

  print("\n" + "=" * 70)
  print("SUMMARY — compare tagged (STOP) vs untagged detections")
  print("=" * 70)

  for det_num, dist, snap_362, snap_363, tagged in detections:
    label = "STOP SIGN" if tagged else "other"
    print(f"\n#{det_num} ({label}, dist={dist}):")
    # Print bytes with index headers
    header = "     " + " ".join(f"{i:>2d}" for i in range(32))
    print(header)
    vals = "362: " + " ".join(f"{snap_362[i]:02X}" if i < len(snap_362) else "  " for i in range(32))
    print(vals)
    if snap_363:
      vals = "363: " + " ".join(f"{snap_363[i]:02X}" if i < len(snap_363) else "  " for i in range(32))
      print(vals)

  # Highlight bytes that differ between tagged and untagged
  tagged_dets = [d for d in detections if d[4]]
  untagged_dets = [d for d in detections if not d[4]]
  if tagged_dets and untagged_dets:
    print(f"\n--- Bytes that differ (STOP vs others) in 0x362 ---")
    for i in range(32):
      stop_vals = set(d[2][i] for d in tagged_dets if i < len(d[2]))
      other_vals = set(d[2][i] for d in untagged_dets if i < len(d[2]))
      if stop_vals and other_vals and not stop_vals & other_vals:
        print(f"  BYTE[{i:>2d}]: STOP={stop_vals}  others={other_vals}")

    if tagged_dets[0][3]:
      print(f"\n--- Bytes that differ (STOP vs others) in 0x363 ---")
      for i in range(32):
        stop_vals = set(d[3][i] for d in tagged_dets if d[3] and i < len(d[3]))
        other_vals = set(d[3][i] for d in untagged_dets if d[3] and i < len(d[3]))
        if stop_vals and other_vals and not stop_vals & other_vals:
          print(f"  BYTE[{i:>2d}]: STOP={stop_vals}  others={other_vals}")

if __name__ == "__main__":
  main()
