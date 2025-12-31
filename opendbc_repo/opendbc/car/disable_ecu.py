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


def compute_security_key(seed: bytes, algorithm: int = 0) -> bytes:
  """Compute the security key from the seed.
  For Hyundai HDA2 ADAS ECUs, there are several known algorithms.
  Returns the computed key based on the algorithm parameter."""
  if len(seed) == 0:
    return b''

  if algorithm == 0:
    # Algorithm 0: XOR with 0x4A (common Hyundai algorithm)
    key = bytes([b ^ 0x4A for b in seed])
  elif algorithm == 1:
    # Algorithm 1: Bitwise NOT (invert all bits)
    key = bytes([~b & 0xFF for b in seed])
  elif algorithm == 2:
    # Algorithm 2: XOR with 0xCA
    key = bytes([b ^ 0xCA for b in seed])
  elif algorithm == 3:
    # Algorithm 3: Swap bytes and XOR with 0x4A
    swapped = bytes(reversed(seed))
    key = bytes([b ^ 0x4A for b in swapped])
  elif algorithm == 4:
    # Algorithm 4: Add 1 to each byte (modulo 256)
    key = bytes([(b + 1) & 0xFF for b in seed])
  elif algorithm == 5:
    # Algorithm 5: Subtract 1 from each byte (modulo 256)
    key = bytes([(b - 1) & 0xFF for b in seed])
  elif algorithm == 6:
    # Algorithm 6: XOR with fixed pattern 0xDE, 0xAD, 0xBE, 0xEF repeating
    pattern = [0xDE, 0xAD, 0xBE, 0xEF]
    key = bytes([seed[i] ^ pattern[i % 4] for i in range(len(seed))])
  elif algorithm == 7:
    # Algorithm 7: XOR first 4 bytes with last 4 bytes
    if len(seed) == 8:
      key = bytes([seed[i] ^ seed[i + 4] for i in range(4)] + list(seed[4:]))
    else:
      key = bytes(seed)
  elif algorithm == 8:
    # Algorithm 8: Common Hyundai ADAS - XOR with alternating 0x71, 0x7B
    pattern = [0x71, 0x7B]
    key = bytes([seed[i] ^ pattern[i % 2] for i in range(len(seed))])
  else:
    # Default: identity
    key = bytes(seed)

  return key


