#!/usr/bin/env python3
"""
Door Lock Test - Try captured patterns
"""
from panda import Panda
import time
import subprocess

print("=" * 60)
print("DOOR LOCK TEST - CAPTURED PATTERNS")
print("=" * 60)

print("\nStopping openpilot...")
subprocess.run(["pkill", "-f", "selfdrive"], capture_output=True)
subprocess.run(["pkill", "-f", "_ui"], capture_output=True)
time.sleep(2)

print("\nCar must be in READY mode")
print("Press Enter when ready...")
input()

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
    """Get current lock state from 0x414"""
    p.can_clear(0xFFFF)
    time.sleep(0.3)
    for _ in range(20):
        for m in safe_recv():
            if m[0] == 0x414 and len(m[1]) >= 5:
                state = (m[1][4] >> 5) & 0x01
                return "UNLOCKED" if state == 1 else "LOCKED"
        time.sleep(0.05)
    return "UNKNOWN"

print(f"\nCurrent state: {get_lock_state()}")

# Captured patterns from physical button:
# UNLOCK: a464011500000000 (byte1=0x64, byte3=0x15)
# LOCK: We need to capture this!

print("\n" + "=" * 60)
print("First, let's capture the LOCK pattern")
print("=" * 60)
print("\nPress the LOCK button on the door NOW!")

p.can_clear(0xFFFF)
start = time.time()
lock_pattern = None

while time.time() - start < 10:
    for m in safe_recv():
        addr, data, bus = m

        if addr == 0x3FF:
            t = time.time() - start
            print(f"  {t:.2f}s: 0x3FF = {data.hex()}")
            # Check if this is a LOCK pattern (byte3 should be 0x00 for lock)
            if len(data) >= 4 and data[3] == 0x00:
                lock_pattern = data.hex()
                print(f"  >>> LOCK pattern captured!")

        if addr == 0x414 and len(data) >= 6:
            state = (data[4] >> 5) & 0x01
            action = data[5] & 0x01
            if action == 1:
                t = time.time() - start
                state_str = "UNLOCKED" if state == 1 else "LOCKED"
                print(f"  {t:.2f}s: Button! State={state_str}")

print(f"\nCurrent state: {get_lock_state()}")

if lock_pattern:
    print(f"\nLOCK pattern found: {lock_pattern}")
else:
    print("\nNo LOCK 0x3FF pattern captured. Using default: 5590010000000000")
    lock_pattern = "5590010000000000"

# Known UNLOCK pattern from capture
unlock_pattern = "a464011500000000"

print("\n" + "=" * 60)
print("TEST: Try sending captured patterns")
print("=" * 60)

input("\nPress Enter to try LOCK command...")
print(f"Sending LOCK: {lock_pattern}")
for i in range(5):
    # Try different counter values
    data = bytes.fromhex(lock_pattern)
    data = bytes([i * 0x10]) + data[1:]  # Vary counter
    p.can_send(0x3FF, data, 1)
    time.sleep(0.04)

time.sleep(0.5)
print(f"State after: {get_lock_state()}")
print("Did doors LOCK? (check physically)")

input("\nPress Enter to try UNLOCK command...")
print(f"Sending UNLOCK: {unlock_pattern}")
for i in range(5):
    data = bytes.fromhex(unlock_pattern)
    data = bytes([i * 0x10]) + data[1:]  # Vary counter
    p.can_send(0x3FF, data, 1)
    time.sleep(0.04)

time.sleep(0.5)
print(f"State after: {get_lock_state()}")
print("Did doors UNLOCK? (check physically)")

# Also try on bus 2 (some messages were there)
print("\n" + "=" * 60)
print("TEST: Try on bus 2")
print("=" * 60)

input("\nPress Enter to try LOCK on bus 2...")
for i in range(5):
    data = bytes.fromhex(lock_pattern)
    p.can_send(0x3FF, data, 2)
    time.sleep(0.04)
print(f"State: {get_lock_state()}")

input("\nPress Enter to try UNLOCK on bus 2...")
for i in range(5):
    data = bytes.fromhex(unlock_pattern)
    p.can_send(0x3FF, data, 2)
    time.sleep(0.04)
print(f"State: {get_lock_state()}")

print("\n" + "=" * 60)
print("DONE - Reboot with: sudo reboot")
print("=" * 60)
