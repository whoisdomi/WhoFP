# Hyundai Ioniq 6 (2023) Tuning Parameters

**Configuration:** CAN FD with taco_tune_hack = True

---

## Current Values

| Parameter | File | Original Values | Current Values |
|-----------|------|-----------------|----------------|
| STEER_MAX | opendbc_repo/opendbc/car/hyundai/values.py | 384 | **400** |
| STEER_DRIVER_ALLOWANCE | opendbc_repo/opendbc/car/hyundai/values.py | 250 | **350** |
| STEER_DRIVER_MULTIPLIER | opendbc_repo/opendbc/car/hyundai/values.py | 3 | **2** |
| STEER_DELTA_UP | opendbc_repo/opendbc/car/hyundai/values.py | 3 | **3** (< 15 m/s) / **1** (>= 15 m/s) |
| STEER_DELTA_DOWN | opendbc_repo/opendbc/car/hyundai/values.py | 4 | **3** (< 15 m/s) / **2** (>= 15 m/s) |
| max_torque | opendbc_repo/opendbc/safety/modes/hyundai_canfd.h | 270 | **720** |
| max_rt_delta | opendbc_repo/opendbc/safety/modes/hyundai_canfd.h | 112 | **150** |
| max_rate_up | opendbc_repo/opendbc/safety/modes/hyundai_canfd.h | 3 | **10** |
| max_rate_down | opendbc_repo/opendbc/safety/modes/hyundai_canfd.h | 7 | **10** |
| driver_torque_allowance | opendbc_repo/opendbc/safety/modes/hyundai_canfd.h | 250 | **350** |
| driver_torque_multiplier | opendbc_repo/opendbc/safety/modes/hyundai_canfd.h | 2 | **3** |
| MAX_RATE_UP | opendbc_repo/opendbc/safety/tests/test_hyundai_canfd.py | 3 | **10** |
| MAX_RATE_DOWN | opendbc_repo/opendbc/safety/tests/test_hyundai_canfd.py | 7 | **10** |
| MAX_TORQUE_LOOKUP | opendbc_repo/opendbc/safety/tests/test_hyundai_canfd.py | [0], [270] | **[0], [720]** |
| MAX_RT_DELTA | opendbc_repo/opendbc/safety/tests/test_hyundai_canfd.py | 112 | **150** |
| DRIVER_TORQUE_ALLOWANCE | opendbc_repo/opendbc/safety/tests/test_hyundai_canfd.py | 250 | **350** |
| DRIVER_TORQUE_FACTOR | opendbc_repo/opendbc/safety/tests/test_hyundai_canfd.py | 2 | **3** |
| MAX_CURVATURE | selfdrive/controls/lib/drive_helpers.py | 0.2 | **0.4** |
| MAX_LATERAL_ACCEL_NO_ROLL | selfdrive/controls/lib/drive_helpers.py | 3 | **4.0** |
| LAT_ACCEL_FACTOR | opendbc_repo/opendbc/car/torque_data/override.toml | 2.5 | **4.2** |
| MAX_LAT_ACCEL_MEASURED | opendbc_repo/opendbc/car/torque_data/override.toml | 2.5 | **3.0** |
| FRICTION | opendbc_repo/opendbc/car/torque_data/override.toml | 0.005 | **0.18** |
| LOW_SPEED_X | selfdrive/controls/lib/latcontrol_torque.py | [0,10,20,30] | **[0, 15, 25, 35]** |
| LOW_SPEED_Y | selfdrive/controls/lib/latcontrol_torque.py | [15,13,10,5] | **[25, 10, 1, 0.5]** |
| UNWIND_MULTIPLIER | selfdrive/controls/lib/latcontrol_torque.py | N/A | **0.95** |
| freeze_integrator | selfdrive/controls/lib/latcontrol_torque.py | < 5 m/s | **< 0.1 m/s** |
| kp | opendbc_repo/opendbc/car/interfaces.py | - | **1.0** |
| ki | opendbc_repo/opendbc/car/interfaces.py | - | **0.3** |
| kf | opendbc_repo/opendbc/car/interfaces.py | - | **1.0** |

---

## PID Parameters Explained

| Parameter | Description |
|-----------|-------------|
| **kp** (proportional) | Reacts to current error - how far off are we right now? Higher = more aggressive correction |
| **ki** (integral) | Reacts to accumulated error over time - corrects persistent offset. Can cause oscillation if too high |
| **kf** (feedforward) | Predicts needed torque based on desired lateral acceleration. Proactive, not reactive. Higher = more aggressive initial response |
| **UNWIND_MULTIPLIER** | Decays integrator when steering returns to center (error opposes integrator). 0.95 = 5% decay per cycle. Prevents overshoot after turns |

---

## Recent Changes

### High-Speed Wobble Fix
- `LOW_SPEED_Y`: Changed to `[25, 10, 1, 0.5]` - reduced high-speed correction factor
- `STEER_DELTA_UP` (high speed): `2` -> `1` - slower torque ramp
- `STEER_DELTA_DOWN` (high speed): `3` -> `2` - more gradual torque release

### Steering Unwind Fix
- Added `UNWIND_MULTIPLIER = 0.95` to `latcontrol_torque.py`
- Added unwind logic to `common/pid.py` - decays integrator when error opposes current integrator value

---

## Notes

- **LOW_SPEED_Y** values get squared in the calculation, so even small values at high speed have significant impact
- **FRICTION** at 0.18 is quite high compared to stock (0.005) - can cause hunting/oscillation
- **UNWIND_MULTIPLIER** at 0.95 means ~99% decay in 1 second when unwinding at 100Hz
- If unwind is still too slow, try lowering UNWIND_MULTIPLIER to 0.90 (10% decay per cycle)
