from openpilot.common.params import Params
from openpilot.selfdrive.ui.mici.widgets.button import BigButton, BigParamControl
from openpilot.selfdrive.ui.mici.widgets.dialog import BigMultiOptionDialog, BigConfirmationDialog
from openpilot.system.ui.lib.application import gui_app
from openpilot.system.ui.widgets.scroller import NavScroller


def _float_options(min_val: float, max_val: float, step: float) -> list[str]:
  options = []
  v = min_val
  while v <= max_val + step * 0.01:
    options.append(f"{v:.1f}")
    v += step
  return options


CSC_MANUAL_OPTIONS = _float_options(1.0, 4.0, 0.1)


class CurveSpeedLayoutMici(NavScroller):
  def __init__(self):
    super().__init__()
    self._params = Params()
    self._params_memory = Params(memory=True)

    self._csc_btn = BigParamControl("curve speed controller", "CurveSpeedController",
                                    toggle_callback=lambda _: self._refresh())

    self._calibrated_btn = BigButton("calibrated lateral accel", "")

    self._manual_enabled_btn = BigParamControl("manual lateral accel", "CSCManualLateralAccelerationEnabled",
                                               toggle_callback=lambda _: self._refresh())

    self._manual_value_btn = BigButton("lateral accel value", "")
    self._manual_value_btn.set_click_callback(self._show_manual_value_picker)

    self._reset_btn = BigButton("reset curve data", "")
    self._reset_btn.set_click_callback(self._confirm_reset)

    self._status_btn = BigParamControl("status widget", "ShowCSCStatus")

    self._scroller.add_widgets([
      self._csc_btn,
      self._calibrated_btn,
      self._manual_enabled_btn,
      self._manual_value_btn,
      self._reset_btn,
      self._status_btn,
    ])

  def show_event(self):
    super().show_event()
    self._refresh()

  def _update_state(self):
    super()._update_state()
    self._refresh()

  def _refresh(self):
    csc_on = self._params.get_bool("CurveSpeedController")
    calibration_progress = self._params_memory.get_float("CalibrationProgress", return_default=True, default=0.0)
    calibrated = calibration_progress > 0
    manual_enabled = self._params.get_bool("CSCManualLateralAccelerationEnabled")

    self._calibrated_btn.set_visible(csc_on)
    cal_val = self._params_memory.get_float("CalibratedLateralAcceleration", return_default=True, default=2.0)
    self._calibrated_btn.set_value(f"{cal_val:.2f} m/s²")

    self._manual_enabled_btn.set_visible(csc_on and calibrated)
    self._manual_value_btn.set_visible(csc_on and calibrated and manual_enabled)

    manual_val = self._params.get_float("CSCManualLateralAcceleration", return_default=True, default=2.0)
    self._manual_value_btn.set_value(f"{manual_val:.1f} m/s²")

    self._reset_btn.set_visible(csc_on)
    self._status_btn.set_visible(csc_on)

  def _show_manual_value_picker(self):
    current = f"{self._params.get_float('CSCManualLateralAcceleration', return_default=True, default=2.0):.1f}"
    if current not in CSC_MANUAL_OPTIONS:
      current = "2.0"

    dialog_holder: dict = {}

    def on_confirm():
      selected = dialog_holder["dialog"].get_selected_option()
      try:
        self._params.put_float("CSCManualLateralAcceleration", float(selected))
      except ValueError:
        pass
      self._refresh()

    dialog = BigMultiOptionDialog(options=CSC_MANUAL_OPTIONS, default=current, right_btn_callback=on_confirm)
    dialog_holder["dialog"] = dialog
    gui_app.push_widget(dialog)

  def _confirm_reset(self):
    def on_confirm():
      self._params_memory.put_float("CalibratedLateralAcceleration", 2.0)
      self._params_memory.remove("CalibrationProgress")
      self._params.remove("CurvatureData")
      self._params.put_bool("CSCManualLateralAccelerationEnabled", False)
      self._refresh()

    icon = gui_app.texture("icons_mici/settings/network/new/trash.png", 54, 64)
    gui_app.push_widget(BigConfirmationDialog("slide to\nreset curve data", icon, on_confirm, red=True))


class StarPilotLayoutMici(NavScroller):
  def __init__(self):
    super().__init__()

    curve_panel = CurveSpeedLayoutMici()
    curve_btn = BigButton("curve speed", "")
    curve_btn.set_click_callback(lambda: gui_app.push_widget(curve_panel))

    self._scroller.add_widgets([
      curve_btn,
    ])
