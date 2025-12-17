#!/usr/bin/env python3
"""
Door Lock Test - Ready Mode (Fixed)
"""
from panda import Panda
import time
import subprocess

print("=" * 60)
print("DOOR LOCK TEST - READY MODE")
print("=" * 60)

# Stop openpilot first
print("\nStopping openpilot...")
subprocess.run(["pkill", "-f", "selfdrive"], capture_output=True)
subprocess.run(["pkill", "-f", "_ui"], capture_output=True)
time.sleep(2)

print("\nIMPORTANT: Car must be in READY mode (not just ACC)")
print("Press Enter when ready...")
input()

p = Panda()
try:
    p.set_safety_mode(Panda.SAFETY_ALLOUTPUT)
except:
    p.set_safety_mode(17)

# Reset panda to clear buffers
p.can_clear(0xFFFF)
time.sleep(0.5)

print("Panda connected!")

def safe_recv():
    """Safe CAN receive with proper parsing"""
    try:
        msgs = p.can_recv()
        result = []
        for m in msgs:
            # Handle different tuple formats
            if len(m) >= 4:
                addr, ts, data, bus = m[0], m[1], m[2], m[3]
            elif len(m) == 3:
                addr, data, bus = m[0], m[1], m[2]
                ts = 0
            else:
                continue

            # Make sure data is bytes
            if isinstance(data, int):
                continue
            if isinstance(data, (bytes, bytearray)):
                result.append((addr, bytes(data), bus))
        return result
    except Exception as e:
        print(f"  recv error: {e}")
        return []

# First, just see if we get ANY CAN messages
print("\n" + "=" * 60)
print("TEST 0: Basic CAN Reception Check")
print("=" * 60)

p.can_clear(0xFFFF)
time.sleep(0.3)
raw_msgs = []
try:
    raw_msgs = p.can_recv()
except Exception as e:
    print(f"Raw recv error: {e}")

print(f"Raw messages received: {len(raw_msgs)}")
if raw_msgs:
    print("Sample messages:")
    for i, m in enumerate(raw_msgs[:5]):
        print(f"  {i}: {m}")

# Check message format
if raw_msgs:
    m = raw_msgs[0]
    print(f"\nMessage format: {len(m)} elements")
    print(f"  Element types: {[type(x).__name__ for x in m]}")

# Now let's try to get 0x414
print("\n" + "=" * 60)
print("TEST 1: Find 0x414 (Door Lock Status)")
print("=" * 60)

p.can_clear(0xFFFF)
time.sleep(0.5)

found_414 = False
for _ in range(50):  # Try for ~5 seconds
    try:
        msgs = p.can_recv()
        for m in msgs:
            addr = m[0]
            if addr == 0x414:
                data = m[2] if len(m) > 2 else m[1]
                if isinstance(data, (bytes, bytearray)) and len(data) >= 5:
                    state = (data[4] >> 5) & 0x01
                    state_str = "UNLOCKED" if state == 1 else "LOCKED"
                    print(f"Found 0x414! State: {state_str}")
                    print(f"  Raw: {data.hex()}")
                    found_414 = True
                    break
        if found_414:
            break
    except:
        pass
    time.sleep(0.1)

if not found_414:
    print("Could not find 0x414 - CAN may not be working")
    print("\nTrying bus 0 instead...")

    # Maybe messages are on bus 0?
    p.can_clear(0xFFFF)
    time.sleep(0.3)
    msgs = safe_recv()

    bus_counts = {}
    addr_samples = {}
    for addr, data, bus in msgs:
        bus_counts[bus] = bus_counts.get(bus, 0) + 1
        if addr not in addr_samples:
            addr_samples[addr] = (data, bus)

    print(f"\nMessages by bus: {bus_counts}")
    print(f"Unique addresses: {len(addr_samples)}")

    if addr_samples:
        print("\nSample addresses:")
        for addr, (data, bus) in list(addr_samples.items())[:10]:
            print(f"  0x{addr:03X} bus{bus}: {data.hex()}")

# ECU Scan
print("\n" + "=" * 60)
print("TEST 2: ECU Scan")
print("=" * 60)

found_ecus = []
for bus in [0, 1, 2]:
    print(f"\nScanning bus {bus}...")
    for addr in range(0x700, 0x7F0, 0x10):  # Faster scan
        req = bytes([0x02, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
        p.can_clear(0xFFFF)
        p.can_send(addr, req, bus)
        time.sleep(0.03)

        for m in safe_recv():
            if m[0] == addr + 8:
                data = m[1]
                if len(data) > 1 and data[1] == 0x7E:
                    print(f"  Found: 0x{addr:03X} on bus {bus}")
                    found_ecus.append((addr, bus))

print(f"\nTotal ECUs: {len(found_ecus)}")

# Try 0x7B1 specifically
print("\n" + "=" * 60)
print("TEST 3: Direct IBU (0x7B1) Test")
print("=" * 60)

for bus in [0, 1, 2]:
    req = bytes([0x02, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
    p.can_clear(0xFFFF)
    p.can_send(0x7B1, req, bus)
    time.sleep(0.1)

    for m in safe_recv():
        recv_addr = m[0]
        if recv_addr >= 0x7B0 and recv_addr <= 0x7C0:
            print(f"  Response on bus {bus}: 0x{recv_addr:03X} = {m[1].hex()}")

# Monitor for button press
print("\n" + "=" * 60)
print("TEST 4: Physical Button Capture")
print("=" * 60)
print("\nPress door lock button NOW! (5 sec)")

p.can_clear(0xFFFF)
start = time.time()
lock_events = []

while time.time() - start < 5:
    for m in safe_recv():
        addr, data, bus = m

        if addr == 0x414 and len(data) >= 6:
            state = (data[4] >> 5) & 0x01
            action = data[5] & 0x01
            if action == 1:
                t = time.time() - start
                state_str = "UNLOCKED" if state == 1 else "LOCKED"
                print(f"  {t:.2f}s: BUTTON! State={state_str}")
                lock_events.append(('414_action', t, data.hex()))

        elif addr == 0x3FF:
            t = time.time() - start
            print(f"  {t:.2f}s: 0x3FF = {data.hex()}")
            lock_events.append(('3ff', t, data.hex()))

print(f"\nCaptured {len(lock_events)} lock-related events")

# Try sending commands
print("\n" + "=" * 60)
print("TEST 5: Send Lock Commands")
print("=" * 60)

print("\nTrying 0x3FF LOCK pattern...")
for _ in range(3):
    p.can_send(0x3FF, bytes.fromhex("5590010000000000"), 1)
    time.sleep(0.04)
print("  Sent. Did doors lock? (check physically)")

time.sleep(1)

print("\nTrying 0x3FF UNLOCK pattern...")
for _ in range(3):
    p.can_send(0x3FF, bytes.fromhex("0074011500000000"), 1)
    time.sleep(0.04)
print("  Sent. Did doors unlock? (check physically)")

print("\n" + "=" * 60)
print("DONE")
print("=" * 60)
print("\nRestart openpilot with: sudo reboot")
