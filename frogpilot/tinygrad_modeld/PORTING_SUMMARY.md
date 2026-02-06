# Tinygrad Modeld Port Summary

## Overview

Ported StarPilot/Dom's tinygrad_modeld (v8-v12 model support with off-policy) from the NewDo branch into FrogPilot's FP-Testing branch (0.10.x). The core challenge was API incompatibilities between the older NewDo codebase and the current FP-Testing codebase.

---

## How It Was Before (Stock FP-Testing)

### Single modeld Process

Before this port, FP-Testing had **one modeld** at `selfdrive/modeld/modeld.py` that handled everything:

- **Process config** (`system/manager/process_config.py`):
  ```python
  PythonProcess("modeld", "selfdrive.modeld.modeld", only_onroad)
  ```
  Just `only_onroad` - it always ran when driving, no model version switching.

- **No tinygrad_modeld folder** existed under `frogpilot/`. The `frogpilot/tinygrad_modeld/` directory did not exist.

- **No tinygrad_modeld SConscript** - `selfdrive/SConscript` only had:
  ```
  SConscript(['modeld/SConscript'])
  ```

### Stock modeld Architecture (`selfdrive/modeld/modeld.py`)

The stock modeld already used tinygrad pkl files (vision + policy), but with key differences from Dom's version:

- **Hardcoded model paths**: `VISION_PKL_PATH`, `POLICY_PKL_PATH` etc. pointed to `selfdrive/modeld/models/` for the default "bd2" model, with custom models in `/data/models/`.
- **`InputQueues` class**: Used `InputQueues` for buffering desire/features with `desire_pulse` key name. Dom's version uses manual numpy buffer management with different key names (e.g. `desire` or `desire_init` instead of `desire_pulse`).
- **`get_action_from_model`**: Simpler function - only used `get_curvature_from_plan()` (yaw-based curvature). No `mlsim`/`is_v9` flags, no `desired_curvature` output support, no `planplus` support. Had FrogPilot-specific ATD (Advanced Turn Desires) logic and lane position offset.
- **`get_accel_from_plan`**: Direct import, not the `_tomb_raider` alias.
- **Off-policy**: Had basic off-policy support (loads if v12 or metadata exists) but with simpler input mirroring (`desire_pulse`, `features_buffer`, `traffic_convention` only).
- **No multi-generation support**: No `policy_generation`, `is_v11`, `is_v9`, `mlsim` flags. Treated all models the same way.
- **No `lateral_control_params`**: Stock modeld didn't pass `lateral_control_params` or `prev_desired_curv` inputs to the policy model.
- **Parser**: Used `Parser()` from `selfdrive/modeld/parse_model_outputs.py` which had `is_mhp()`, `parse_vision_outputs()`, `parse_policy_outputs()` but no `split_outputs()` and no `ignore_missing` support.
- **PubMaster**: Published to `["modelV2", "drivingModelData", "cameraOdometry"]`, then extended with `frogpilotModelV2` via `pm.extend()`.
- **SubMaster**: Did NOT subscribe to `frogpilotPlan` initially, extended via `sm.extend()`.
- **`fill_model_msg`**: Stock version at `selfdrive/modeld/fill_model_msg.py` had `lane_position_offset` parameter, initialized `laneLines` with 4 entries (not 6), and had no `temporalPose` handling (field already deprecated in schema).
- **`LAT_SMOOTH_SECONDS = 0.1`**: Stock used 0.1s lateral smoothing. Dom's version uses 0.0s.
- **USBGPU support**: Stock modeld had `USBGPU` env var support for AMD USB GPU. Dom's version doesn't.

### frogpilot_variables.py Model Section (Before)

Before the port, the model section was simpler:
```python
toggle.model = self.params.get("Model") or DEFAULT_MODEL
toggle.model_name = DEFAULT_MODEL_NAME if toggle.model == DEFAULT_MODEL else toggle.model
toggle.model_version = self.params.get("ModelVersion") or DEFAULT_MODEL_VERSION
```

No `ModelVersions` list lookup, no `toggle.classic_model`, no `toggle.tinygrad_model`, no `toggle.tomb_raider`. Just a direct read of `Model` and `ModelVersion` params.

### drive_helpers.py (Before)

No `get_accel_from_plan_tomb_raider` alias existed. The function was just `get_accel_from_plan`.

---

## What Was Changed (The Port)

