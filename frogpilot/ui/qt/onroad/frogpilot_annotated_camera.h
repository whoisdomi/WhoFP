#pragma once

#include "selfdrive/ui/qt/onroad/buttons.h"
#include "selfdrive/ui/qt/widgets/cameraview.h"

const int widget_size = img_size + (UI_BORDER_SIZE / 2);

struct RadarTrackData {
  QPointF calibrated_point;
};

class FrogPilotAnnotatedCameraWidget : public QWidget {
  Q_OBJECT

public:
  explicit FrogPilotAnnotatedCameraWidget(QWidget *parent = 0);

  void mousePressEvent(QMouseEvent *mouseEvent) override;
  void paintAdjacentPaths(QPainter &p, SubMaster &sm, SubMaster &fpsm);
  void paintBlindSpotPath(QPainter &p, SubMaster &sm);
  void paintFrogPilotWidgets(QPainter &p, UIState &s, SubMaster &sm);
  void paintLeadMetrics(QPainter &p, bool adjacent, QPointF *chevron, const cereal::RadarState::LeadData::Reader &lead_data);
  void paintPathEdges(QPainter &p, SubMaster &sm);
  void paintRainbowPath(QPainter &p, QLinearGradient &bg, float lin_grad_point);
  void updateState(const UIState &s, const FrogPilotUIState &fs);

  bool hideBottomIcons = false;
  bool isCruiseSet = false;
  bool rightHandDM = false;

  int alertHeight = 0;
  int speedLimitHeight = 0;
  int standstillDuration = 0;

  float speed = 0.0f;

  std::vector<RadarTrackData> radar_tracks;

  FrogPilotUIScene frogpilot_scene;

  QColor blueColor(int alpha = 255) { return QColor(0, 0, 255, alpha); }
  QColor purpleColor(int alpha = 255) { return QColor(128, 0, 128, alpha); }
  QColor whiteColor(int alpha = 255) { return QColor(255, 255, 255, alpha); }

  QJsonObject frogpilot_toggles;

  QPoint dmIconPosition;
  QPoint experimentalButtonPosition;

  QPolygonF track_adjacent_vertices[2];
  QPolygonF track_edge_vertices;
  QPolygonF track_vertices;

  QRect adjacentLeadTextRect;
  QRect leadTextRect;
  QRect setSpeedRect;

  QSize defaultSize;

  QString signalStyle;

protected:
  void showEvent(QShowEvent *event) override;

private:
  void paintCEMStatus(QPainter &p, SubMaster &sm);
  void paintCompass(QPainter &p);
  void paintCurveSpeedControl(QPainter &p, SubMaster &fpsm);
  void paintCurveSpeedControlTraining(QPainter &p, SubMaster &fpsm);
  void paintForceStop(QPainter &p, SubMaster &fpsm);
  void paintManualStop(QPainter &p, SubMaster &fpsm);
  void paintLateralPaused(QPainter &p);
  void paintLongitudinalPaused(QPainter &p);
  void paintPedalIcons(QPainter &p, SubMaster &sm, SubMaster &fpsm);
  void paintPendingSpeedLimit(QPainter &p, SubMaster &fpsm);
  void paintRadarTracks(QPainter &p);
  void paintRoadName(QPainter &p);
  void paintSpeedLimit(QPainter &p);
  void paintSpeedLimitSources(QPainter &p, SubMaster &fpsm);
  void paintStandstillTimer(QPainter &p);
  void paintStoppingPoint(QPainter &p, SubMaster &sm);
  void paintTurnSignals(QPainter &p, SubMaster &sm);
  void paintWeather(QPainter &p, SubMaster &fpsm);
  void updateSignals();

  bool speedLimitChanged = false;

  // Cached values to avoid parsing in paint methods (20Hz)
  int cachedBearing = 0;
  QString cachedRoadName;

  // Cached toggle bools — updated in updateState(), used in paint at 20Hz
  bool toggleCemStatus = false;
  bool toggleCompass = false;
  bool toggleCscStatus = false;
  bool togglePedalsOnUi = false;
  bool toggleRadarTracks = false;
  bool toggleRoadNameUi = false;
  bool toggleHideSpeedLimit = false;
  bool toggleShowSpeedLimits = false;
  bool toggleSpeedLimitController = false;
  bool toggleSpeedLimitSources = false;
  bool toggleShowStoppingPoint = false;
  bool toggleBlindSpotPath = false;
  bool toggleAdjacentPathMetrics = false;
  bool toggleShowStoppingPointMetrics = false;
  bool toggleShowSpeedLimitOffset = false;
  bool toggleSpeedLimitVienna = false;
  bool toggleSlcPriorityMode = false;
  double toggleLaneDetectionWidth = 3.5;

  int animationFrameIndex = 0;
  int desiredFollowDistance = 0;
  int frogHopCount = 0;
  int signalAnimationLength = 0;
  int signalHeight = 0;
  int signalMovement = 0;
  int signalWidth = 0;
  int totalFrames = 0;

  float distanceConversion = 1.0f;
  float setSpeed = 0.0f;
  float speedConversion = 1.0f;
  float speedConversionMetrics = 1.0f;
  float speedLimit = 0.0f;

  Params params;
  Params params_memory{"", true};

  QColor blackColor(int alpha = 255) { return QColor(0, 0, 0, alpha); }
  QColor redColor(int alpha = 255) { return QColor(201, 34, 49, alpha); }

  QElapsedTimer glowTimer;
  QElapsedTimer pendingLimitTimer;
  QElapsedTimer standstillTimer;

  QPixmap brakePedalImg;
  QPixmap curveSpeedIcon;
  QPixmap curveSpeedIconFlipped;  // Pre-cached flipped version
  QPixmap dashboardIcon;
  QPixmap dashboardIconScaled;
  QPixmap forceStopImg;
  QPixmap manualStopImg;
  QPixmap mapDataIcon;
  QPixmap mapDataIconScaled;
  QPixmap mapboxIcon;
  QPixmap mapboxIconScaled;
  QPixmap nextMapsIcon;
  QPixmap nextMapsIconScaled;
  QPixmap gasPedalImg;
  QPixmap gasPedalImgScaled;
  QPixmap pausedIcon;
  QPixmap speedIcon;
  QPixmap stopSignImg;
  QPixmap turnIcon;

  QPoint cemStatusPosition;
  QPoint compassPosition;
  QPoint lateralPausedPosition;

  QRect newSpeedLimitRect;
  QRect speedLimitRect;

  QSharedPointer<QMovie> cemCurveIcon;
  QSharedPointer<QMovie> cemLeadIcon;
  QSharedPointer<QMovie> cemSpeedIcon;
  QSharedPointer<QMovie> cemStopIcon;
  QSharedPointer<QMovie> cemTurnIcon;
  QSharedPointer<QMovie> chillModeIcon;
  QSharedPointer<QMovie> experimentalModeIcon;
  QSharedPointer<QMovie> weatherClearDay;
  QSharedPointer<QMovie> weatherClearNight;
  QSharedPointer<QMovie> weatherLowVisibility;
  QSharedPointer<QMovie> weatherRain;
  QSharedPointer<QMovie> weatherSnow;

  QString leadDistanceUnit;
  QString leadSpeedUnit;
  QString speedLimitOffsetStr;
  QString speedUnit;

  QTimer *animationTimer;

  QVector<QPixmap> blindspotImages;
  QVector<QPixmap> blindspotImagesFlipped;  // Pre-cached flipped versions
  QVector<QPixmap> signalImages;
  QVector<QPixmap> signalImagesFlipped;  // Pre-cached flipped versions
};
