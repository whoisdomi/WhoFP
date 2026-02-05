#!/usr/bin/env python3
"""
Ioniq 6 ECU Disable Service
Runs at boot to disable the ADAS_DRV ECU before openpilot takes control.
Must run in IGN_ON mode (accessory mode, park gear) before READY state.
"""

import time
import os
import sys

# Add openpilot to path
sys.path.insert(0, '/data/openpilot')

def wait_for_ign_on():
    """Wait for car to enter IGN_ON state (accessories on)"""
    from panda import Panda

    print("[ECU Disable Service] Waiting for panda connection...")
    max_wait = 60  # Wait up to 60 seconds
    start_time = time.time()

    while time.time() - start_time < max_wait:
        try:
            panda = Panda()
            print(f"[ECU Disable Service] Connected to panda")
            return panda
        except Exception as e:
            time.sleep(1)

    raise TimeoutError("Could not connect to panda within 60 seconds")

def check_longitudinal_enabled():
    """Check if openpilot longitudinal control is enabled"""
    from common.params import Params
    params = Params()

    # Check if FrogPilot has longitudinal enabled
    # This might be stored in different param depending on FrogPilot version
    try:
        # Try to read the experimental mode toggle
        experimental_mode = params.get_bool("ExperimentalMode")
        print(f"[ECU Disable Service] ExperimentalMode: {experimental_mode}")
        return experimental_mode
    except Exception:
        # If we can't read params, assume we should try ECU disable
        # The worst that happens is we disable ECU when not needed
        print("[ECU Disable Service] Could not read params, proceeding with ECU disable")
        return True

def perform_ecu_disable():
    """Perform ECU disable with SecurityAccess for Ioniq 6"""
    from opendbc.car.disable_ecu import disable_ecu
    from panda import Panda

    print("[ECU Disable Service] Starting ECU disable sequence...")

    try:
        # Connect to panda
        panda = wait_for_ign_on()

        # Set permissive safety mode for diagnostics
        panda.set_safety_mode(0)
        time.sleep(0.1)

        # Clear buffers
        panda.can_clear(0xFFFF)
        time.sleep(0.2)

        # For Ioniq 6 with CANFD_LKA_STEERING:
        # Target ECU address 0x730 on bus 1 (ECAN)
        addr = 0x730
        bus = 1

        # Create CAN send/recv functions
        def can_send(msgs):
            for addr, dat, bus in msgs:
                panda.can_send(addr, dat, bus)

        def can_recv(wait_for_one=False):
            return panda.can_recv()

        print(f"[ECU Disable Service] Disabling ECU at addr=0x{addr:x}, bus={bus}")

        # Communication control command to disable TX/RX
        com_cont = bytes([0x28, 0x83, 0x01])

        # Perform ECU disable with SecurityAccess enabled
        result = disable_ecu(
            can_recv=can_recv,
            can_send=can_send,
            bus=bus,
            addr=addr,
            com_cont_req=com_cont,
            timeout=0.5,
            retry=5,
            security_access=True
        )

        if result:
            print("[ECU Disable Service] ✓ ECU disable successful!")
            return True
        else:
            print("[ECU Disable Service] ✗ ECU disable failed")
            return False

    except Exception as e:
        print(f"[ECU Disable Service] ✗ Exception during ECU disable: {e}")
        import traceback
        traceback.print_exc()
        return False

def detect_car_state(panda, timeout=2.0):
    """
    Detect car state: 'ign_on' (safe to disable ECU) or 'ready' (too late).

    For Ioniq 6 (CAN-FD EV), we detect by checking for messages that ONLY
    appear when the car is in READY mode (after brake+start).

    READY-only messages (not present in ACC or IGN-ON):
    - 0x090, 0x255, 0x2e5, 0x3a0, 0x3b0, 0x3b1, 0x3b5, 0x3f0, 0x3f5

    Returns: 'ign_on' or 'ready'
    """
    # Messages that ONLY appear in READY mode (verified from CAN captures)
    READY_ONLY_MSGS = {0x090, 0x255, 0x2e5, 0x3a0, 0x3b0, 0x3b1, 0x3b5, 0x3f0, 0x3f5}

    msgs_received = set()
    ready_msgs_seen = set()

    print(f"[ECU Disable Service] Detecting car state (reading CAN for {timeout}s)...")

    start_time = time.monotonic()
    while time.monotonic() - start_time < timeout:
        try:
            can_msgs = panda.can_recv()
            for msg in can_msgs:
                if len(msg) >= 4:
                    addr = msg[0]
                    bus = msg[3]

                    if bus == 1:  # ECAN
                        msgs_received.add(addr)
                        if addr in READY_ONLY_MSGS:
                            ready_msgs_seen.add(addr)

        except Exception as e:
            print(f"[ECU Disable Service] CAN recv error: {e}")

        time.sleep(0.01)

    print(f"[ECU Disable Service] Messages seen on ECAN: {len(msgs_received)} total")
    print(f"[ECU Disable Service] READY-only messages seen: {[hex(m) for m in sorted(ready_msgs_seen)]}")

    # If ANY READY-only message is present, car is in READY mode
    if ready_msgs_seen:
        print(f"[ECU Disable Service] Detected READY mode (found {len(ready_msgs_seen)} READY-only messages)")
        return 'ready'
    else:
        # No READY-only messages = ACC or IGN-ON mode (safe to run ECU disable)
        print("[ECU Disable Service] No READY-only messages found - safe to run ECU disable")
        return 'ign_on'


def main():
    from openpilot.common.params import Params
    params = Params()

    print("[ECU Disable Service] Starting service...")

    # Wait a bit for system to stabilize
    time.sleep(2)

    # Connect to panda first
    try:
        panda = wait_for_ign_on()
    except TimeoutError as e:
        print(f"[ECU Disable Service] Failed to connect to panda: {e}")
        sys.exit(1)

    # Set permissive safety mode for CAN reading
    panda.set_safety_mode(0)
    time.sleep(0.1)
    panda.can_clear(0xFFFF)
    time.sleep(0.2)

    # Detect car state BEFORE attempting ECU disable
    car_state = detect_car_state(panda)
    print(f"[ECU Disable Service] Detected car state: {car_state.upper()}")

    if car_state in ('ready', 'driving'):
        # Car already past IGN-ON - skip ECU disable, disable longitudinal
        print(f"[ECU Disable Service] Car in {car_state.upper()} mode - skipping ECU disable")
        params.put_bool("SkipEcuDisable", True)
        # Note: Disabling longitudinal automatically disables experimental mode
        print("[ECU Disable Service] Longitudinal will be disabled (experimental auto-disabled)")
        print("[ECU Disable Service] Stock ACC will be used, lateral control still active")
        sys.exit(0)

    # Car in IGN-ON (or ACC) mode - proceed with ECU disable
    print("[ECU Disable Service] Car in IGN-ON mode - proceeding with ECU disable")
    params.put_bool("SkipEcuDisable", False)

    # Perform ECU disable
    success = perform_ecu_disable()

    if success:
        print("[ECU Disable Service] ECU disable successful")
        # Enable BOTH longitudinal AND experimental
        params.put_bool("ExperimentalMode", True)
        print("[ECU Disable Service] Longitudinal + Experimental mode ENABLED")
        sys.exit(0)
    else:
        print("[ECU Disable Service] ECU disable FAILED")
        params.put_bool("SkipEcuDisable", True)
        # Note: Disabling longitudinal automatically disables experimental mode
        print("[ECU Disable Service] Longitudinal DISABLED due to failure (experimental auto-disabled)")
        sys.exit(1)

if __name__ == "__main__":
    main()
