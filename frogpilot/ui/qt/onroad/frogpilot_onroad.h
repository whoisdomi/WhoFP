#pragma once

#include "selfdrive/ui/qt/onroad/annotated_camera.h"

class FrogPilotOnroadWindow : public QWidget {
  Q_OBJECT

public:
  FrogPilotOnroadWindow(QWidget* parent = 0);

  void updateState(const UIState &s, const FrogPilotUIState &fs);

  double fps;

  const FrogPilotUIScene *frogpilot_scene = nullptr;

  QColor bg;

  const QJsonObject *frogpilot_toggles = nullptr;

private:
  void paintEvent(QPaintEvent *event);
  void paintFPS(QPainter &p);
  void paintSteeringTorqueBorder(QPainter &p);
  void paintTurnSignalBorder(QPainter &p);
  void resizeEvent(QResizeEvent *event);

  bool blindSpotLeft;
  bool blindSpotRight;
  bool flickerActive;
  bool showBlindspot;
  bool showFPS;
  bool showSignal;
  bool showSteering;
  bool turnSignalLeft;
  bool turnSignalRight;

  float torque;

  QRect rect;

  QRegion marginRegion;

  QTimer *signalTimer;
};
