#!/usr/bin/env python3
"""
Test 0x4E4 as door lock command!
Found during key fob capture - appears BEFORE lock state changes
"""
from panda import Panda
import time
import subprocess

print("=" * 60)
print("TEST 0x4E4 AS LOCK COMMAND")
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
    p.can_clear(0xFFFF)
    time.sleep(0.3)
    for _ in range(30):
        for m in safe_recv():
            if m[0] == 0x414 and len(m[1]) >= 5:
                state = (m[1][4] >> 5) & 0x01
                return "UNLOCKED" if state == 1 else "LOCKED"
        time.sleep(0.05)
    return "UNKNOWN"

# Captured patterns from key fob:
# UNLOCK: d180010000020000 (byte1=0x80, byte4=0x02)
# LOCK:   6b90010000000000 (byte1=0x90, byte4=0x00)

print(f"\nCurrent state: {get_lock_state()}")

print("\n" + "=" * 60)
print("TEST 1: Send 0x4E4 LOCK pattern")
print("=" * 60)

input("\nPress Enter to send LOCK (6b90010000000000)...")
print("Sending 0x4E4 LOCK pattern...")

# Try with different counters (byte0 varies)
for i in range(10):
    counter = (i * 0x10) & 0xFF
    # LOCK pattern: byte1=0x90, byte4=0x00
    data = bytes([counter, 0x90, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00])
    p.can_send(0x4E4, data, 1)
    time.sleep(0.04)

time.sleep(0.5)
state = get_lock_state()
print(f"State after: {state}")
print(">>> DID THE DOORS LOCK? <<<")

print("\n" + "=" * 60)
print("TEST 2: Send 0x4E4 UNLOCK pattern")
print("=" * 60)

input("\nPress Enter to send UNLOCK (d180010000020000)...")
print("Sending 0x4E4 UNLOCK pattern...")

for i in range(10):
    counter = (i * 0x10) & 0xFF
    # UNLOCK pattern: byte1=0x80, byte4=0x02
    data = bytes([counter, 0x80, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00])
    p.can_send(0x4E4, data, 1)
    time.sleep(0.04)

time.sleep(0.5)
state = get_lock_state()
print(f"State after: {state}")
print(">>> DID THE DOORS UNLOCK? <<<")

print("\n" + "=" * 60)
print("TEST 3: Try exact captured patterns")
print("=" * 60)

input("\nPress Enter to try exact LOCK pattern...")
# Exact captured: 6b90010000000000
for _ in range(5):
    p.can_send(0x4E4, bytes.fromhex("6b90010000000000"), 1)
    time.sleep(0.04)
time.sleep(0.5)
print(f"State: {get_lock_state()}")

input("\nPress Enter to try exact UNLOCK pattern...")
# Exact captured: d180010000020000
for _ in range(5):
    p.can_send(0x4E4, bytes.fromhex("d180010000020000"), 1)
    time.sleep(0.04)
time.sleep(0.5)
print(f"State: {get_lock_state()}")

print("\n" + "=" * 60)
print("TEST 4: Try 0x33A (appeared before lock)")
print("=" * 60)
# 0x33A: 42fd00000000000000000000f000000000000000000000000000000000000000

input("\nPress Enter to try 0x33A...")
for _ in range(5):
    p.can_send(0x33A, bytes.fromhex("42fd00000000000000000000f000000000000000000000000000000000000000"), 1)
    time.sleep(0.04)
time.sleep(0.5)
print(f"State: {get_lock_state()}")

print("\n" + "=" * 60)
print("TEST 5: Try on bus 0 and bus 2")
print("=" * 60)

for bus in [0, 2]:
    input(f"\nPress Enter to try 0x4E4 LOCK on bus {bus}...")
    for _ in range(5):
        p.can_send(0x4E4, bytes.fromhex("6b90010000000000"), bus)
        time.sleep(0.04)
    time.sleep(0.3)
    print(f"State: {get_lock_state()}")

    input(f"\nPress Enter to try 0x4E4 UNLOCK on bus {bus}...")
    for _ in range(5):
        p.can_send(0x4E4, bytes.fromhex("d180010000020000"), bus)
        time.sleep(0.04)
    time.sleep(0.3)
    print(f"State: {get_lock_state()}")

print("\n" + "=" * 60)
print("DONE")
print("=" * 60)
print("\nIf nothing worked, the command may require:")
print("  - Authentication/security handshake")
print("  - Specific counter/checksum values")
print("  - Coming from the actual SMK ECU address")
print("\nReboot with: sudo reboot")
