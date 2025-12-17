#!/usr/bin/env python3
"""
Door Lock Test - Ready Mode
Run this with car FULLY ON (Ready mode - press brake + start twice)
Vehicle must be in PARK with foot on brake

Tests all known approaches now that ECUs should be fully active.
"""
from panda import Panda
import time
import subprocess

print("=" * 60)
print("DOOR LOCK TEST - READY MODE")
print("=" * 60)

# Stop openpilot first to avoid CAN conflicts
print("\nStopping openpilot...")
subprocess.run(["pkill", "-f", "selfdrive"], capture_output=True)
subprocess.run(["pkill", "-f", "_ui"], capture_output=True)
time.sleep(2)

print("\nIMPORTANT: Car must be:")
print("  1. In PARK")
print("  2. Foot on BRAKE")
print("  3. READY mode (not just ACC)")
print("\nPress Enter when ready...")
input()

p = Panda()
try:
    p.set_safety_mode(Panda.SAFETY_ALLOUTPUT)
except:
    p.set_safety_mode(17)

# Clear any corrupted buffer
try:
    p.can_clear(0xFFFF)
    time.sleep(0.1)
    p.can_recv()
except:
    pass

print("Panda connected!\n")

def safe_can_recv():
    """Receive CAN messages with error handling"""
    try:
        return p.can_recv()
    except AssertionError:
        # Clear buffer on checksum error
        try:
            p.can_clear(0xFFFF)
        except:
            pass
        return []

def send_uds(addr, data, bus=1, wait=0.1):
    """Send UDS request and get response"""
    try:
        p.can_clear(0xFFFF)
    except:
        pass
    p.can_send(addr, data, bus)
    time.sleep(wait)

    responses = []
    for msg in safe_can_recv():
        recv_addr = msg[0]
        recv_data = msg[2] if len(msg) > 2 else msg[1]
        recv_bus = msg[3] if len(msg) > 3 else (msg[2] if len(msg) > 2 else 0)
        if recv_addr == addr + 8:
            responses.append((recv_addr, recv_data, recv_bus))
    return responses

def try_extended_session(addr, bus=1):
    """Enter extended diagnostic session"""
    data = bytes([0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00])
    resp = send_uds(addr, data, bus)
    for r in resp:
        if len(r[1]) > 1 and r[1][1] == 0x50:
            return True
    return False

def monitor_414_state():
    """Read current door lock state from 0x414"""
    try:
        p.can_clear(0xFFFF)
    except:
        pass
    time.sleep(0.2)

    for msg in safe_can_recv():
        if msg[0] == 0x414:
            data = msg[2] if len(msg) > 2 else msg[1]
            if len(data) >= 5:
                state = (data[4] >> 5) & 0x01
                return "UNLOCKED" if state == 1 else "LOCKED"
    return "UNKNOWN"

print("Current lock state:", monitor_414_state())
print()

# Test 1: Full ECU scan in Ready mode
print("=" * 60)
print("TEST 1: Full ECU Scan (Ready Mode)")
print("=" * 60)

