#pragma once

#include "frogpilot/ui/qt/offroad/frogpilot_settings.h"

class FrogPilotLateralPanel : public FrogPilotListWidget {
  Q_OBJECT

public:
  explicit FrogPilotLateralPanel(FrogPilotSettingsWindow *parent, bool forceOpen = false);

signals:
  void openSubPanel();

protected:
  void showEvent(QShowEvent *event) override;

private:
  void updateMetric(bool metric, bool bootRun);
  void updateState(const UIState &s);
  void updateToggles();

  bool forceOpenDescriptions;
  bool started;

  std::map<QString, AbstractControl*> toggles;

  QSet<QString> advancedLateralTuneKeys = {"ForceAutoTune", "ForceAutoTuneOff", "ForceTorqueController", "LatSmoothSeconds", "SteerDelay", "SteerFriction", "SteerKi", "SteerKP", "SteerLatAccel", "SteerRatio"};
  QSet<QString> advancedTurnDesiresKeys = {"TurnLatSmooth", "TurnLeftBiasPercent", "TurnRightBiasPercent", "PostTurnSmoothingTime", "LowSpeedTurnAssist", "LowSpeedTurnMinSpeed"};
  QSet<QString> aolKeys = {"AlwaysOnLateralLKAS", "PauseAOLOnBrake"};
  QSet<QString> laneChangeKeys = {"LaneChangeDuration", "LaneChangeJerkResponse", "LaneChangeLateralAccel", "LaneChangeTime", "LaneDetectionWidth", "MinimumLaneChangeSpeed", "NudgelessLaneChange", "OneLaneChange"};
  QSet<QString> lateralTuneKeys = {"NNFF", "NNFFLite", "TurnDesires"};
  QSet<QString> qolKeys = {"PauseLateralSpeed"};

  QSet<QString> parentKeys;

  FrogPilotParamValueButtonControl *steerDelayToggle;
  FrogPilotParamValueButtonControl *steerFrictionToggle;
  FrogPilotParamValueButtonControl *steerKiToggle;
  FrogPilotParamValueButtonControl *steerKPToggle;
  FrogPilotParamValueButtonControl *steerLatAccelToggle;
  FrogPilotParamValueButtonControl *steerRatioToggle;
  FrogPilotParamValueButtonControl *latSmoothSecondsToggle;

  FrogPilotSettingsWindow *parent;

  Params params;
};
