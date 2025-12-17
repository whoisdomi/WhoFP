#!/usr/bin/env python3
"""
Capture KEY FOB lock/unlock - might use CAN instead of LIN!
"""
from panda import Panda
import time
import subprocess

print("=" * 60)
print("KEY FOB LOCK/UNLOCK CAPTURE")
print("=" * 60)

print("\nStopping openpilot...")
subprocess.run(["pkill", "-f", "selfdrive"], capture_output=True)
subprocess.run(["pkill", "-f", "_ui"], capture_output=True)
time.sleep(2)

print("\nCar must be in READY mode (or at least ACC)")
print("Have your KEY FOB ready!")
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

# Collect baseline messages for 2 seconds
print("\nCollecting baseline (don't press anything)...")
p.can_clear(0xFFFF)
baseline_addrs = {}
start = time.time()
while time.time() - start < 2:
    for addr, data, bus in safe_recv():
        if addr not in baseline_addrs:
            baseline_addrs[addr] = []
        baseline_addrs[addr].append(data.hex())

print(f"Baseline: {len(baseline_addrs)} unique addresses")

# Now capture key fob
print("\n" + "=" * 60)
print("Press KEY FOB LOCK button NOW!")
print("Monitoring for 10 seconds...")
print("=" * 60)

p.can_clear(0xFFFF)
start = time.time()
all_events = []
new_addrs = set()

while time.time() - start < 10:
    for addr, data, bus in safe_recv():
        t = time.time() - start

        # Track all messages
        all_events.append((t, addr, data, bus))

        # Highlight messages we haven't seen in baseline
        if addr not in baseline_addrs:
            if addr not in new_addrs:
                print(f"  {t:.2f}s: NEW 0x{addr:03X} bus{bus} = {data.hex()}")
                new_addrs.add(addr)

        # Always show 0x414 state changes
        if addr == 0x414 and len(data) >= 6:
            state = (data[4] >> 5) & 0x01
            action = data[5] & 0x01
            if action == 1:
                state_str = "UNLOCKED" if state == 1 else "LOCKED"
                print(f"  {t:.2f}s: 0x414 State={state_str} (action=1)")

        # Always show 0x3FF
        if addr == 0x3FF:
            print(f"  {t:.2f}s: 0x3FF = {data.hex()}")

print(f"\nTotal events: {len(all_events)}")
print(f"New addresses not in baseline: {len(new_addrs)}")

# Analyze what appeared around lock events
print("\n" + "=" * 60)
print("Analyzing messages around lock/unlock events...")
print("=" * 60)

# Find when 0x414 state changed
lock_times = []
for i, (t, addr, data, bus) in enumerate(all_events):
    if addr == 0x414 and len(data) >= 6:
        action = data[5] & 0x01
        if action == 1:
            state = (data[4] >> 5) & 0x01
            lock_times.append((t, "UNLOCK" if state == 1 else "LOCK"))

print(f"\nLock events detected: {lock_times}")

# For each lock event, show messages 500ms before
for event_time, event_type in lock_times:
    print(f"\n--- {event_type} at {event_time:.2f}s ---")
    print("Messages 500ms BEFORE:")

    before_msgs = {}
    for t, addr, data, bus in all_events:
        if event_time - 0.5 <= t < event_time:
            key = f"0x{addr:03X}"
            if key not in before_msgs:
                before_msgs[key] = []
            before_msgs[key].append((t, data.hex(), bus))

    # Show addresses that are NOT common (not in baseline or low frequency)
    for addr_str, msgs in sorted(before_msgs.items()):
        addr_int = int(addr_str, 16)
        baseline_count = len(baseline_addrs.get(addr_int, []))

        # If this address wasn't in baseline or had few messages, it's interesting
        if baseline_count < 5 or addr_int in [0x3FF, 0x414]:
            for t, data, bus in msgs[-3:]:  # Last 3
                print(f"  {t:.2f}s: {addr_str} bus{bus} = {data}")

# Second capture for UNLOCK
print("\n" + "=" * 60)
print("Now press KEY FOB UNLOCK button!")
print("Monitoring for 10 seconds...")
print("=" * 60)

p.can_clear(0xFFFF)
start = time.time()
all_events2 = []

while time.time() - start < 10:
    for addr, data, bus in safe_recv():
        t = time.time() - start
        all_events2.append((t, addr, data, bus))

        if addr not in baseline_addrs and addr not in new_addrs:
            print(f"  {t:.2f}s: NEW 0x{addr:03X} bus{bus} = {data.hex()}")
            new_addrs.add(addr)

        if addr == 0x414 and len(data) >= 6:
            state = (data[4] >> 5) & 0x01
            action = data[5] & 0x01
            if action == 1:
                state_str = "UNLOCKED" if state == 1 else "LOCKED"
                print(f"  {t:.2f}s: 0x414 State={state_str}")

        if addr == 0x3FF:
            print(f"  {t:.2f}s: 0x3FF = {data.hex()}")

# Find unique messages that appeared ONLY during lock/unlock
print("\n" + "=" * 60)
print("SUMMARY: Potential command addresses")
print("=" * 60)

all_captured = set()
for t, addr, data, bus in all_events + all_events2:
    all_captured.add(addr)

potential_commands = []
for addr in all_captured:
    if addr not in baseline_addrs:
        potential_commands.append(addr)
    elif len(baseline_addrs[addr]) < 3:  # Very rare in baseline
        potential_commands.append(addr)

print(f"\nAddresses that appeared during fob press but not in baseline:")
for addr in sorted(potential_commands):
    print(f"  0x{addr:03X}")

print("\n" + "=" * 60)
print("DONE - Reboot with: sudo reboot")
print("=" * 60)
