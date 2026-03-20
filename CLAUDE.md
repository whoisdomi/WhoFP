# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is **FrogPilot**, a community fork of openpilot. openpilot is an open-source operating system for robotics that provides driver assistance (ACC, LKAS, FCW, LDW) for 300+ supported cars. FrogPilot adds features like Always On Lateral, Conditional Experimental Mode, custom themes, speed limit controller, and enhanced tuning options.

**Target Hardware:** comma 3/3X devices (also supports PC development)

**Target Car:** When a car is not specified, assume it is for a 2023 Ioniq 6

## Build Commands

```bash
# Build C++ and Cython extensions
scons -j$(nproc)

# Release build (stripped/optimized)
release/build_release.sh

# Development build
release/build_devel.sh
```

## Testing

```bash
# Run all tests
pytest

# Run specific test file
pytest path/to/test_file.py

# Run specific test
pytest path/to/test_file.py::test_name

# Skip slow tests
pytest -m 'not slow'

# Run tests in parallel (default)
pytest -n auto

# Run hardware-specific tests (C3/C3X only)
pytest -m 'tici'
```

## Linting

```bash
# Run all lint checks
scripts/lint/lint.sh

# Fast mode (skip mypy and codespell)
scripts/lint/lint.sh --fast

# Run specific checks only
scripts/lint/lint.sh ruff mypy

# Skip specific checks
scripts/lint/lint.sh --skip mypy codespell
```

**Linters used:** ruff (format + lint), mypy (type checking), codespell (spell check)

## Code Style

- **Line length:** 160 characters
- **Indentation:** 2 spaces
- **Python version:** 3.11+
- **Import style:** Use fully qualified imports (`openpilot.selfdrive`, `openpilot.common`, `openpilot.system`)
- **Testing framework:** pytest (not unittest)
- **Time functions:** Use `time.monotonic` (not `time.time`)

## Architecture

### Core Directories

