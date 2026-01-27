# STAR Button Integration Guide - 2023 Hyundai Ioniq 6

## What Was Added

Added STAR button (CUSTOM_BUTTON) detection for your 2023 Hyundai Ioniq 6:
- **Bus**: 1 (ECAN)
- **Message ID**: 1096 (0x448) - "STEERING_WHEEL_MEDIA_BUTTONS"
- **Signal**: "CUSTOM_BUTTON" at bit 44 (byte 5, bit 4)
- **Pressed**: 1
- **Released**: 0

## Files Modified

1. `opendbc_repo/opendbc/dbc/generator/hyundai/hyundai_canfd.dbc`
   - Added `BO_ 1096 STEERING_WHEEL_MEDIA_BUTTONS` message
   - Added all steering wheel media button signals including `CUSTOM_BUTTON`

## Testing on Comma Device

### Step 1: Rebuild opendbc

SSH into your comma device and run:

```bash
cd /data/openpilot/opendbc_repo
scons -j4
```

### Step 2: Test the button (one-liner)

```bash
cd /data/openpilot && python3 -c "
import cereal.messaging as messaging

sm = messaging.SubMaster(['can'])

print('Testing STAR button - Press to verify!')
prev_bit = None
press_count = 0

while True:
    sm.update(100)
    if sm.updated['can']:
        for msg in sm['can']:
            if msg.src == 1 and msg.address == 1096:
                custom_button = (msg.dat[5] >> 4) & 0x01
                if custom_button != prev_bit and prev_bit is not None:
                    if custom_button == 1:
                        press_count += 1
                        print(f'{press_count:3d} PRESSED')
                    else:
                        print(f'{press_count:3d} RELEASED')
                prev_bit = custom_button
"
```

Press Ctrl+C to exit.

## Available Steering Wheel Buttons

All buttons on message 1096 (0x448):
- `VOICE_BUTTON` - bit 16
- `PHONE_BUTTON` - bit 18
- `MODE_BUTTON` - bit 22
- `RIGHT_SCROLL_PRESS` - bit 24
- `NEXT_BUTTON` - bit 26
- `PREVIOUS_BUTTON` - bit 28
- `MENU_BUTTON` - bit 34
- `LEFT_SCROLL_PRESS` - bit 38
- `CUSTOM_BUTTON` - bit 44 (STAR button)

## Using in FrogPilot Code

### Option 1: In carstate.py (Read button state)

Add to `opendbc_repo/opendbc/car/hyundai/carstate.py`:

```python
def __init__(self, CP, FPCP):
    super().__init__(CP, FPCP)
    # ... existing code ...
    self.prev_star_button = 0  # Initialize to released state

def update(self, cp, cp_cam, *_):
    ret = structs.CarState()

    # ... existing code ...

    # Read STAR button (CUSTOM_BUTTON on steering wheel)
    if self.CP.flags & HyundaiFlags.CANFD:
        star_button = cp.vl["STEERING_WHEEL_MEDIA_BUTTONS"]["CUSTOM_BUTTON"]
        if star_button == 1 and self.prev_star_button == 0:
            # Button just pressed! Trigger your action here
            print("STAR button pressed!")
            # TODO: Set a flag or trigger custom action
        self.prev_star_button = star_button

    return ret
```

And add the signal to the CAN parser signals list:

```python
signals = [
    # ... existing signals ...
    ("CUSTOM_BUTTON", "STEERING_WHEEL_MEDIA_BUTTONS"),
]

checks = [
    # ... existing checks ...
    ("STEERING_WHEEL_MEDIA_BUTTONS", 50),
]
```

### Option 2: In FrogPilot Process

Add to `frogpilot/frogpilot_process.py`:

```python
from opendbc.can.parser import CANParser

class FrogPilotProcess:
    def __init__(self):
        # ... existing code ...

        # Add STAR button to parser for Ioniq 6
        if self.CP.carFingerprint == CAR.HYUNDAI_IONIQ_6:
            signals = [("CUSTOM_BUTTON", "STEERING_WHEEL_MEDIA_BUTTONS")]
            checks = [("STEERING_WHEEL_MEDIA_BUTTONS", 50)]
            self.star_parser = CANParser("hyundai_canfd_generated", signals, checks, 1)
            self.prev_star_button = 0

    def update(self):
        # ... existing code ...

        # Check STAR button
        if hasattr(self, 'star_parser'):
            for msg in sm['can']:
                if msg.src == 1:
                    self.star_parser.update(msg.address, msg.src, bytes(msg.dat), msg.logMonoTime)

            star_button = self.star_parser.vl["STEERING_WHEEL_MEDIA_BUTTONS"]["CUSTOM_BUTTON"]
            if star_button == 1 and self.prev_star_button == 0:
                # Button pressed - do something!
                self.on_star_button_pressed()
            self.prev_star_button = star_button

    def on_star_button_pressed(self):
        # Your custom action here!
        print("STAR button pressed - triggering custom action!")
```

### Option 3: Simple Example - Cycle Drive Modes

```python
def on_star_button_pressed(self):
    # Cycle through drive modes: Normal -> Eco -> Sport
    current_mode = self.params.get("CurrentDriveMode", encoding='utf-8')

    if current_mode == "NORMAL":
        new_mode = "ECO"
    elif current_mode == "ECO":
        new_mode = "SPORT"
    else:
        new_mode = "NORMAL"

    self.params.put("CurrentDriveMode", new_mode)
    print(f"Drive mode changed to: {new_mode}")
```

## Common Use Cases

1. **Cycle Drive Modes**: Press STAR to cycle Normal → Eco → Sport
2. **Toggle Experimental Mode**: Quick enable/disable of experimental mode
3. **Adjust Following Distance**: Cycle through 1-4 bar settings
4. **Toggle Custom Features**: Enable/disable FrogPilot features on the fly
5. **Quick Settings Menu**: Open custom settings overlay

## Notes

- The button signal is a clean boolean: 1 when pressed, 0 when released
- Always use edge detection (`prev_star_button == 0 and current == 1`) to trigger actions only on press, not while holding
- The button is on ECAN (Bus 1) for all Hyundai CAN-FD vehicles with LKA steering
- Message 1096 is sent at 50 Hz, so button presses are detected quickly
- All other steering wheel media buttons are also available in the same message if you want to use them
