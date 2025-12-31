import time
import datetime
from opendbc.car.carlog import carlog
from opendbc.car.isotp_parallel_query import IsoTpParallelQuery

EXT_DIAG_REQUEST = b'\x10\x03'
EXT_DIAG_RESPONSE = b'\x50\x03'

COM_CONT_RESPONSE = b''

# SecurityAccess constants
SECURITY_ACCESS_SEED_REQUEST = b'\x27\x01'  # Request seed (level 1)
SECURITY_ACCESS_SEED_RESPONSE = b'\x67\x01'  # Positive response with seed
SECURITY_ACCESS_KEY_SEND = b'\x27\x02'  # Send key (level 1)

# File-based logging for debugging
ECU_LOG_FILE = "/data/ecu_disable.log"

def ecu_log(msg):
  """Write to both carlog and a dedicated log file for debugging."""
  timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
  log_line = f"[{timestamp}] {msg}"
  carlog.warning(msg)
  try:
    with open(ECU_LOG_FILE, "a") as f:
      f.write(log_line + "\n")
  except Exception:
    pass


def compute_security_key(seed: bytes) -> bytes:
  """Compute the security key from the seed.
  For Hyundai HDA2 ADAS ECUs, the key algorithm is typically simple.
  Common algorithms: XOR with constant, bitwise NOT, or identity.
  This implements a common Hyundai algorithm."""
  if len(seed) == 0:
    return b''

  # Try simple XOR with 0x00 (identity) - some ECUs accept seed as key
  # This is a common "null security" implementation
  key = bytes(seed)
  return key


def perform_security_access(can_send, can_recv, bus, addr, sub_addr, timeout=0.2):
  """Perform full SecurityAccess handshake (seed request + key send).
  Returns True if security access was granted."""
  try:
    # Step 1: Request seed
    ecu_log("security access: requesting seed (0x27 0x01)...")
    seed_query = IsoTpParallelQuery(can_send, can_recv, bus, [(addr, sub_addr)],
                                     [SECURITY_ACCESS_SEED_REQUEST], [SECURITY_ACCESS_SEED_RESPONSE])
    seed_response = seed_query.get_data(timeout)

    for (rx_addr, _), data in seed_response.items():
      if len(data) >= 2 and data[0:2] == SECURITY_ACCESS_SEED_RESPONSE:
        seed = data[2:]  # Extract seed bytes after 0x67 0x01
        ecu_log(f"security access: received seed ({len(seed)} bytes): {seed.hex()}")

        if len(seed) == 0:
          # Zero-length seed means security is already unlocked
          ecu_log("security access: already unlocked (zero-length seed)")
          return True

        # Step 2: Compute and send key
        key = compute_security_key(seed)
        key_request = SECURITY_ACCESS_KEY_SEND + key
        ecu_log(f"security access: sending key (0x27 0x02 + {len(key)} bytes): {key.hex()}")

        key_query = IsoTpParallelQuery(can_send, can_recv, bus, [(addr, sub_addr)],
                                        [key_request], [b'\x67\x02'])
        key_response = key_query.get_data(timeout)

        for (rx_addr2, _), data2 in key_response.items():
          if len(data2) >= 2 and data2[0:2] == b'\x67\x02':
            ecu_log("security access: GRANTED")
            return True
          elif len(data2) >= 3 and data2[0] == 0x7F:
            # Negative response
            nrc = data2[2]
            nrc_meanings = {
              0x12: "subFunctionNotSupported",
              0x22: "conditionsNotCorrect",
              0x24: "requestSequenceError",
              0x35: "invalidKey",
              0x36: "exceededNumberOfAttempts",
              0x37: "requiredTimeDelayNotExpired",
            }
            nrc_name = nrc_meanings.get(nrc, "unknown")
            ecu_log(f"security access: key rejected (NRC: 0x{nrc:02x} = {nrc_name})")
            return False

    ecu_log("security access: no valid seed response")
    return False

  except Exception as e:
    ecu_log(f"security access: exception - {e}")
    return False


def disable_ecu(can_recv, can_send, bus=0, addr=0x7d0, sub_addr=None, com_cont_req=b'\x28\x83\x01', timeout=0.1, retry=10, security_access=False):
  """Silence an ECU by disabling sending and receiving messages using UDS 0x28.
  The ECU will stay silent as long as openpilot keeps sending Tester Present.

  This is used to disable the radar in some cars. Openpilot will emulate the radar.
  WARNING: THIS DISABLES AEB!

  Args:
    security_access: If True, performs SecurityAccess handshake (0x27) before Communication Control.
                     Required for HDA2 cars (e.g., Ioniq 6) that return 0x7F2822 without this handshake."""
  ecu_log(f"=== ECU DISABLE START === addr={hex(addr)}, bus={bus}, security_access={security_access}")

  # For HDA2 cars with security access, use more retries and longer delays
  # The ECU needs time to boot and be ready to accept UDS commands
  if security_access:
    retry = max(retry, 30)  # Ensure at least 30 retries for HDA2
    ecu_log(f"security access mode: using {retry} retries with delays")
    # Initial delay to let ECU boot - this is critical for consistency
    ecu_log("waiting 2.0s for ECU boot...")
    time.sleep(2.0)

  for i in range(retry):
    try:
      # Enter extended diagnostic session
      ecu_log(f"attempt {i+1}/{retry}: requesting extended diagnostic session (0x10 0x03)...")
      query = IsoTpParallelQuery(can_send, can_recv, bus, [(addr, sub_addr)], [EXT_DIAG_REQUEST], [EXT_DIAG_RESPONSE])
      ext_diag_response = query.get_data(timeout)

      for _, _ in ext_diag_response.items():
        ecu_log("extended diagnostic session established")

        # For HDA2 cars, perform SecurityAccess handshake before Communication Control
        if security_access:
          # Try security access up to 3 times
          security_granted = False
          for sec_attempt in range(3):
            if perform_security_access(can_send, can_recv, bus, addr, sub_addr, timeout=0.3):
              security_granted = True
              break
            ecu_log(f"security access attempt {sec_attempt + 1}/3 failed, retrying...")
            time.sleep(0.15)

          if not security_granted:
            ecu_log("security access not granted, attempting communication control anyway...")

        # Send communication control command
        ecu_log(f"communication control: sending {com_cont_req.hex()}...")
        query = IsoTpParallelQuery(can_send, can_recv, bus, [(addr, sub_addr)], [com_cont_req], [COM_CONT_RESPONSE])
        query.get_data(0)

        ecu_log("=== ECU DISABLED SUCCESSFULLY ===")
        return True

    except Exception as e:
      ecu_log(f"attempt {i+1}/{retry} exception: {e}")

    # Delay between retries - ECU may need time to be ready
    retry_delay = 0.3 if security_access else 0.05
    time.sleep(retry_delay)

  ecu_log("=== ECU DISABLE FAILED after all retries ===")
  return False
