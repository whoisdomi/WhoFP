# STAR Button Integration Guide

## What Was Added

Added STAR button detection to your 2023 Hyundai Ioniq 6:
- **Bus**: 1 (ECAN)
- **Message ID**: 74 (0x4a) - "STEERING_BUTTONS"
- **Signal**: "STAR_BUTTON" at Byte[10]
- **Pressed**: 0xe5 (229)
- **Released**: 0xdf (223)

## Files Modified

1. `opendbc_repo/opendbc/dbc/generator/hyundai/hyundai_canfd.dbc`
   - Added `BO_ 74 STEERING_BUTTONS` message
   - Added `STAR_BUTTON` signal

## Testing on Comma Device

### Step 1: Rebuild opendbc

SSH into your comma device and run:

```bash
cd /data/openpilot/opendbc_repo
scons -j4
```

### Step 2: Test the button

```bash
cd /data/openpilot
python3 test_star_button_dbc.py
```

Press the STAR button - you should see "PRESSED" and "RELEASED" events being detected.

## Using in FrogPilot Code

### Option 1: In carstate.py (Read button state)

Add button reading to `opendbc_repo/opendbc/car/hyundai/carstate.py`:

```python
def update(self, cp, cp_cam, *_):
    ret = structs.CarState()

    # ... existing code ...

    # Read STAR button (add after other button reading code)
    if self.CP.flags & HyundaiFlags.CANFD:
        star_button = cp.vl["STEERING_BUTTONS"]["STAR_BUTTON"]
        if star_button == 0xe5 and self.prev_star_button != 0xe5:
            # Button just pressed! Trigger your action here
            print("STAR button pressed!")
            # TODO: Set a flag or trigger custom action
        self.prev_star_button = star_button

    return ret
```

Don't forget to initialize in `__init__`:

```python
def __init__(self, CP, FPCP):
    super().__init__(CP, FPCP)
    # ... existing code ...
    self.prev_star_button = 0xdf  # Initialize to released state
```

### Option 2: In FrogPilot Process

Add to `frogpilot/frogpilot_process.py`:

```python
from opendbc.can.parser import CANParser

class FrogPilotProcess:
    def __init__(self):
        # ... existing code ...

        # Add STAR button to parser
        if self.CP.carFingerprint == CAR.HYUNDAI_IONIQ_6:
            signals = [("STAR_BUTTON", "STEERING_BUTTONS")]
            checks = [("STEERING_BUTTONS", 50)]
            self.star_parser = CANParser("hyundai_canfd_generated", signals, checks, 1)
            self.prev_star_button = 0xdf

    def update(self):
        # ... existing code ...

        # Check STAR button
        if hasattr(self, 'star_parser'):
            for msg in sm['can']:
                if msg.src == 1:
                    self.star_parser.update(msg.address, msg.src, bytes(msg.dat), msg.logMonoTime)

            star_button = self.star_parser.vl["STEERING_BUTTONS"]["STAR_BUTTON"]
            if star_button == 0xe5 and self.prev_star_button != 0xe5:
                # Button pressed - do something!
                self.on_star_button_pressed()
            self.prev_star_button = star_button

    def on_star_button_pressed(self):
        # Your custom action here!
        # Examples:
        # - Cycle through drive modes
        # - Toggle experimental mode
        # - Adjust following distance
        # - Enable/disable custom FrogPilot feature
        print("STAR button pressed - triggering custom action!")
```

### Option 3: Simple Example - Toggle Experimental Mode

```python
def on_star_button_pressed(self):
    # Toggle experimental mode on STAR button press
    current_mode = self.params.get_bool("ExperimentalMode")
    self.params.put_bool("ExperimentalMode", not current_mode)
    print(f"Experimental Mode: {'ON' if not current_mode else 'OFF'}")
```

## Common Use Cases

1. **Cycle Drive Modes**: Press STAR to cycle Normal → Eco → Sport
2. **Toggle Features**: Enable/disable specific FrogPilot features
3. **Adjust Settings**: Increase/decrease follow distance, speed offsets, etc.
4. **Quick Actions**: One-press shortcuts for common tasks

## Notes

- The button value may briefly show other values (0xd8, 0xec, 0xd1, etc.) due to checksums/counters
- Always compare to the exact values 0xe5 (pressed) and 0xdf (released)
- Use edge detection (check `prev_star_button != current_value`) to avoid repeated triggers
- The button is on ECAN (Bus 1) for Ioniq 6 with LKA steering variant