def perform_security_access(can_send, can_recv, bus, addr, sub_addr, timeout=0.2, security_level=0x01):
  """Perform full SecurityAccess handshake (seed request + key send).
  Returns True if security access was granted. Tries multiple key algorithms."""
  try:
    # Step 1: Request seed for the specified security level
    seed_request = bytes([0x27, security_level])
    seed_response_expected = bytes([0x67, security_level])
    key_send_sublevel = security_level + 1  # Key send is always odd level + 1

    ecu_log(f"security access: requesting seed (0x27 0x{security_level:02x})...")

    # Use empty expected response to capture ANY response
    seed_query = IsoTpParallelQuery(can_send, can_recv, bus, [(addr, sub_addr)],
                                     [seed_request], [b''])
    seed_response = seed_query.get_data(timeout)

    for (rx_addr, _), data in seed_response.items():
      ecu_log(f"security access: ECU responded with {len(data)} bytes: {data.hex()}")

      # Check for negative response
      if len(data) >= 3 and data[0] == 0x7F and data[1] == 0x27:
        nrc = data[2]
        nrc_meanings = {
          0x12: "subFunctionNotSupported",
          0x13: "incorrectMessageLengthOrInvalidFormat",
          0x22: "conditionsNotCorrect",
          0x24: "requestSequenceError",
          0x35: "invalidKey",
          0x36: "exceededNumberOfAttempts",
          0x37: "requiredTimeDelayNotExpired",
        }
        nrc_name = nrc_meanings.get(nrc, "unknown")
        ecu_log(f"security access: seed request rejected (NRC: 0x{nrc:02x} = {nrc_name})")
        return False

      if len(data) >= 2 and data[0:2] == seed_response_expected:
        seed = data[2:]  # Extract seed bytes after 0x67 0xXX
        ecu_log(f"security access: received seed ({len(seed)} bytes): {seed.hex()}")

        if len(seed) == 0:
          # Zero-length seed means security is already unlocked
          ecu_log("security access: already unlocked (zero-length seed)")
          return True

        # Step 2: Try different key algorithms
        algorithm_names = {
          0: "XOR 0x4A",
          1: "bitwise NOT",
          2: "XOR 0xCA",
          3: "swap+XOR 0x4A",
          4: "add 1",
          5: "subtract 1",
          6: "XOR DEADBEEF",
          7: "XOR halves",
          8: "XOR 71/7B",
        }

        for algo in range(9):
          key = compute_security_key(seed, algorithm=algo)
          key_request = bytes([0x27, key_send_sublevel]) + key
          ecu_log(f"trying algorithm {algo} ({algorithm_names.get(algo, '?')}): key={key.hex()}")

          # Need to re-enter extended diag session before each key attempt
          # because some ECUs reset the session after a failed key
          try:
            key_query = IsoTpParallelQuery(can_send, can_recv, bus, [(addr, sub_addr)],
                                            [key_request], [b''])
            key_response = key_query.get_data(timeout)

            for (rx_addr2, _), data2 in key_response.items():
              ecu_log(f"key response: {data2.hex()}")

              if len(data2) >= 2 and data2[0] == 0x67 and data2[1] == key_send_sublevel:
                ecu_log(f"security access: GRANTED with algorithm {algo} ({algorithm_names.get(algo, '?')})")
                return True
              elif len(data2) >= 3 and data2[0] == 0x7F:
                nrc = data2[2]
                if nrc == 0x35:
                  ecu_log(f"algorithm {algo}: invalidKey, trying next...")
                  # Re-request seed for next attempt
                  time.sleep(0.05)
                  seed_query2 = IsoTpParallelQuery(can_send, can_recv, bus, [(addr, sub_addr)],
                                                   [seed_request], [b''])
                  seed_response2 = seed_query2.get_data(timeout)
                  for (_, _), data3 in seed_response2.items():
                    if len(data3) >= 2 and data3[0:2] == seed_response_expected:
                      seed = data3[2:]
                      ecu_log(f"new seed: {seed.hex()}")
                      break
                elif nrc == 0x36:
                  ecu_log("exceeded attempts, need to wait or power cycle")
                  return False
                elif nrc == 0x37:
                  ecu_log("time delay required, waiting 10s...")
                  time.sleep(10)
                else:
                  ecu_log(f"algorithm {algo}: NRC 0x{nrc:02x}")
          except Exception as e:
            ecu_log(f"algorithm {algo} exception: {e}")
            continue

        ecu_log("security access: all algorithms failed")
        return False

    ecu_log("security access: no response from ECU")
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
    retry = max(retry, 10)  # Fewer retries to avoid overwhelming ECU
    ecu_log(f"HDA2 mode: using {retry} retries")
    # Moderate initial delay
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

        # For HDA2 cars, skip SecurityAccess and rely on timing/state
        # The ECU accepts CommunicationControl in certain states without security
        if security_access:
          ecu_log("HDA2 mode: skipping SecurityAccess, relying on ECU state/timing")

        # Send communication control command and check response
        ecu_log(f"communication control: sending {com_cont_req.hex()}...")
        query = IsoTpParallelQuery(can_send, can_recv, bus, [(addr, sub_addr)], [com_cont_req], [b''])
        com_response = query.get_data(0.2)

        got_response = False
        for (rx_addr, _), data in com_response.items():
          got_response = True
          ecu_log(f"communication control response: {data.hex()}")
          # Check for positive response (0x68 = positive response to 0x28)
          if len(data) >= 1 and data[0] == 0x68:
            ecu_log("=== ECU DISABLED SUCCESSFULLY (positive response) ===")
            return True
          elif len(data) >= 3 and data[0] == 0x7F and data[1] == 0x28:
            nrc = data[2]
            nrc_meanings = {
              0x12: "subFunctionNotSupported",
              0x22: "conditionsNotCorrect (security access required)",
              0x31: "requestOutOfRange",
            }
            nrc_name = nrc_meanings.get(nrc, "unknown")
            ecu_log(f"communication control rejected (NRC: 0x{nrc:02x} = {nrc_name})")
            # Continue retrying

        # Some ECUs silently accept the command without responding
        # Original approach: return on first "no response" - verification may interfere
        if not got_response:
          ecu_log("communication control: no response (command sent)")
          ecu_log("=== ECU DISABLE SENT (no verification) ===")
          return True
        else:
          ecu_log("communication control: got negative response, retrying...")

    except Exception as e:
      ecu_log(f"attempt {i+1}/{retry} exception: {e}")

    # Delay between retries - spread attempts over longer period
    retry_delay = 0.5 if security_access else 0.05
    ecu_log(f"waiting {retry_delay}s before next attempt...")
    time.sleep(retry_delay)

  ecu_log("=== ECU DISABLE FAILED after all retries ===")
  return False