### 1. Copied Folders from NewDo Branch
- **`frogpilot/tinygrad_modeld/`** - Dom's entire tinygrad model runner including:
  - `tinygrad_modeld.py` - Main inference loop with multi-generation support
  - `fill_model_msg.py` - Message builder with 6 lane lines, temporalPose, sim_pose
  - `parse_model_outputs.py` - Parser with `split_outputs()`, `ignore_missing`, adaptive MHP detection
  - `constants.py` - Same constants as stock plus `MODEL_FREQ`, `HISTORY_FREQ`, `TEMPORAL_SKIP`, buffer length constants
  - `get_model_metadata.py` - Metadata extractor for ONNX models
  - `models/` - ONNX files (driving_vision, driving_policy, driving_off_policy, dmonitoring_model)
  - `models/commonmodel.cc`, `commonmodel_pyx.pyx` - Cython OpenCL frame processing
  - `transforms/` - OpenCL transform/loadyuv kernels
  - `runners/tinygrad_helpers.py` - QCOM GPU tensor helper
  - `SConscript` - Build rules for Cython, tinygrad compilation, metadata extraction
- **`tinygrad_repo/`** - The tinygrad ML inference framework (git submodule)

### 2. Process Switching (`system/manager/process_config.py`)

**Before:**
```python
PythonProcess("modeld", "selfdrive.modeld.modeld", only_onroad),
```

**After:**
```python
def run_tinygrad_modeld(started, params, CP, frogpilot_toggles):
  return started and getattr(frogpilot_toggles, 'tinygrad_model', False)

def run_stock_modeld(started, params, CP, frogpilot_toggles):
  return started and not getattr(frogpilot_toggles, 'tinygrad_model', False)

# In procs list:
PythonProcess("modeld", "selfdrive.modeld.modeld", run_stock_modeld),
PythonProcess("tinygrad_modeld", "frogpilot.tinygrad_modeld.tinygrad_modeld", run_tinygrad_modeld),
```

Only ONE modeld runs at a time. When `toggle.tinygrad_model` is True (v8-v12), Dom's tinygrad_modeld runs. Otherwise, stock modeld runs.

### 3. Model Version Lookup (`frogpilot/common/frogpilot_variables.py`)

**Before:**
```python
toggle.model = self.params.get("Model") or DEFAULT_MODEL
toggle.model_version = self.params.get("ModelVersion") or DEFAULT_MODEL_VERSION
```

**After:** NewDo-style lookup using three comma-separated param lists:
- `AvailableModels` - model IDs (e.g. `"bd2,space-lab,mypilot"`)
- `AvailableModelNames` - display names
- `ModelVersions` - version per model (e.g. `"v11,v9,v12"`)

Looks up the selected model's index in `AvailableModels`, then reads the corresponding version from `ModelVersions`. Falls back to direct `ModelVersion` param if lists aren't available.

Added three new toggles:
```python
toggle.classic_model = toggle.model_version in {"v1", "v2", "v3", "v4"}
toggle.tinygrad_model = toggle.model_version in {"v8", "v9", "v10", "v11", "v12"}
toggle.tomb_raider = toggle.model == "space-lab"
```

### 4. Build System (`selfdrive/SConscript`)

**Before:**
```
SConscript(['modeld/SConscript'])
```

**After:**
```
SConscript(['modeld/SConscript'])
SConscript(['../frogpilot/tinygrad_modeld/SConscript'])
```

The tinygrad_modeld SConscript handles:
- Compiling Cython `commonmodel_pyx.so`
- Extracting model metadata from ONNX files to `_metadata.pkl`
- Compiling ONNX models to tinygrad pkl files via `tinygrad_repo/examples/openpilot/compile3.py`
- Conditional off-policy compilation (only if `driving_off_policy.onnx` exists)

### 5. Compatibility Alias (`selfdrive/controls/lib/drive_helpers.py`)

**Added:**
```python
# Alias for tinygrad_modeld compatibility
get_accel_from_plan_tomb_raider = get_accel_from_plan
```

Dom's tinygrad_modeld imports `get_accel_from_plan_tomb_raider`. Rather than modifying Dom's code, we alias it.

### 6. API Fixes in `tinygrad_modeld.py` (Old API -> Current API)

These were runtime errors discovered one at a time on the comma device:

| What broke | Old (NewDo) | Fixed to | Error |
|---|---|---|---|
| Import path | `openpilot.selfdrive.car.car_helpers` | `opendbc.car.car_helpers` | ImportError |
| Params encoding | `params.get("Model", encoding="utf-8")` | `params.get("Model")` + manual `.decode()` | TypeError: unexpected kwarg 'encoding' |
| Car name field | `CP.carName` | `CP.brand` | AttributeError: no such member 'carName' |
| CarParams loading | `car.CarParams.from_bytes()` | `messaging.log_from_bytes()` | AttributeError: no 'from_bytes' |
| Missing toggle | `frogpilot_toggles.recovery_power` | `getattr(frogpilot_toggles, 'recovery_power', 1.0)` | AttributeError (preventive fix) |

### 7. Schema Fix in `fill_model_msg.py`

**Error:** `AttributeError: struct has no such member; name = temporalPose`

