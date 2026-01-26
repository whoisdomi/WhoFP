#!/usr/bin/env python3
"""
Test script to verify STAR button is correctly parsed from DBC
Run this on your comma device after rebuilding opendbc
"""
import cereal.messaging as messaging
from opendbc.can.parser import CANParser
from opendbc.car.hyundai.values import CAR

def main():
  # Create CAN parser for Ioniq 6
  signals = [("STAR_BUTTON", "STEERING_BUTTONS")]
  checks = [("STEERING_BUTTONS", 50)]  # 50 Hz check rate

  parser = CANParser("hyundai_canfd_generated", signals, checks, 1)  # Bus 1 = ECAN

  sm = messaging.SubMaster(['can'])

  print("=" * 60)
  print("STAR Button DBC Test - Press button to verify!")
  print("=" * 60)
  print()

  prev_value = None
  press_count = 0
  release_count = 0

  while True:
    sm.update(100)

    if sm.updated['can']:
      # Update parser with CAN messages
      for msg in sm['can']:
        if msg.src == 1:  # ECAN bus
          parser.update(msg.address, msg.src, bytes(msg.dat), msg.logMonoTime)

      # Get parsed button value
      button_val = parser.vl["STEERING_BUTTONS"]["STAR_BUTTON"]

      if button_val != prev_value and prev_value is not None:
        if button_val == 0xe5:
          press_count += 1
          print(f"{press_count:3d} PRESSED   (DBC value: {int(button_val)})")
        elif button_val == 0xdf:
          release_count += 1
          print(f"{release_count:3d} RELEASED  (DBC value: {int(button_val)})")
        else:
          print(f"    OTHER     (DBC value: {int(button_val)})")

      prev_value = button_val

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print("\nTest complete!")
