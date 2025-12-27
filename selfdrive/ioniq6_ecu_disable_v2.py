#!/usr/bin/env python3
"""
Ioniq 6 ECU disable using openpilot's existing infrastructure
This leverages the IsoTpParallelQuery that handles CAN-FD properly
"""

import time
import sys

def ioniq6_ecu_disable():
    """Use openpilot's disable_ecu with SecurityAccess"""
    print("[Ioniq6 ECU Disable V2] Starting...")

    # Import openpilot's CAN infrastructure
    from opendbc.car.disable_ecu import disable_ecu
    from panda import Panda

    try:
        # Connect to panda
        panda = Panda()
        print(f"[Ioniq6 ECU Disable V2] Connected to panda")

        # Set permissive safety mode for diagnostics
        panda.set_safety_mode(0)
        time.sleep(0.1)

        # Clear buffers
        panda.can_clear(0xFFFF)
        time.sleep(0.2)

        # For Ioniq 6 with CANFD_LKA_STEERING:
        addr = 0x730
        bus = 1  # ECAN for LKA steering cars

        # Create CAN send/recv functions compatible with IsoTpParallelQuery
        def can_send(msgs):
            for addr, dat, bus in msgs:
                panda.can_send(addr, dat, bus)

        def can_recv(wait_for_one=False):
            # IsoTpParallelQuery expects wait_for_one parameter
            return panda.can_recv()

        print(f"[Ioniq6 ECU Disable V2] Calling disable_ecu(addr=0x{addr:x}, bus={bus}, security_access=True)...")

        # Communication control with security access
        com_cont = bytes([0x28, 0x83, 0x01])  # Disable TX/RX

        # Call the existing disable_ecu function with SecurityAccess enabled
        disable_ecu(
            can_recv=can_recv,
            can_send=can_send,
            bus=bus,
            addr=addr,
            com_cont_req=com_cont,
            timeout=0.5,
            retry=3,
            security_access=True
        )

        print("[Ioniq6 ECU Disable V2] ✓ disable_ecu completed")
        return True

    except Exception as e:
        print(f"[Ioniq6 ECU Disable V2] ✗ Failed: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = ioniq6_ecu_disable()
    sys.exit(0 if success else 1)
