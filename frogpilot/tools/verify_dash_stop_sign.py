#!/usr/bin/env python3
"""
Verify dashboard stop sign CAN signal (CCNC_0x162 SIGNS == 8).

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Watch the output as you drive past stop signs. When the dashboard
shows the stop sign symbol, this script should print "STOP SIGN DETECTED".
Press Ctrl+C to stop.
"""
import time
import cereal.messaging as messaging

def main():
  sm = messaging.SubMaster(["frogpilotCarState"])

  prev_state = None
  print("Monitoring dashboardStopSign signal... (Ctrl+C to stop)")
  print("-" * 50)

  while True:
    sm.update(100)

    if sm.updated["frogpilotCarState"]:
      current = sm["frogpilotCarState"].dashboardStopSign

      if current != prev_state:
        timestamp = time.strftime("%H:%M:%S")
        if current:
          print(f"[{timestamp}] STOP SIGN DETECTED (dashboardStopSign = True)")
        else:
          print(f"[{timestamp}] Cleared (dashboardStopSign = False)")
        prev_state = current

if __name__ == "__main__":
  main()
