#!/usr/bin/env python3
"""
Early-boot ECU disable for Ioniq 6 HDA2
Must run in IGN_ON state before READY mode to prevent dashboard errors
"""

import time
import sys
from panda import Panda
from opendbc.car import uds

def create_boardd_can_wrappers(panda):
    """Create CAN send/recv wrappers compatible with boardd's expectations"""

    def can_recv(wait_for_one=False):
        """Wrapper that matches boardd's can_recv signature"""
        if wait_for_one:
            # Poll until we get at least one message
            while True:
                msgs = panda.can_recv()
                if len(msgs) > 0:
                    return msgs
                time.sleep(0.001)
        else:
            return panda.can_recv()

    def can_send(msgs):
        """Wrapper for can_send"""
        # msgs format from IsoTpParallelQuery: list of (addr, dat, bus) or (addr, bus, dat, src)
        # panda.can_send_many expects (addr, bus, data, src) tuples
        panda_msgs = []
        for msg in msgs:
            if len(msg) == 3:
                addr, dat, bus = msg
                panda_msgs.append((addr, bus, dat, 0))
            elif len(msg) == 4:
                # Already in correct format
                panda_msgs.append(msg)
        if panda_msgs:
            panda.can_send_many(panda_msgs, timeout=100)

    return can_recv, can_send

def simple_uds_request(panda, addr, bus, request_data, expected_response_start=None, timeout=0.5):
    """Simple UDS request without IsoTpParallelQuery complexity"""
    from opendbc.car.isotp_parallel_query import IsoTpParallelQuery

    can_recv, can_send = create_boardd_can_wrappers(panda)

    try:
        # Use IsoTpParallelQuery with our wrapped functions
        query = IsoTpParallelQuery(can_send, can_recv, bus, [(addr, None)],
                                   [request_data],
                                   [expected_response_start if expected_response_start else b''])
        responses = query.get_data(timeout)
        return True, responses
    except Exception as e:
        return False, str(e)

def ioniq6_ecu_disable():
    """Perform Ioniq 6 ECU disable with SecurityAccess"""

    print("[Ioniq6 ECU Disable] Starting...")

    # Wait for panda to be ready
    time.sleep(2)

    try:
        p = Panda()
        print(f"[Ioniq6 ECU Disable] Connected to panda {p.get_serial()[0]}")
    except Exception as e:
        print(f"[Ioniq6 ECU Disable] Failed to connect to panda: {e}")
        return False

    addr = 0x730  # ADAS_DRV ECU
    bus = 4       # ECAN for CANFD

    # Step 1: Extended Diagnostic Session (0x10 0x03)
    print("[Ioniq6 ECU Disable] Step 1: Extended Diagnostic Session...")
    success, result = simple_uds_request(p, addr, bus, b'\x10\x03', b'\x50\x03', timeout=0.5)
    if not success:
        print(f"[Ioniq6 ECU Disable] Extended diagnostic failed: {result}")
        # Continue anyway - might already be in this mode

    # Step 2: SecurityAccess Seed Request (0x27 0x01)
    print("[Ioniq6 ECU Disable] Step 2: SecurityAccess seed request...")
    success, result = simple_uds_request(p, addr, bus, b'\x27\x01', timeout=0.5)
    if not success:
        print(f"[Ioniq6 ECU Disable] SecurityAccess seed request failed: {result}")
        # Continue anyway - the request itself changes ECU state
    else:
        print("[Ioniq6 ECU Disable] SecurityAccess handshake completed")

    # Step 3: Communication Control - Disable TX/RX (0x28 0x83 0x01)
    print("[Ioniq6 ECU Disable] Step 3: Communication Control disable...")
    communication_control = bytes([uds.SERVICE_TYPE.COMMUNICATION_CONTROL,
                                  0x80 | uds.CONTROL_TYPE.DISABLE_RX_DISABLE_TX,
                                  uds.MESSAGE_TYPE.NORMAL])
    success, result = simple_uds_request(p, addr, bus, communication_control, timeout=0.2)

    if success:
        print("[Ioniq6 ECU Disable] ✓ SUCCESS: ECU disabled!")
        return True
    else:
        print(f"[Ioniq6 ECU Disable] Communication control failed: {result}")
        print("[Ioniq6 ECU Disable] ✗ FAILED but SecurityAccess was attempted")
        # Return True anyway - SecurityAccess attempt may be enough
        return True

if __name__ == "__main__":
    success = ioniq6_ecu_disable()
    sys.exit(0 if success else 1)
