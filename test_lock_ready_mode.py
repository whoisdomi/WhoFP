#!/usr/bin/env python3
"""
Capture BlueLink remote lock/unlock command
BlueLink: Phone → Cloud → Cellular → Telematics (CCU) → ??? → IBU
"""
from panda import Panda
import time
import subprocess

print("=" * 60)
print("BLUELINK REMOTE LOCK/UNLOCK CAPTURE")
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

print(f"\nCurrent state: {get_lock_state()}")

# Collect baseline
print("\nCollecting baseline (5 seconds, don't do anything)...")
p.can_clear(0xFFFF)
baseline = {}
start = time.time()
while time.time() - start < 5:
    for addr, data, bus in safe_recv():
        if addr not in baseline:
            baseline[addr] = []
        baseline[addr].append(data.hex())

print(f"Baseline: {len(baseline)} addresses")

print("\n" + "=" * 60)
print("BLUELINK CAPTURE")
print("=" * 60)
print("\nINSTRUCTIONS:")
print("  1. Open BlueLink app on your phone")
print("  2. Go to Remote tab")
print("  3. When ready, press LOCK or UNLOCK in the app")
print("  4. Wait for car to respond (can take 30-60 seconds!)")
print("\nMonitoring for 120 seconds (BlueLink can be slow)...")
print("Press Enter to start, then use BlueLink app...")
input()

p.can_clear(0xFFFF)
start = time.time()
all_events = []
last_state = None
lock_time = None
new_addrs = set()

while time.time() - start < 120:
    for addr, data, bus in safe_recv():
        t = time.time() - start

        # Track state changes
        if addr == 0x414 and len(data) >= 6:
            state = (data[4] >> 5) & 0x01
            action = data[5] & 0x01
            state_str = "UNLOCKED" if state == 1 else "LOCKED"

            if state != last_state:
                print(f"  {t:.1f}s: *** STATE CHANGE -> {state_str} ***")
                lock_time = t
                last_state = state

            if action == 1:
                print(f"  {t:.1f}s: ACTION detected ({state_str})")
                all_events.append((t, '0x414_ACTION', data.hex()))

        # Track 0x3FF
        if addr == 0x3FF:
            print(f"  {t:.1f}s: 0x3FF = {data.hex()}")
            all_events.append((t, '0x3FF', data.hex()))

        # Track 0x4E4 (key fob command)
        if addr == 0x4E4:
            print(f"  {t:.1f}s: 0x4E4 = {data.hex()}")
            all_events.append((t, '0x4E4', data.hex()))

        # NEW addresses not in baseline - these are interesting!
        if addr not in baseline and addr not in new_addrs:
            print(f"  {t:.1f}s: ** NEW ** 0x{addr:03X} bus{bus} = {data.hex()}")
            all_events.append((t, f'NEW_0x{addr:03X}', data.hex()))
            new_addrs.add(addr)

        # Track addresses with LOW frequency in baseline (rare messages)
        if addr in baseline and len(baseline[addr]) < 3:
            all_events.append((t, f'RARE_0x{addr:03X}', data.hex()))

        # Telematics-related addresses (common CCU ranges)
        if addr in range(0x500, 0x600) or addr in range(0x600, 0x700):
            all_events.append((t, f'0x{addr:03X}', data.hex()))

print(f"\n\nCapture complete!")
print(f"Recorded {len(all_events)} interesting events")
print(f"New addresses seen: {len(new_addrs)}")

if lock_time:
    print(f"\n*** Lock/unlock occurred at {lock_time:.1f}s ***")
    print("\nMessages 5 seconds BEFORE state change:")
    print("-" * 50)

    before_events = [(t, a, d) for t, a, d in all_events if lock_time - 5 < t < lock_time]
    for t, addr, data in before_events:
        print(f"  {t:.1f}s: {addr} = {data}")

    print("\nMessages 2 seconds AFTER state change:")
    print("-" * 50)
    after_events = [(t, a, d) for t, a, d in all_events if lock_time < t < lock_time + 2]
    for t, addr, data in after_events:
        print(f"  {t:.1f}s: {addr} = {data}")

# If we found any new addresses, try replaying them
if new_addrs and lock_time:
    print("\n" + "=" * 60)
    print("TEST: Replay new addresses found during BlueLink")
    print("=" * 60)

    # Find events from new addresses that appeared before lock
    replay_candidates = []
    for t, a, d in all_events:
        if a.startswith('NEW_') and lock_time - 5 < t < lock_time:
            addr_hex = a.replace('NEW_', '')
            addr_int = int(addr_hex, 16)
            replay_candidates.append((addr_int, d))

    if replay_candidates:
        print(f"\nFound {len(replay_candidates)} candidates to replay!")
        for addr, data in replay_candidates:
            print(f"  0x{addr:03X} = {data}")

        input("\nPress Enter to try replaying these...")
        for addr, data in replay_candidates:
            print(f"Sending 0x{addr:03X} = {data}")
            for _ in range(5):
                p.can_send(addr, bytes.fromhex(data), 1)
                time.sleep(0.04)

        time.sleep(1)
        print(f"\nState after replay: {get_lock_state()}")
        print(">>> DID THE DOORS MOVE? <<<")

print("\n" + "=" * 60)
print("DONE")
print("=" * 60)
print("\nReboot with: sudo reboot")