- **selfdrive/** - Self-driving logic
  - `controls/` - Lateral/longitudinal control (controlsd, plannerd, radard)
  - `modeld/` - Neural network inference
  - `car/` - Vehicle-specific implementations
  - `monitoring/` - Driver monitoring

- **system/** - System services
  - `manager/` - Process orchestration
  - `camerad/` - Camera capture
  - `loggerd/` - Data logging
  - `hardware/` - Hardware abstraction
  - `ui/` - Qt-based UI

- **cereal/** - Message definitions (Cap'n Proto)
  - `log.capnp` - Event message schemas
  - `custom.capnp` - Fork-specific extensions (use reserved slots for compatibility)

- **frogpilot/** - FrogPilot-specific features
  - `controls/` - Enhanced control tuning
  - `ui/` - Themed UI components
  - `frogpilot_process.py` - Main FrogPilot process handler

- **common/** - Shared utilities
- **tools/** - Development and testing tools

### Messaging System (cereal)

Inter-process communication uses msgq (pub/sub) with Cap'n Proto serialization.

```python
import cereal.messaging as messaging

# Subscribe
sm = messaging.SubMaster(['sensorEvents'])
sm.update()
print(sm['sensorEvents'])

# Publish
pm = messaging.PubMaster(['sensorEvents'])
dat = messaging.new_message('sensorEvents', size=1)
pm.send('sensorEvents', dat)
```

**Conventions:**
- All fields use SI units unless specified in field name
- Events have `logMonoTime` and `valid` fields
- Fork customizations should use reserved slots in `custom.capnp` for backwards compatibility

### Submodules

External dependencies are git submodules:
- `panda/` - CAN interface
- `opendbc_repo/` - DBC CAN database definitions
- `msgq_repo/` - Message queue backend
- `rednose_repo/` - Extended Kalman filter
- `tinygrad_repo/` - ML inference

## Key Processes

- **controlsd** - Main control loop (steering, acceleration, braking)
- **modeld** - Neural network model inference
- **plannerd** - Motion planning
- **radard** - Radar/sensor fusion
- **card** - Vehicle CAN communication
- **camerad** - Camera capture and preprocessing

## FrogPilot Branches

| Branch | Description |
|--------|-------------|
| FrogPilot | Main release |
| FrogPilot-Staging | Beta with upcoming features |
| FrogPilot-Testing | Alpha with bleeding-edge features |
| FrogPilot-Development | Active development (unstable) |

## New UI Component Checklist

When adding a new FrogPilot UI setting, modify these files in order:

### 1. Parameter Definition
**File:** `common/params_keys.h`
```cpp
{"ParamName", {PERSISTENT, FLOAT, "default", "default", tuning_level}},
```
- Types: `BOOL`, `INT`, `FLOAT`, `STRING`, `JSON`
- Tuning levels: 0=Minimal, 1=Standard, 2=Advanced, 3=Developer

### 2. Toggle Definition
**File:** `frogpilot/common/frogpilot_variables.py`
```python
toggle.paramName = self.get_value("ParamName", cast=float, condition=parent_toggle, default=0.0, min=0, max=1.0)
```
- Add under appropriate section (e.g., `advanced_lateral_tuning`)
- Toggle values are accessed via `frogpilot_toggles.paramName` in other files

### 3. UI Control Definition
**File:** `frogpilot/ui/qt/offroad/<section>_settings.cc`

Add to `lateralToggles` vector:
```cpp
{"ParamName", tr("Display Name (Default: X.XX)"), tr("<b>Description.</b>"), "icon_path_or_empty"},
```

Add control handler:
```cpp
} else if (param == "ParamName") {
  std::vector<QString> paramButton{"Reset"};
  lateralToggle = new FrogPilotParamValueButtonControl(param, title, desc, icon, min, max, QString(), std::map<float, QString>(), step, false, {}, paramButton, false, false);
}
```

Add reset button handler:
```cpp
paramToggle = static_cast<FrogPilotParamValueButtonControl*>(toggles["ParamName"]);
QObject::connect(paramToggle, &FrogPilotParamValueButtonControl::buttonClicked, [this]() {
  if (FrogPilotConfirmationDialog::yesorno(tr("Reset <b>Param Name</b> to its default value?"), this)) {
    params.putFloat("ParamName", defaultValue);
    paramToggle->refresh();
  }
});
```

### 4. Header Declaration
**File:** `frogpilot/ui/qt/offroad/<section>_settings.h`

Add the param key to the appropriate panel key set (CRITICAL — without this, the toggle won't appear in the UI):
```cpp
QSet<QString> advancedLateralTuneKeys = {"...", "ParamName", "..."};
```

Add the toggle pointer declaration:
```cpp
FrogPilotParamValueButtonControl *paramToggle;
```

### 5. Code Integration
Update the actual code that uses the value to read from `frogpilot_toggles`:
- **controlsd.py** - For real-time steering/control values
- **modeld.py** - For model-related values
- Other relevant files

### Live Updates
Settings update live while driving when:
1. UI sets `FrogPilotTogglesUpdated = True`
2. Process checks: `if sm['frogpilotPlan'].togglesUpdated: frogpilot_toggles = get_frogpilot_toggles()`

## Driving Variables Reference

### Lateral Control (Steering)

#### PID Controller (`common/pid.py`)
| Variable | Default | Description |
|----------|---------|-------------|
| `kp` | 1.0 | Proportional gain - immediate response to error |
| `ki` | 0.3 | Integral gain - corrects persistent offset (causes windup) |
| `kf` | 1.0 | Feedforward gain - predicts needed torque |
| `unwind_multiplier` | 1.0 | Integrator decay when unwinding (0.95 = 5% decay/cycle) |

**Set in:** `opendbc_repo/opendbc/car/interfaces.py:301-303`
**Runtime update:** `selfdrive/controls/controlsd.py` (via `self.LaC.pid._k_p`, `self.LaC.pid._k_i`)

#### Torque Controller (`selfdrive/controls/lib/latcontrol_torque.py`)
| Variable | Default | Description |
|----------|---------|-------------|
| `LOW_SPEED_X` | [0, 15, 25, 35] | Speed breakpoints (m/s) |
| `LOW_SPEED_Y` | [15, 13, 10, 5] | Correction factors (squared in calculation) |
| `UNWIND_MULTIPLIER` | 0.95 | Integrator decay rate when steering unwinds |

#### Model Output Smoothing (`selfdrive/modeld/modeld.py`)
| Variable | Default | Description |
|----------|---------|-------------|
| `LAT_SMOOTH_SECONDS` | 0.1 | Curvature target smoothing time |
| `LONG_SMOOTH_SECONDS` | 0.3 | Acceleration smoothing time |
| `MIN_LAT_CONTROL_SPEED` | 0.3 | Below this, curvature doesn't update |

#### Car-Specific Torque Data (`opendbc_repo/opendbc/car/torque_data/override.toml`)
| Variable | Description |
|----------|-------------|
| `LAT_ACCEL_FACTOR` | Torque needed per unit lateral acceleration |
| `MAX_LAT_ACCEL_MEASURED` | Maximum measured lateral acceleration |
| `FRICTION` | Steering friction compensation |

**Format:** `"CAR_NAME" = [LAT_ACCEL_FACTOR, MAX_LAT_ACCEL_MEASURED, FRICTION]`

### Hyundai CAN-FD Specific

#### Steering Limits (`opendbc_repo/opendbc/car/hyundai/values.py`)
| Variable | Stock | Taco Tune | Description |
|----------|-------|-----------|-------------|
| `STEER_MAX` | 384 | 400 | Maximum steering torque |
| `STEER_DRIVER_ALLOWANCE` | 250 | 350 | Driver override threshold |
| `STEER_DRIVER_MULTIPLIER` | 3 | 2 | Driver torque multiplier |
| `STEER_DELTA_UP` | 3 | 3/2 (speed dep.) | Torque increase rate |
| `STEER_DELTA_DOWN` | 4 | 3/2 (speed dep.) | Torque decrease rate |

#### Safety Limits (`opendbc_repo/opendbc/safety/modes/hyundai_canfd.h`)
| Variable | Stock | Modified | Description |
|----------|-------|----------|-------------|
| `max_torque` | 270 | 720 | Maximum allowed torque |
| `max_rt_delta` | 112 | 150 | Real-time delta limit |
| `max_rate_up` | 3 | 10 | Max torque increase rate |
| `max_rate_down` | 7 | 10 | Max torque decrease rate |
| `driver_torque_allowance` | 250 | 350 | Driver override allowance |
| `driver_torque_multiplier` | 2 | 3 | Driver torque factor |

### Curvature Limits (`selfdrive/controls/lib/drive_helpers.py`)
| Variable | Default | Description |
|----------|---------|-------------|
| `MAX_CURVATURE` | 0.2 | Maximum path curvature |
| `MAX_LATERAL_ACCEL_NO_ROLL` | 3.0 | Max lateral accel without roll compensation |

### Tuning Tips

**High-speed wobble/oscillation:**
- Reduce `LOW_SPEED_Y` values at higher speeds
- Lower `FRICTION`
- Lower `kp` or `kf`
- Reduce `STEER_DELTA_UP/DOWN`

**Poor turn unwind (overshoot after turns):**
- Lower `ki` (less integrator buildup)
- Add/lower `unwind_multiplier` (faster integrator decay)
- Lower `FRICTION`
- Lower `LAT_SMOOTH_SECONDS` (faster target response)

**Sluggish steering response:**
- Increase `kp`
- Increase `STEER_DELTA_UP`
- Lower `LAT_SMOOTH_SECONDS`

**Persistent lane offset (crosswind, crowned roads):**
- Increase `ki` (but watch for turn overshoot)

---

## Project Structure

```
.
├── frogpilot/               # CUSTOM FORK LOGIC (High Priority)
│   ├── controls/            # Custom driving logic overrides (Longitudinal/Lateral)
│   ├── navigation/          # Custom navigation/map implementation
│   ├── ui/                  # Custom Qt UI elements and themes
│   ├── assets/              # Custom sounds, icons, and theme files
│   └── tools/               # FrogPilot-specific scripts
├── selfdrive/               # CORE OPENPILOT LOGIC
│   ├── car/                 # Vehicle interfaces (Fingerprinting, CAN parsing for Toyota, Honda, etc.)
│   ├── controls/            # Planning & Control (Planner, PID, MPC)
│   ├── locationd/           # Localization (GPS, IMU, Kalman Filters)
│   ├── modeld/              # Driving model runner
│   └── ui/                  # Standard Onroad UI (Qt/OpenGL)
├── system/                  # HARDWARE ABSTRACTION LAYER
│   ├── camerad/             # Camera drivers and image processing
│   ├── hardware/            # Tici/Comma 3X hardware interactions
│   ├── loggerd/             # Data logging (rlogs)
│   └── sensord/             # IMU and sensor management
├── opendbc_repo/            # CAN BUS DEFINITIONS
│   ├── opendbc/can/         # CAN parsing libraries
│   └── opendbc/dbc/         # .dbc files for all car makes
├── panda/                   # HARDWARE INTERFACE (STM32 firmware)
│   ├── board/               # Firmware code for the Panda
│   └── python/              # Python library for talking to Panda
├── cereal/                  # MESSAGING SPEC (Cap'n Proto)
│   └── messaging/           # IPC library for inter-process communication
├── tinygrad_repo/           # NEURAL NETWORK FRAMEWORK
│   └── tinygrad/            # Core tensor library used for models
├── tools/                   # UTILITIES
│   ├── cabana/              # CAN signal analysis tool
│   ├── replay/              # Tools to replay log files
│   └── sim/                 # Simulation bridge
└── scripts/                 # Setup and maintenance scripts

```
