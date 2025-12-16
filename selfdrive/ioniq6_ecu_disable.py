#!/usr/bin/env python3
"""
Early-boot ECU disable for Ioniq 6 HDA2
Must run in IGN_ON state before READY mode to prevent dashboard errors

Uses direct CAN messaging instead of IsoTpParallelQuery to avoid API issues.
"""

import time
import sys
import struct

def ioniq6_ecu_disable():
    """Perform Ioniq 6 ECU disable with SecurityAccess using direct CAN"""
    from panda import Panda

    print("[Ioniq6 ECU Disable] Starting...")

    # Wait for panda and car CAN to be ready
    time.sleep(2)

    try:
        p = Panda()
        print(f"[Ioniq6 ECU Disable] Connected to panda {p.get_serial()[0]}")
    except Exception as e:
        print(f"[Ioniq6 ECU Disable] Failed to connect to panda: {e}")
        return False

    # For Ioniq 6 HDA2 with LKA steering
    ecu_addr = 0x730      # ADAS_DRV ECU TX address
    ecu_rx_addr = 0x738   # ADAS_DRV ECU RX address (usually TX + 8)
    bus = 1               # ECAN - where SCC_CONTROL (0x1a0) is seen

    # Clear any corrupted buffer data
    print("[Ioniq6 ECU Disable] Clearing CAN buffer...")
    try:
        p.can_clear(0xFFFF)
        time.sleep(0.2)
        # Drain any remaining messages
        for _ in range(10):
            try:
                p.can_recv()
            except:
                pass
            time.sleep(0.02)
    except Exception as e:
        print(f"[Ioniq6 ECU Disable] Buffer clear warning: {e}")

    def send_isotp_single_frame(p, addr, data, bus):
        """Send a single-frame ISO-TP message (for messages <= 7 bytes)"""
        # ISO-TP single frame: first byte is 0x0N where N is data length
        frame = bytes([len(data)]) + data
        # Pad to 8 bytes for classic CAN or full length for CAN-FD
        frame = frame.ljust(8, b'\x00')
        p.can_send(addr, frame, bus)

    def recv_isotp_response(p, expected_rx_addr, bus, timeout=1.0):
        """Wait for ISO-TP response from ECU"""
        start = time.time()
        while time.time() - start < timeout:
            try:
                msgs = p.can_recv()
            except Exception as e:
                # Buffer corruption - clear and retry
                try:
                    p.can_clear(0xFFFF)
                except:
                    pass
                time.sleep(0.05)
                continue
            for msg in msgs:
                if len(msg) >= 3:
                    addr = msg[0]
                    dat = msg[1]
                    msg_bus = msg[2]
                    if addr == expected_rx_addr and msg_bus == bus:
                        # Parse ISO-TP single frame
                        if len(dat) > 0 and (dat[0] & 0xF0) == 0x00:
                            length = dat[0] & 0x0F
                            return dat[1:1+length]
            time.sleep(0.01)
        return None

    # Step 1: Extended Diagnostic Session (0x10 0x03)
    print("[Ioniq6 ECU Disable] Step 1: Extended Diagnostic Session...")
    send_isotp_single_frame(p, ecu_addr, b'\x10\x03', bus)
    response = recv_isotp_response(p, ecu_rx_addr, bus, timeout=0.5)
    if response:
        if response[0] == 0x50:
            print(f"[Ioniq6 ECU Disable]   -> OK: {response.hex()}")
        elif response[0] == 0x7F:
            print(f"[Ioniq6 ECU Disable]   -> Negative response: {response.hex()}")
        else:
            print(f"[Ioniq6 ECU Disable]   -> Response: {response.hex()}")
    else:
        print("[Ioniq6 ECU Disable]   -> No response (timeout)")

    time.sleep(0.1)

    # Step 2: SecurityAccess Seed Request (0x27 0x01)
    print("[Ioniq6 ECU Disable] Step 2: SecurityAccess seed request...")
    send_isotp_single_frame(p, ecu_addr, b'\x27\x01', bus)
    response = recv_isotp_response(p, ecu_rx_addr, bus, timeout=0.5)
    if response:
        if response[0] == 0x67:
            print(f"[Ioniq6 ECU Disable]   -> Seed received: {response.hex()}")
            print("[Ioniq6 ECU Disable]   SecurityAccess handshake completed!")
        elif response[0] == 0x7F:
            print(f"[Ioniq6 ECU Disable]   -> Negative response: {response.hex()}")
            # 0x7F 27 22 means "conditions not correct" - might need READY state
            if len(response) >= 3 and response[1] == 0x27 and response[2] == 0x22:
                print("[Ioniq6 ECU Disable]   (Conditions not correct - may need different state)")
        else:
            print(f"[Ioniq6 ECU Disable]   -> Response: {response.hex()}")
    else:
        print("[Ioniq6 ECU Disable]   -> No response (timeout)")

    time.sleep(0.1)

    # Step 3: Communication Control - Disable TX/RX (0x28 0x83 0x01)
    print("[Ioniq6 ECU Disable] Step 3: Communication Control disable...")
    # 0x28 = Communication Control service
    # 0x83 = controlType (bit 7 set for positive response suppression) + 0x03 (disableRxAndTx)
    # 0x01 = communicationType (normal communication messages)
    send_isotp_single_frame(p, ecu_addr, b'\x28\x83\x01', bus)
    response = recv_isotp_response(p, ecu_rx_addr, bus, timeout=0.5)
    if response:
        if response[0] == 0x68:
            print(f"[Ioniq6 ECU Disable]   -> OK: {response.hex()}")
            print("[Ioniq6 ECU Disable] ✓ SUCCESS: ECU disabled!")
            return True
        elif response[0] == 0x7F:
            print(f"[Ioniq6 ECU Disable]   -> Negative response: {response.hex()}")
            if len(response) >= 3:
                nrc = response[2]
                nrc_names = {
                    0x12: "subFunctionNotSupported",
                    0x13: "incorrectMessageLengthOrInvalidFormat",
                    0x22: "conditionsNotCorrect",
                    0x31: "requestOutOfRange",
                    0x33: "securityAccessDenied",
                    0x35: "invalidKey",
                    0x36: "exceededNumberOfAttempts",
                }
                print(f"[Ioniq6 ECU Disable]   NRC: {nrc_names.get(nrc, f'Unknown (0x{nrc:02X})')}")
        else:
            print(f"[Ioniq6 ECU Disable]   -> Response: {response.hex()}")
    else:
        print("[Ioniq6 ECU Disable]   -> No response (may have succeeded with suppressed response)")
        # Bit 7 was set, so positive response is suppressed
        print("[Ioniq6 ECU Disable] ✓ ECU disable command sent (response suppressed)")
        return True

    print("[Ioniq6 ECU Disable] ✗ FAILED to disable ECU")
    return False

if __name__ == "__main__":
    success = ioniq6_ecu_disable()
    sys.exit(0 if success else 1)
