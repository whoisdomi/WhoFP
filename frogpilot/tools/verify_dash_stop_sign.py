#!/usr/bin/env python3
"""
Identify the stop sign type byte from CAM_0x362 and CAM_0x363.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

When BYTE22 (distance) becomes non-zero, snapshots all bytes from both
0x362 and 0x363 so we can compare stop sign vs speed limit detections.

Press ENTER to mark when the dashboard stop sign icon appears/disappears.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging
import sys
import select
import termios
import tty

def check_keypress():
  """Non-blocking check for keypress. Returns True if a key was pressed."""
  if select.select([sys.stdin], [], [], 0)[0]:
    sys.stdin.read(1)
    return True
  return False

def main():
  # Set terminal to raw mode for non-blocking key detection
  old_settings = termios.tcgetattr(sys.stdin)
  tty.setcbreak(sys.stdin.fileno())

  try:
    sm = messaging.SubMaster(["can"])

    prev_active = False
    dash_icon_on = False
    msg_362 = None
    msg_363 = None
    baseline_362 = None
    baseline_363 = None
    detection_count = 0

    print("Sign type identifier — shows byte snapshots when signs are detected")
    print("Note which bytes differ between stop signs vs other signs")
    print()
    print("  Press ENTER when dash stop sign icon APPEARS")
    print("  Press ENTER again when dash stop sign icon DISAPPEARS")
    print("-" * 70)
    print("Capturing baselines (no sign detected)...")

    while True:
      sm.update(100)

      # Check for keypress to mark dashboard icon
      if check_keypress():
        dash_icon_on = not dash_icon_on
        if dash_icon_on:
          print(f"  >>> DASH ICON ON  (marked by user)")
          # Snapshot current state
          if msg_362 and len(msg_362) > 22:
            print(f"      0x362 BYTE22={msg_362[22]} at mark")
          if msg_363 and len(msg_363) > 4:
            print(f"      0x363 BYTE4={msg_363[4]} at mark")
        else:
          print(f"  >>> DASH ICON OFF (marked by user)")
          if msg_362 and len(msg_362) > 22:
            print(f"      0x362 BYTE22={msg_362[22]} at mark")

      if sm.updated["can"]:
        for msg in sm["can"]:
          if msg.src != 2:
            continue
          data = bytes(msg.dat)
          if msg.address == 0x362:
            msg_362 = data
          elif msg.address == 0x363:
            msg_363 = data

        if msg_362 is None or msg_363 is None:
          continue

        active = len(msg_362) > 22 and msg_362[22] > 0

        # Capture baseline when no sign is detected
        if not active and not prev_active:
          baseline_362 = msg_362
          baseline_363 = msg_363

        # Sign just appeared — show the bytes that changed from baseline
        if active and not prev_active and baseline_362 and baseline_363:
          detection_count += 1
          dist = msg_362[22]
          print(f"\n=== Detection #{detection_count}  (dist={dist}) ===")

          print("  0x362 changed bytes:")
          for i in range(min(len(baseline_362), len(msg_362))):
            if baseline_362[i] != msg_362[i]:
              print(f"    BYTE[{i:>2d}]: {baseline_362[i]:>3d} -> {msg_362[i]:>3d}  "
                    f"(0x{baseline_362[i]:02X} -> 0x{msg_362[i]:02X})")

          print("  0x363 changed bytes:")
          for i in range(min(len(baseline_363), len(msg_363))):
            if baseline_363[i] != msg_363[i]:
              print(f"    BYTE[{i:>2d}]: {baseline_363[i]:>3d} -> {msg_363[i]:>3d}  "
                    f"(0x{baseline_363[i]:02X} -> 0x{msg_363[i]:02X})")

        # Sign just cleared
        if not active and prev_active:
          print(f"  --- Detection #{detection_count} cleared ---")

        prev_active = active

  finally:
    # Restore terminal settings
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)

if __name__ == "__main__":
  main()
