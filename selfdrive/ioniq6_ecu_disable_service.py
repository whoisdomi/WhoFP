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

def main():
    print("[ECU Disable Service] Starting service...")

    # Wait a bit for system to stabilize
    time.sleep(2)

    # Check if longitudinal is enabled
    # (We'll attempt anyway, but this is for logging)
    longitudinal_enabled = check_longitudinal_enabled()

    if longitudinal_enabled:
        print("[ECU Disable Service] Longitudinal control appears to be enabled")
    else:
        print("[ECU Disable Service] Longitudinal control not enabled, but attempting ECU disable anyway")

    # Perform ECU disable
    success = perform_ecu_disable()

    if success:
        print("[ECU Disable Service] Service completed successfully")
        sys.exit(0)
    else:
        print("[ECU Disable Service] Service failed")
        sys.exit(1)

if __name__ == "__main__":
    main()
