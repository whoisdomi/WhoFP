# New UI Component Checklist

When adding a new FrogPilot UI setting, modify these files **in order**:

## 1. Parameter Definition
**File:** `common/params_keys.h`
```cpp
{"ParamName", {PERSISTENT, FLOAT, "default", "default", tuning_level}},
```
- Types: `BOOL`, `INT`, `FLOAT`, `STRING`, `JSON`
- Tuning levels: 0=Minimal, 1=Standard, 2=Advanced, 3=Developer

## 2. Toggle Definition
**File:** `frogpilot/common/frogpilot_variables.py`
```python
toggle.paramName = self.get_value("ParamName", cast=float, condition=parent_toggle, default=0.0, min=0, max=1.0)
```
- Add under the appropriate section (e.g., `advanced_lateral_tuning`)
- Access values elsewhere via `frogpilot_toggles.paramName`

## 3. UI Control Definition
**File:** `frogpilot/ui/qt/offroad/<section>_settings.cc`

Add to the toggles vector:
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

## 4. Header Declaration
**File:** `frogpilot/ui/qt/offroad/<section>_settings.h`

Add the param key to the appropriate key set — **CRITICAL: without this the toggle won't appear**:
```cpp
QSet<QString> advancedLateralTuneKeys = {"...", "ParamName", "..."};
```

Add the toggle pointer:
```cpp
FrogPilotParamValueButtonControl *paramToggle;
```

## 5. Code Integration
Update the code that uses the value to read from `frogpilot_toggles`:
- `selfdrive/controls/controlsd.py` — real-time steering/control values
- `selfdrive/modeld/modeld.py` — model-related values

## Live Updates
Settings update live while driving when:
1. UI sets `FrogPilotTogglesUpdated = True`
2. Process checks: `if sm['frogpilotPlan'].togglesUpdated: frogpilot_toggles = get_frogpilot_toggles()`
