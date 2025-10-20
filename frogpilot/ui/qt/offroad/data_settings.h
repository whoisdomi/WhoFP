#pragma once

#include "common/params.h"
#include "frogpilot/ui/qt/widgets/frogpilot_controls.h"

class FrogPilotSettingsWindow;

class FrogPilotDataPanel : public FrogPilotListWidget {
  Q_OBJECT

public:
  explicit FrogPilotDataPanel(FrogPilotSettingsWindow *parent);

private:
  FrogPilotSettingsWindow *parent;

  Params params;
};