The capnp schema renamed `temporalPose` to `temporalPoseDEPRECATED`. Dom's fill_model_msg.py writes to `modelV2.temporalPose` for sim_pose or plan velocity/orientation data.

**Fix:** Wrapped the entire temporalPose block in `try/except AttributeError: pass`

---

## Key Differences: Stock modeld vs Dom's tinygrad_modeld

| Feature | Stock (`selfdrive/modeld/`) | Dom's (`frogpilot/tinygrad_modeld/`) |
|---|---|---|
| Multi-generation | No | Yes (v8/v9/v10/v11/v12 flags) |
| `desired_curvature` output | No (derives from yaw) | Yes (v9+ direct output) |
| `mlsim` mode | No | Yes (v8/v10/v11/v12) |
| `planplus` / off-policy blend | No | Yes (`recovery_power` scaling) |
| Input buffering | `InputQueues` class | Manual numpy with `TEMPORAL_SKIP` |
| Desire key | `desire_pulse` | Dynamic (`desire`, `desire_init`, etc.) |
| `lateral_control_params` | Not passed | Passed if model expects it |
| `prev_desired_curv` | Not passed | Passed with history buffer |
| Lane lines | 4 | 6 (4 raw + 2 averaged) |
| Lateral smoothing | 0.1s | 0.0s |
| ATD / lane offset | Yes (in get_action) | No |
| Parser | Basic | `split_outputs()`, `ignore_missing` |
| PubMaster setup | Init 3, extend 1 | Init all 4 at once |
| USBGPU | Yes | No |

---

## Model Architecture

Three model runners coexist:
- **Stock modeld** (`selfdrive/modeld/`) - Used when `toggle.tinygrad_model` is False
- **Tinygrad modeld** (`frogpilot/tinygrad_modeld/`) - Used when `toggle.tinygrad_model` is True (v8-v12)
- **Classic THNEED** - v1-v4 (legacy, not actively used)

The tinygrad modeld runs up to three models per frame:
1. **Vision model** (`driving_vision_tinygrad.pkl`) - processes camera images, outputs features + hidden_state
2. **Policy model** (`driving_policy_tinygrad.pkl`) - takes features + desire + context, outputs plan/actions
3. **Off-policy model** (`driving_off_policy_tinygrad.pkl`) - optional, outputs `planplus` corrections

---

## New Model Compatibility

### File Naming Convention
Downloaded models in `/data/models/` must follow:
- `{model_id}_driving_vision_tinygrad.pkl`
- `{model_id}_driving_policy_tinygrad.pkl`
- `{model_id}_driving_vision_metadata.pkl`
- `{model_id}_driving_policy_metadata.pkl`
- (Optional) `{model_id}_driving_off_policy_tinygrad.pkl` + metadata

The default "bd2" model uses built-in files from `frogpilot/tinygrad_modeld/models/` directly (no prefix).

### Version Routing
New models need their version set correctly in the `ModelVersions` param (comma-separated, matching `AvailableModels` order). The version determines which runner loads the model:
- v1-v4: stock modeld (classic/THNEED mode)
- v5-v7: stock modeld
- v8-v12: tinygrad_modeld (Dom's version)

### Adaptive Parser
The parser (`parse_model_outputs.py`) handles variable architectures dynamically:
- Checks for keys like `desired_curvature`, `planplus`, `lat_planner_solution` conditionally
- Detects MHP (multi-hypothesis prediction) vs non-MHP from output tensor shapes
- `ignore_missing=True` mode for off-policy parser (not all outputs present)
- Different model versions (v8/v9/v10/v11/v12) work without code changes

---

## Files Modified

| File | Change |
|---|---|
| `system/manager/process_config.py` | Added `run_tinygrad_modeld` + `run_stock_modeld` gates, changed modeld entry, added tinygrad_modeld process |
| `frogpilot/common/frogpilot_variables.py` | NewDo-style model version lookup with `ModelVersions`/`AvailableModels` lists, added `classic_model`/`tinygrad_model`/`tomb_raider` toggles |
| `selfdrive/controls/lib/drive_helpers.py` | Added `get_accel_from_plan_tomb_raider = get_accel_from_plan` alias |
| `selfdrive/SConscript` | Added `SConscript(['../frogpilot/tinygrad_modeld/SConscript'])` |
| `frogpilot/tinygrad_modeld/tinygrad_modeld.py` | 5 API compatibility fixes (import path, params encoding, CP.brand, CarParams loading, recovery_power) |
| `frogpilot/tinygrad_modeld/fill_model_msg.py` | Wrapped temporalPose in try/except for deprecated schema field |

## Files Added (Copied from NewDo)

| File/Directory | Purpose |
|---|---|
| `frogpilot/tinygrad_modeld/` (entire directory) | Dom's tinygrad model runner |
| `tinygrad_repo/` | Tinygrad ML framework (submodule) |
