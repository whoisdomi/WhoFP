from opendbc.car.carlog import carlog
from opendbc.car.isotp_parallel_query import IsoTpParallelQuery

EXT_DIAG_REQUEST = b'\x10\x03'
EXT_DIAG_RESPONSE = b'\x50\x03'

COM_CONT_RESPONSE = b''


def disable_ecu(can_recv, can_send, bus=0, addr=0x7d0, sub_addr=None, com_cont_req=b'\x28\x83\x01', timeout=0.1, retry=10, security_access=False):
  """Silence an ECU by disabling sending and receiving messages using UDS 0x28.
  The ECU will stay silent as long as openpilot keeps sending Tester Present.

  This is used to disable the radar in some cars. Openpilot will emulate the radar.
  WARNING: THIS DISABLES AEB!

  Args:
    security_access: If True, performs SecurityAccess seed request (0x27) before Communication Control.
                     Required for HDA2 cars (e.g., Ioniq 6) that return 0x7F2822 without this handshake.
                     Note: We only request the seed, not send a key - the request itself changes ECU state.
  """
  carlog.warning(f"ecu disable {hex(addr), sub_addr} ...")

  for i in range(retry):
    try:
      query = IsoTpParallelQuery(can_send, can_recv, bus, [(addr, sub_addr)], [EXT_DIAG_REQUEST], [EXT_DIAG_RESPONSE])

      for _, _ in query.get_data(timeout).items():
        # For HDA2 cars, perform SecurityAccess handshake before Communication Control
        # This changes the ECU's internal state machine to accept the disable command
        if security_access:
          carlog.warning("security access seed request ...")
          try:
            # Request seed (0x27 0x01) - we don't send a key back
            # The ECU just needs to see the SecurityAccess request
            sec_query = IsoTpParallelQuery(can_send, can_recv, bus, [(addr, sub_addr)], [b'\x27\x01'], [b''])
            sec_query.get_data(timeout)
            carlog.warning("security access handshake completed")
          except Exception:
            carlog.warning("security access failed, attempting communication control anyway")

        carlog.warning("communication control disable tx/rx ...")

        query = IsoTpParallelQuery(can_send, can_recv, bus, [(addr, sub_addr)], [com_cont_req], [COM_CONT_RESPONSE])
        query.get_data(0)

        carlog.warning("ecu disabled")
        return True

    except Exception:
      carlog.exception("ecu disable exception")

    carlog.error(f"ecu disable retry ({i + 1}) ...")
  carlog.error("ecu disable failed")
  return False
