#!/usr/bin/env python3
import time
import datetime
from openpilot.selfdrive.car.isotp_parallel_query import IsoTpParallelQuery
from openpilot.common.swaglog import cloudlog

EXT_DIAG_REQUEST = b'\x10\x03'
EXT_DIAG_RESPONSE = b'\x50\x03'

# File-based logging for debugging
ECU_LOG_FILE = "/data/ecu_disable.log"

def ecu_log(msg):
  """Write to both cloudlog and a dedicated log file for debugging."""
  timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
  log_line = f"[{timestamp}] {msg}"
  cloudlog.warning(msg)
  try:
    with open(ECU_LOG_FILE, "a") as f:
      f.write(log_line + "\n")
  except Exception:
    pass


def disable_ecu(logcan, sendcan, bus=0, addr=0x7d0, sub_addr=None, com_cont_req=b'\x28\x83\x01', timeout=0.1, retry=10, debug=False):
  """Silence an ECU by disabling sending and receiving messages using UDS 0x28.
  The ECU will stay silent as long as openpilot keeps sending Tester Present.

  This is used to disable the radar in some cars. Openpilot will emulate the radar.
  WARNING: THIS DISABLES AEB!"""
  ecu_log(f"=== ECU DISABLE START === addr={hex(addr)}, bus={bus}")

  # Try multiple times with different approaches
  for i in range(retry):
    try:
      # Enter extended diagnostic session
      ecu_log(f"attempt {i+1}/{retry}: diag session...")
      query = IsoTpParallelQuery(sendcan, logcan, bus, [(addr, sub_addr)], [EXT_DIAG_REQUEST], [EXT_DIAG_RESPONSE], debug=debug)

      for _, _ in query.get_data(timeout).items():
        ecu_log("diag session OK")

        # Small delay to let ECU fully enter diagnostic mode
        time.sleep(0.05)

        # Send CC command and log the response
        ecu_log("sending CC...")
        cc_query = IsoTpParallelQuery(sendcan, logcan, bus, [(addr, sub_addr)], [com_cont_req], [b''], debug=debug)
        cc_response = cc_query.get_data(timeout)

        # Log what we got back
        cc_success = False
        cc_rejected = False
        for (rx_addr, _), data in cc_response.items():
          ecu_log(f"CC response: {data.hex() if data else 'empty'}")
          # Check for positive response (0x68 = 0x28 + 0x40)
          if len(data) >= 1 and data[0] == 0x68:
            ecu_log("=== ECU DISABLE CONFIRMED ===")
            cc_success = True
          # Check for negative response
          elif len(data) >= 3 and data[0] == 0x7F:
            nrc = data[2]
            nrc_meanings = {
              0x12: "subFunctionNotSupported",
              0x13: "incorrectMessageLengthOrInvalidFormat",
              0x22: "conditionsNotCorrect (car must be in IGN-ON, not READY)",
              0x31: "requestOutOfRange",
              0x33: "securityAccessDenied",
            }
            nrc_name = nrc_meanings.get(nrc, "unknown")
            ecu_log(f"CC rejected: NRC 0x{nrc:02x} = {nrc_name}")
            cc_rejected = True

        if cc_success:
          return True
        elif cc_rejected:
          # ECU explicitly rejected - don't retry, it won't work
          ecu_log("=== ECU DISABLE REJECTED ===")
          return False
        else:
          # No response - consider it sent (ECU might have stopped responding)
          ecu_log("=== ECU DISABLE SENT (no response) ===")
          return True

    except Exception as e:
      ecu_log(f"attempt {i+1} exception: {e}")

    time.sleep(0.1)

  ecu_log("=== ECU DISABLE FAILED ===")
  return False


if __name__ == "__main__":
  import time
  import cereal.messaging as messaging
  sendcan = messaging.pub_sock('sendcan')
  logcan = messaging.sub_sock('can')
  time.sleep(1)

  # honda bosch radar disable
  disabled = disable_ecu(logcan, sendcan, bus=1, addr=0x18DAB0F1, com_cont_req=b'\x28\x83\x03', timeout=0.5, debug=False)
  print(f"disabled: {disabled}")
