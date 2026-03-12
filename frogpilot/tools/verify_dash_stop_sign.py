#!/usr/bin/env python3
"""
Verify dashboard stop sign signal via frogpilotCarState.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Monitors frogpilotCarState.dashboardStopSign and the raw CAM_0x362 BYTE22
value so you can confirm detection works end-to-end.

Press Ctrl+C to stop.
"""
import cereal.messaging as messaging

def main():
  sm = messaging.SubMaster(["frogpilotCarState", "can"])

  prev_detected = False
  prev_byte22 = 0

  print("Monitoring dashboard stop sign...")
  print("  - dashboardStopSign: from frogpilotCarState (bool)")
  print("  - BYTE22: raw distance from CAM_0x362 on CAM bus")
  print("-" * 60)

  while True:
    sm.update(100)

    # Read the parsed frogpilotCarState value
    if sm.updated["frogpilotCarState"]:
      detected = sm["frogpilotCarState"].dashboardStopSign
      if detected != prev_detected:
        state = "ON" if detected else "OFF"
        print(f"  dashboardStopSign: {state}")
        prev_detected = detected

    # Read raw CAN to show BYTE22 distance
    if sm.updated["can"]:
      for msg in sm["can"]:
        if msg.address == 0x362:
          data = bytes(msg.dat)
          if len(data) > 22:
            byte22 = data[22]
            if byte22 != prev_byte22:
              print(f"  CAM_0x362 BYTE22: {byte22:>3d}  (0x{byte22:02X})  bus={msg.src}")
              prev_byte22 = byte22

if __name__ == "__main__":
  main()