found_ecus = []
for addr in range(0x700, 0x800):
    data = bytes([0x02, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
    try:
        p.can_clear(0xFFFF)
    except:
        pass
    p.can_send(addr, data, 1)  # Bus 1 (ECAN)
    time.sleep(0.02)

    for msg in safe_can_recv():
        recv_addr = msg[0]
        recv_data = msg[2] if len(msg) > 2 else msg[1]
        if recv_addr == addr + 8 and len(recv_data) > 1 and recv_data[1] == 0x7E:
            found_ecus.append(addr)
            print(f"  Found: 0x{addr:03X} -> 0x{recv_addr:03X}")

print(f"\nTotal ECUs found on bus 1: {len(found_ecus)}")

# Also scan bus 0
print("\nScanning bus 0...")
for addr in range(0x700, 0x800):
    data = bytes([0x02, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
    try:
        p.can_clear(0xFFFF)
    except:
        pass
    p.can_send(addr, data, 0)  # Bus 0
    time.sleep(0.02)

    for msg in safe_can_recv():
        recv_addr = msg[0]
        recv_data = msg[2] if len(msg) > 2 else msg[1]
        if recv_addr == addr + 8 and len(recv_data) > 1 and recv_data[1] == 0x7E:
            if addr not in found_ecus:
                found_ecus.append(addr)
            print(f"  Found on bus 0: 0x{addr:03X} -> 0x{recv_addr:03X}")

# Test 2: Check if IBU (0x7B1) responds now
print("\n" + "=" * 60)
print("TEST 2: Check IBU (0x7B1) in Ready Mode")
print("=" * 60)

# Try tester present
data = bytes([0x02, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
resp = send_uds(0x7B1, data, 1, 0.1)
if resp:
    print("  IBU 0x7B1 RESPONDS!")
    for r in resp:
        print(f"    Response: {r[1].hex()}")
else:
    print("  IBU 0x7B1 still not responding via UDS")

# Test 3: Try functional addressing for body control
print("\n" + "=" * 60)
print("TEST 3: Functional Addressing for Body Control")
print("=" * 60)

# Standard OBD functional address
data = bytes([0x02, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
try:
    p.can_clear(0xFFFF)
except:
    pass
p.can_send(0x7DF, data, 1)
time.sleep(0.2)

print("Responses to 0x7DF (functional):")
for msg in safe_can_recv():
    recv_addr = msg[0]
    recv_data = msg[2] if len(msg) > 2 else msg[1]
    if recv_addr >= 0x700 and recv_addr < 0x800:
        print(f"  0x{recv_addr:03X}: {recv_data.hex()}")

# Test 4: IO Control on all found ECUs
print("\n" + "=" * 60)
print("TEST 4: IO Control on Found ECUs")
print("=" * 60)

# Common body control DIDs
body_dids = [
    (0xB010, "Door Lock Control"),
    (0xD001, "Central Lock"),
    (0xF011, "Body Function 11"),
    (0xF012, "Body Function 12"),
    (0x0100, "Central Lock Alt"),
    (0x3000, "Door Lock"),
    (0x3001, "Door Unlock"),
]

# Test on 0x7B3 which we know responds
test_addrs = [0x7B3] + [a for a in found_ecus if a not in [0x7B3]]

for addr in test_addrs[:5]:  # Test top 5
    print(f"\nTesting 0x{addr:03X}:")

    # Enter extended session
    if try_extended_session(addr, 1):
        print(f"  Session OK on 0x{addr:03X}")

        for did, name in body_dids:
            did_high = (did >> 8) & 0xFF
            did_low = did & 0xFF

            # IO Control - Short Term Adjustment (LOCK = 0x01)
            io_req = bytes([0x05, 0x2F, did_high, did_low, 0x03, 0x01, 0x00, 0x00])
            try:
                p.can_clear(0xFFFF)
            except:
                pass
            p.can_send(addr, io_req, 1)
            time.sleep(0.1)

            for msg in safe_can_recv():
                recv_addr = msg[0]
                recv_data = msg[2] if len(msg) > 2 else msg[1]
                if recv_addr == addr + 8:
                    if len(recv_data) > 1:
                        if recv_data[1] == 0x6F:
                            print(f"  >>> {name} (0x{did:04X}): POSITIVE RESPONSE!")
                            print(f"      Check if doors locked!")
                        elif recv_data[1] == 0x7F:
                            nrc = recv_data[3] if len(recv_data) > 3 else 0
                            # Only show if not "service not supported"
                            if nrc not in [0x11, 0x12, 0x31]:
                                print(f"  {name}: NRC 0x{nrc:02X}")
    else:
        print(f"  Could not establish session on 0x{addr:03X}")

# Test 5: Try 0x3FF patterns
print("\n" + "=" * 60)
print("TEST 5: 0x3FF Message Patterns")
print("=" * 60)

print("\nSending LOCK pattern (0x5590010000000000)...")
print("Current state:", monitor_414_state())

for i in range(3):
    p.can_send(0x3FF, bytes.fromhex("5590010000000000"), 1)
    time.sleep(0.04)

time.sleep(0.5)
print("State after:", monitor_414_state())

print("\nSending UNLOCK pattern (0x0074011500000000)...")
for i in range(3):
    p.can_send(0x3FF, bytes.fromhex("0074011500000000"), 1)
    time.sleep(0.04)

time.sleep(0.5)
print("State after:", monitor_414_state())

# Test 6: Direct 0x414 manipulation
print("\n" + "=" * 60)
print("TEST 6: Direct 0x414 with ACTION bit")
print("=" * 60)

print("\nSending 0x414 LOCK (state=0, action=1)...")
print("Current state:", monitor_414_state())

# Based on captured data: byte4 bit5 = state, byte5 bit0 = action
for i in range(5):
    # LOCK: byte4=0x08 (bit5=0=locked), byte5=0x01 (action)
    p.can_send(0x414, bytes.fromhex("0000000008010000"), 1)
    time.sleep(0.04)

time.sleep(0.5)
print("State after:", monitor_414_state())

print("\nSending 0x414 UNLOCK (state=1, action=1)...")
for i in range(5):
    # UNLOCK: byte4=0x28 (bit5=1=unlocked), byte5=0x01 (action)
    p.can_send(0x414, bytes.fromhex("0000000028010000"), 1)
    time.sleep(0.04)

time.sleep(0.5)
print("State after:", monitor_414_state())

# Test 7: Routine Control
print("\n" + "=" * 60)
print("TEST 7: Routine Control")
print("=" * 60)

routine_ids = [
    (0xD001, "Door Routine"),
    (0xD010, "Lock Routine"),
    (0xB001, "Body Routine"),
    (0xFF00, "Factory Routine"),
]

for addr in [0x7B3, 0x730]:
    print(f"\nRoutine Control on 0x{addr:03X}:")

    if try_extended_session(addr, 1):
        for rid, name in routine_ids:
            rid_high = (rid >> 8) & 0xFF
            rid_low = rid & 0xFF

            # Routine Control Start with param 0x01 (lock)
            routine_req = bytes([0x05, 0x31, 0x01, rid_high, rid_low, 0x01, 0x00, 0x00])
            try:
                p.can_clear(0xFFFF)
            except:
                pass
            p.can_send(addr, routine_req, 1)
            time.sleep(0.1)

            for msg in safe_can_recv():
                recv_addr = msg[0]
                recv_data = msg[2] if len(msg) > 2 else msg[1]
                if recv_addr == addr + 8 and len(recv_data) > 1:
                    if recv_data[1] == 0x71:
                        print(f"  >>> {name}: POSITIVE RESPONSE!")
                    elif recv_data[1] == 0x7F:
                        nrc = recv_data[3] if len(recv_data) > 3 else 0
                        if nrc not in [0x11, 0x12, 0x31]:
                            print(f"  {name}: NRC 0x{nrc:02X}")

# Test 8: Monitor for new messages during physical button press
print("\n" + "=" * 60)
print("TEST 8: Capture Physical Button Press")
print("=" * 60)
print("\nPress the physical door lock button NOW!")
print("Monitoring for 5 seconds...")

try:
    p.can_clear(0xFFFF)
except:
    pass
start = time.time()
seen_addrs = set()
messages_by_addr = {}

while time.time() - start < 5:
    for msg in safe_can_recv():
        addr = msg[0]
        data = msg[2] if len(msg) > 2 else msg[1]
        bus = msg[3] if len(msg) > 3 else 0

        if addr not in messages_by_addr:
            messages_by_addr[addr] = []
        messages_by_addr[addr].append((time.time() - start, data.hex(), bus))

        # Print interesting messages
        if addr == 0x414:
            state = (data[4] >> 5) & 0x01 if len(data) > 4 else -1
            action = data[5] & 0x01 if len(data) > 5 else -1
            state_str = "UNLOCKED" if state == 1 else "LOCKED"
            if action == 1:
                print(f"  {time.time()-start:.2f}s: 0x414 BUTTON PRESS! State={state_str}")
        elif addr == 0x3FF:
            print(f"  {time.time()-start:.2f}s: 0x3FF {data.hex()}")
        elif addr not in seen_addrs and addr < 0x700:
            seen_addrs.add(addr)

print(f"\nSaw {len(messages_by_addr)} unique message IDs")

# Look for rare messages
for addr, msgs in messages_by_addr.items():
    if len(msgs) <= 10 and addr not in [0x414, 0x3FF]:  # Rare messages
        print(f"\n0x{addr:03X} ({len(msgs)} msgs):")
        for t, data, bus in msgs[:5]:
            print(f"  {t:.2f}s bus{bus}: {data}")

print("\n" + "=" * 60)
print("TESTS COMPLETE")
print("=" * 60)
print("\nFinal lock state:", monitor_414_state())
print("\nIf no tests worked, door lock control may require:")
print("  1. Direct LIN bus access (not available via OBD)")
print("  2. Factory diagnostic tool with security access")
print("  3. Different ECU not accessible via gateway")
