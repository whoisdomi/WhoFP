#!/usr/bin/env python3
"""
Test door lock with KEY OUTSIDE car + capture auto-lock sequence
"""
from panda import Panda
import time
import subprocess

print("=" * 60)
print("DOOR LOCK TEST - KEY OUTSIDE + AUTO-LOCK CAPTURE")
print("=" * 60)

print("\nStopping openpilot...")
subprocess.run(["pkill", "-f", "selfdrive"], capture_output=True)
subprocess.run(["pkill", "-f", "_ui"], capture_output=True)
time.sleep(2)

p = Panda()
try:
    p.set_safety_mode(Panda.SAFETY_ALLOUTPUT)
except:
    p.set_safety_mode(17)

p.can_clear(0xFFFF)
time.sleep(0.3)
print("Panda connected!")

def safe_recv():
    try:
        msgs = p.can_recv()
        result = []
        for m in msgs:
            if len(m) >= 3:
                addr, data, bus = m[0], m[1], m[2]
                if isinstance(data, (bytes, bytearray)):
                    result.append((addr, bytes(data), bus))
        return result
    except:
        return []

def get_lock_state():
    p.can_clear(0xFFFF)
    time.sleep(0.2)
    for _ in range(20):
        for m in safe_recv():
            if m[0] == 0x414 and len(m[1]) >= 5:
                state = (m[1][4] >> 5) & 0x01
                return "UNLOCKED" if state == 1 else "LOCKED"
        time.sleep(0.05)
    return "UNKNOWN"

print("\n" + "=" * 60)
print("TEST 1: Lock commands with KEY OUTSIDE the car")
print("=" * 60)
print("\nINSTRUCTIONS:")
print("  1. Leave the key fob OUTSIDE the car (on roof, with someone, etc)")
print("  2. Stay inside with laptop/phone connected to comma")
print("  3. Car should be OFF (not ACC, not Ready)")
print("\nPress Enter when key is OUTSIDE and you're ready...")
input()

print(f"Current state: {get_lock_state()}")

print("\nTrying 0x4E4 LOCK command...")
for i in range(10):
    counter = (i * 0x10) & 0xFF
    data = bytes([counter, 0x90, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00])
    p.can_send(0x4E4, data, 1)
    time.sleep(0.04)

time.sleep(1)
print(f"State after: {get_lock_state()}")
print(">>> DID THE DOORS LOCK? (check physically!) <<<")

input("\nPress Enter to try UNLOCK...")
for i in range(10):
    counter = (i * 0x10) & 0xFF
    data = bytes([counter, 0x80, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00])
    p.can_send(0x4E4, data, 1)
    time.sleep(0.04)

time.sleep(1)
print(f"State after: {get_lock_state()}")
print(">>> DID THE DOORS UNLOCK? <<<")

print("\n" + "=" * 60)
print("TEST 2: Capture AUTO-LOCK sequence")
print("=" * 60)
print("\nThis captures what happens when car auto-locks after door close.")
print("\nINSTRUCTIONS:")
print("  1. Car must be OFF (no ACC)")
print("  2. Doors should be UNLOCKED")
print("  3. Open driver door, then close it")
print("  4. Wait for auto-lock (~30 seconds)")
print("\nPress Enter to start monitoring (60 second capture)...")
input()

print("\nMonitoring... Open and close door, then wait for auto-lock!")
print("Will show any interesting messages...")

p.can_clear(0xFFFF)
start = time.time()

# Track baseline addresses
baseline = set()
for _ in range(50):
    for m in safe_recv():
        baseline.add(m[0])
    time.sleep(0.02)

print(f"Baseline addresses: {len(baseline)}")

# Now monitor for changes
all_events = []
last_state = None
door_open_time = None
auto_lock_time = None

while time.time() - start < 60:
    for addr, data, bus in safe_recv():
        t = time.time() - start

        # Track 0x414 state changes
        if addr == 0x414 and len(data) >= 6:
            state = (data[4] >> 5) & 0x01
            action = data[5] & 0x01
            state_str = "UNLOCKED" if state == 1 else "LOCKED"

            if state != last_state:
                print(f"  {t:.1f}s: STATE CHANGE -> {state_str}")
                if state == 0:  # Just locked
                    auto_lock_time = t
                last_state = state

            if action == 1:
                print(f"  {t:.1f}s: ACTION detected (state={state_str})")

        # Track 0x3FF
        if addr == 0x3FF:
            print(f"  {t:.1f}s: 0x3FF = {data.hex()}")
            all_events.append((t, '0x3FF', data.hex()))

        # Track 0x4E4
        if addr == 0x4E4:
            print(f"  {t:.1f}s: 0x4E4 = {data.hex()}")
            all_events.append((t, '0x4E4', data.hex()))

        # NEW addresses not in baseline
        if addr not in baseline:
            print(f"  {t:.1f}s: NEW 0x{addr:03X} = {data.hex()}")
            all_events.append((t, f'0x{addr:03X}', data.hex()))
            baseline.add(addr)  # Don't repeat

        # Door-related messages (common addresses)
        if addr in [0x19A, 0x30A, 0x33A, 0x3CA]:
            all_events.append((t, f'0x{addr:03X}', data.hex()))

print(f"\n\nCapture complete!")
print(f"Recorded {len(all_events)} interesting events")

if auto_lock_time:
    print(f"\nAuto-lock occurred at {auto_lock_time:.1f}s")
    print("\nMessages in 2 seconds BEFORE auto-lock:")
    for t, addr, data in all_events:
        if auto_lock_time - 2 < t < auto_lock_time:
            print(f"  {t:.1f}s: {addr} = {data}")

print("\n" + "=" * 60)
print("TEST 3: Try captured patterns with key outside")
print("=" * 60)

if all_events:
    # Find 0x4E4 patterns
    e4_patterns = [(t, d) for t, a, d in all_events if a == '0x4E4']
    if e4_patterns:
        print(f"\nFound {len(e4_patterns)} 0x4E4 patterns during auto-lock!")
        for t, data in e4_patterns:
            print(f"  {t:.1f}s: {data}")

        input("\nPress Enter to replay these patterns...")
        for t, data in e4_patterns:
            p.can_send(0x4E4, bytes.fromhex(data), 1)
            time.sleep(0.04)

        time.sleep(1)
        print(f"State after: {get_lock_state()}")

print("\n" + "=" * 60)
print("DONE")
print("=" * 60)
print("\nReboot with: sudo reboot")
