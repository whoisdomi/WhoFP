#!/usr/bin/env python3
"""
Full-bus stop sign identifier with ENTER key marking.

Run on the comma device via SSH while openpilot is running:
  python3 /data/openpilot/frogpilot/tools/verify_dash_stop_sign.py

Phase 1 (10 sec): Learn noisy bytes. Drive with NO stop sign on dash.
Phase 2: Drive toward stop signs.
  - Press ENTER when dash stop sign icon APPEARS
  - Press ENTER again when it DISAPPEARS
  - Repeat for multiple stop signs if possible.
  - Press Ctrl+C when done — shows summary of signals that correlate.

The script finds bytes that ONLY changed during the ON window.
"""
import cereal.messaging as messaging
import sys
import select
import termios
import tty
import time

def check_keypress():
  if select.select([sys.stdin], [], [], 0)[0]:
    sys.stdin.read(1)
    return True
  return False

def main():
  old_settings = termios.tcgetattr(sys.stdin)
  tty.setcbreak(sys.stdin.fileno())

  try:
    sm = messaging.SubMaster(["can"])

    # Phase 1: Learn noisy bytes
    print("Phase 1: Learning noisy bytes (10 seconds)...")
    print("         Drive normally — NO stop sign on dash!")
    print()

    baseline = {}
    noisy_bytes = set()
    frames = 0
    target = 200

    while frames < target:
      sm.update(100)
      if sm.updated["can"]:
        for msg in sm["can"]:
          key = (msg.src, msg.address)
          data = bytes(msg.dat)
          if key in baseline:
            old = baseline[key]
            for i in range(min(len(old), len(data))):
              if old[i] != data[i]:
                noisy_bytes.add((msg.src, msg.address, i))
          baseline[key] = data
        frames += 1

    print(f"Learned {len(noisy_bytes)} noisy byte positions")
    print()
    print("Phase 2: Drive toward stop signs.")
    print("  Press ENTER when dash stop sign icon APPEARS")
    print("  Press ENTER again when it DISAPPEARS")
    print("  Repeat for multiple stop signs. Ctrl+C when done.")
    print("-" * 60)

    # Phase 2: Track changes correlated with ENTER presses
    dash_on = False
    # For each (bus, addr, byte_idx): track how many times it changed
    # during ON windows vs OFF windows
    on_changes = {}   # (bus, addr, byte_idx) -> count of changes during ON
    off_changes = {}  # (bus, addr, byte_idx) -> count of changes during OFF
    on_windows = 0
    # Track values at ON/OFF transitions
    on_snapshots = []   # list of baseline dicts at moment of ON press
    off_snapshots = []  # list of baseline dicts at moment of OFF press

    while True:
      sm.update(100)

      if check_keypress():
        dash_on = not dash_on
        if dash_on:
          on_windows += 1
          on_snapshots.append(dict(baseline))
          print(f"  >>> DASH ON  (window #{on_windows})")
        else:
          off_snapshots.append(dict(baseline))
          print(f"  >>> DASH OFF (window #{on_windows})")

      if sm.updated["can"]:
        for msg in sm["can"]:
          key = (msg.src, msg.address)
          data = bytes(msg.dat)
          if key in baseline:
            old = baseline[key]
            for i in range(min(len(old), len(data))):
              change_key = (msg.src, msg.address, i)
              if old[i] != data[i] and change_key not in noisy_bytes:
                if dash_on:
                  on_changes[change_key] = on_changes.get(change_key, 0) + 1
                else:
                  off_changes[change_key] = off_changes.get(change_key, 0) + 1
          baseline[key] = data

  except KeyboardInterrupt:
    pass
  finally:
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)

  # Analysis
  print("\n" + "=" * 60)
  print(f"ANALYSIS — {on_windows} ON/OFF window(s) recorded")
  print("=" * 60)

  if not on_snapshots:
    print("No ON windows recorded.")
    return

  # Find bytes that changed ONLY during ON (not during OFF)
  on_only = set(on_changes.keys()) - set(off_changes.keys())
  # Find bytes that changed during ON AND OFF
  both = set(on_changes.keys()) & set(off_changes.keys())
  # Find bytes that changed much more during ON than OFF
  biased = {}
  for k in both:
    ratio = on_changes[k] / max(off_changes[k], 1)
    if ratio > 3:
      biased[k] = ratio

  if on_only:
    print(f"\nBytes that ONLY changed during DASH ON ({len(on_only)}):")
    sorted_keys = sorted(on_only, key=lambda k: (-on_changes[k], k))
    for bus, addr, byte_idx in sorted_keys:
      count = on_changes[(bus, addr, byte_idx)]
      # Show value at ON and OFF snapshots
      vals_on = []
      vals_off = []
      for snap in on_snapshots:
        key = (bus, addr)
        if key in snap and byte_idx < len(snap[key]):
          vals_on.append(snap[key][byte_idx])
      for snap in off_snapshots:
        key = (bus, addr)
        if key in snap and byte_idx < len(snap[key]):
          vals_off.append(snap[key][byte_idx])
      print(f"  Bus {bus:>3d}  Msg 0x{addr:03X} ({addr:>4d})  BYTE[{byte_idx:>2d}]  "
            f"changes={count}  at_ON={vals_on}  at_OFF={vals_off}")

  if biased:
    print(f"\nBytes that changed MUCH MORE during DASH ON ({len(biased)}):")
    sorted_keys = sorted(biased.keys(), key=lambda k: -biased[k])
    for bus, addr, byte_idx in sorted_keys[:20]:
      ratio = biased[(bus, addr, byte_idx)]
      print(f"  Bus {bus:>3d}  Msg 0x{addr:03X} ({addr:>4d})  BYTE[{byte_idx:>2d}]  "
            f"ON={on_changes[(bus,addr,byte_idx)]}x  OFF={off_changes[(bus,addr,byte_idx)]}x  ratio={ratio:.1f}")

  if not on_only and not biased:
    print("\nNo strong candidates found. Try with more ON/OFF windows.")

if __name__ == "__main__":
  main()
