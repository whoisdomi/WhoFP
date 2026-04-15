# AGENTS.md

This file provides guidance to AI agents when working with code in this repository.

## Project Overview

This is **FrogPilot**, a community fork of openpilot. openpilot provides driver assistance (ACC, LKAS, FCW, LDW) for 300+ cars. FrogPilot adds Always On Lateral, Conditional Experimental Mode, custom themes, speed limit controller, and enhanced tuning options.

**Target Hardware:** comma 3/3X
**Target Car:** 2023 Ioniq 6 (assume this when no car is specified)

## Build Commands

```bash
scons -j$(nproc)              # Build C++ and Cython extensions
release/build_release.sh      # Release build (stripped/optimized)
release/build_devel.sh        # Development build
```

## Testing

```bash
pytest                             # Run all tests
pytest path/to/test_file.py        # Run specific file
pytest path/to/test_file.py::name  # Run specific test
pytest -m 'not slow'               # Skip slow tests
pytest -n auto                     # Run in parallel (default)
pytest -m 'tici'                   # Hardware tests (C3/C3X only)
```

## Linting

```bash
scripts/lint/lint.sh              # All lint checks
scripts/lint/lint.sh --fast       # Skip mypy and codespell
scripts/lint/lint.sh ruff mypy    # Specific checks only
scripts/lint/lint.sh --skip mypy  # Skip specific checks
```

**Linters:** ruff (format + lint), mypy (type checking), codespell (spell check)

## Code Style

- **Line length:** 160 characters
- **Indentation:** 2 spaces
- **Python version:** 3.11+
- **Imports:** Fully qualified (`openpilot.selfdrive`, `openpilot.common`, `openpilot.system`)
- **Testing:** pytest (not unittest)
- **Time:** `time.monotonic` (not `time.time`)

## Architecture

| Directory | Purpose |
|-----------|---------|
| `frogpilot/` | FrogPilot-specific features (controls, UI, navigation) |
| `selfdrive/controls/` | Lateral/longitudinal control (controlsd, plannerd, radard) |
| `selfdrive/modeld/` | Neural network inference |
| `selfdrive/car/` | Vehicle-specific implementations |
| `system/` | System services (manager, camerad, loggerd, hardware, ui) |
| `cereal/` | Cap'n Proto message definitions — use reserved slots in `custom.capnp` for fork additions |
| `common/` | Shared utilities |
| `opendbc_repo/` | CAN bus definitions and car-specific tuning data |

### Messaging (cereal/msgq)

```python
import cereal.messaging as messaging
sm = messaging.SubMaster(['sensorEvents'])
sm.update()
# Publish:
pm = messaging.PubMaster(['sensorEvents'])
dat = messaging.new_message('sensorEvents', size=1)
pm.send('sensorEvents', dat)
```

- All fields use SI units unless specified in the field name
- Events have `logMonoTime` and `valid` fields

### Submodules (do not modify these directly)

`panda/` · `opendbc_repo/` · `msgq_repo/` · `rednose_repo/` · `tinygrad_repo/`

## Key Processes

| Process | Role |
|---------|------|
| `controlsd` | Main control loop (steering, acceleration, braking) |
| `modeld` | Neural network model inference |
| `plannerd` | Motion planning |
| `radard` | Radar/sensor fusion |
| `card` | Vehicle CAN communication |
| `camerad` | Camera capture and preprocessing |

## FrogPilot Branches

| Branch | Status |
|--------|--------|
| `FrogPilot` | Main release |
| `FrogPilot-Staging` | Beta |
| `FrogPilot-Testing` | Alpha |
| `FrogPilot-Development` | Unstable dev |

## Domain-Specific References

For detailed checklists and variable references, see the subdirectory AGENTS.md files:
- **UI settings:** `frogpilot/ui/AGENTS.md`
- **Lateral tuning:** `frogpilot/controls/AGENTS.md`

> **Maintenance rule:** When modifying any tuning variable, default value, or file path related to lateral control — update `frogpilot/controls/AGENTS.md` and its identical siblings `CLAUDE.md` and `GEMINI.md` in the same directory.
