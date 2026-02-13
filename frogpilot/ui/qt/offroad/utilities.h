#pragma once

#include "frogpilot/ui/qt/offroad/frogpilot_settings.h"

class FrogPilotUtilitiesPanel : public FrogPilotListWidget {
  Q_OBJECT

public:
  explicit FrogPilotUtilitiesPanel(FrogPilotSettingsWindow *parent, bool forceOpen = false);

protected:
  void showEvent(QShowEvent *event) override;

private:
  bool forceOpenDescriptions;

  ButtonControl *pondButton;

  FrogPilotSettingsWindow *parent;

  Params params;
  Params params_memory{"", true};

  QNetworkAccessManager *networkManager;

  QTimer *pairingPollTimer;

  std::set<std::string> excluded_keys = {
    "AvailableModels", "AvailableModelNames", "FrogPilotStats",
    "GithubSshKeys", "GithubUsername", "MapBoxRequests",
    "ModelDrivesAndScores", "OverpassRequests", "SpeedLimits",
    "SpeedLimitsFiltered", "UpdaterAvailableBranches",
  };
};
