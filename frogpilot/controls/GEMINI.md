# Lateral Tuning Reference

> **Maintenance rule:** If you discover a new tuning variable, change a default value, find a new file path, or learn a new tuning tip from testing — update this file as part of the same task. Also update the identical files at `frogpilot/controls/CLAUDE.md` and `frogpilot/controls/AGENTS.md`.

## PID Controller
**File:** `common/pid.py` · Set in `opendbc_repo/opendbc/car/interfaces.py:301-303`
**Runtime update:** `selfdrive/controls/controlsd.py` (via `self.LaC.pid._k_p`, `self.LaC.pid._k_i`)

| Variable | Default | Description |
|----------|---------|-------------|
| `kp` | 1.0 | Proportional gain — immediate response to error |
| `ki` | 0.3 | Integral gain — corrects persistent offset (causes windup) |
| `kf` | 1.0 | Feedforward gain — predicts needed torque |
| `unwind_multiplier` | 1.0 | Integrator decay when unwinding (0.95 = 5% decay/cycle) |

## Torque Controller
**File:** `selfdrive/controls/lib/latcontrol_torque.py`

| Variable | Default | Description |
|----------|---------|-------------|
| `LOW_SPEED_X` | [0, 15, 25, 35] | Speed breakpoints (m/s) |
| `LOW_SPEED_Y` | [15, 13, 10, 5] | Correction factors (squared in calculation) |
| `UNWIND_MULTIPLIER` | 0.95 | Integrator decay rate when steering unwinds |

## Model Output Smoothing
**File:** `selfdrive/modeld/modeld.py`

| Variable | Default | Description |
|----------|---------|-------------|
| `LAT_SMOOTH_SECONDS` | 0.1 | Curvature target smoothing time |
| `LONG_SMOOTH_SECONDS` | 0.3 | Acceleration smoothing time |
| `MIN_LAT_CONTROL_SPEED` | 0.3 | Below this, curvature doesn't update |

## Car-Specific Torque Data
**File:** `opendbc_repo/opendbc/car/torque_data/override.toml`
**Format:** `"CAR_NAME" = [LAT_ACCEL_FACTOR, MAX_LAT_ACCEL_MEASURED, FRICTION]`

| Variable | Description |
|----------|-------------|
| `LAT_ACCEL_FACTOR` | Torque needed per unit lateral acceleration |
| `MAX_LAT_ACCEL_MEASURED` | Maximum measured lateral acceleration |
| `FRICTION` | Steering friction compensation |

## Hyundai CAN-FD Steering Limits
**File:** `opendbc_repo/opendbc/car/hyundai/values.py`

| Variable | Stock | Modified | Description |
|----------|-------|----------|-------------|
| `STEER_MAX` | 384 | 400 | Maximum steering torque |
| `STEER_DRIVER_ALLOWANCE` | 250 | 350 | Driver override threshold |
| `STEER_DRIVER_MULTIPLIER` | 3 | 2 | Driver torque multiplier |
| `STEER_DELTA_UP` | 3 | 3/2 (speed dep.) | Torque increase rate |
| `STEER_DELTA_DOWN` | 4 | 3/2 (speed dep.) | Torque decrease rate |

## Hyundai CAN-FD Safety Limits
**File:** `opendbc_repo/opendbc/safety/modes/hyundai_canfd.h`

| Variable | Stock | Modified | Description |
|----------|-------|----------|-------------|
| `max_torque` | 270 | 720 | Maximum allowed torque |
| `max_rt_delta` | 112 | 150 | Real-time delta limit |
| `max_rate_up` | 3 | 10 | Max torque increase rate |
| `max_rate_down` | 7 | 10 | Max torque decrease rate |
| `driver_torque_allowance` | 250 | 350 | Driver override allowance |
| `driver_torque_multiplier` | 2 | 3 | Driver torque factor |

## Curvature Limits
**File:** `selfdrive/controls/lib/drive_helpers.py`

| Variable | Default | Description |
|----------|---------|-------------|
| `MAX_CURVATURE` | 0.2 | Maximum path curvature |
| `MAX_LATERAL_ACCEL_NO_ROLL` | 3.0 | Max lateral accel without roll compensation |

## Tuning Tips

**High-speed wobble/oscillation:** lower `LOW_SPEED_Y` at higher speeds · lower `FRICTION` · lower `kp`/`kf` · reduce `STEER_DELTA_UP/DOWN`

**Poor turn unwind (overshoot):** lower `ki` · lower `unwind_multiplier` · lower `FRICTION` · lower `LAT_SMOOTH_SECONDS`

**Sluggish response:** increase `kp` · increase `STEER_DELTA_UP` · lower `LAT_SMOOTH_SECONDS`

**Persistent lane offset:** increase `ki` (watch for turn overshoot)
