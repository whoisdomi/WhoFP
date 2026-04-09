#include "frogpilot/ui/qt/onroad/frogpilot_annotated_camera.h"

#include <QCoreApplication>

#include "common/swaglog.h"
#include "common/util.h"
#include "common/watchdog.h"

volatile int fpWidgetPaintStage = 0;
volatile int fpUpdateStage = 0;

FrogPilotAnnotatedCameraWidget::FrogPilotAnnotatedCameraWidget(QWidget *parent) : QWidget(parent) {
  animationTimer = new QTimer(this);

  brakePedalImg = loadPixmap("../../frogpilot/assets/other_images/brake_pedal.png", {btn_size, btn_size});
  curveSpeedIcon = loadPixmap("../../frogpilot/assets/other_images/curve_speed.png", {btn_size, btn_size});
  curveSpeedIconFlipped = curveSpeedIcon.transformed(QTransform().scale(-1, 1));  // Pre-cache flipped version
  dashboardIcon = loadPixmap("../../frogpilot/assets/other_images/dashboard_icon.png", {btn_size / 2, btn_size / 2});
  forceStopImg = loadPixmap("../../frogpilot/assets/other_images/force_stop.png", {btn_size, btn_size});
  manualStopImg = loadPixmap("../../frogpilot/assets/other_images/manual_stop.png", {btn_size, btn_size});
  gasPedalImg = loadPixmap("../../frogpilot/assets/other_images/gas_pedal.png", {btn_size, btn_size});
  mapboxIcon = loadPixmap("../../frogpilot/assets/other_images/mapbox_icon.png", {btn_size / 2, btn_size / 2});
  mapDataIcon = loadPixmap("../../frogpilot/assets/other_images/offline_maps_icon.png", {btn_size / 2, btn_size / 2});
  nextMapsIcon = loadPixmap("../../frogpilot/assets/other_images/next_maps_icon.png", {btn_size / 2, btn_size / 2});

  // Pre-scale speed limit source icons to avoid SmoothTransformation at 20Hz
  int slcIconSize = img_size / 4;
  QSize slcSize(slcIconSize, slcIconSize);
  dashboardIconScaled = dashboardIcon.scaled(slcSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  gasPedalImgScaled = gasPedalImg.scaled(slcSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  mapboxIconScaled = mapboxIcon.scaled(slcSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  mapDataIconScaled = mapDataIcon.scaled(slcSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  nextMapsIconScaled = nextMapsIcon.scaled(slcSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  pausedIcon = loadPixmap("../../frogpilot/assets/other_images/paused_icon.png", {widget_size, widget_size});
  speedIcon = loadPixmap("../../frogpilot/assets/other_images/speed_icon.png", {widget_size, widget_size});
  stopSignImg = loadPixmap("../../frogpilot/assets/stuff/stop_sign2.png", {96, 96});
  turnIcon = loadPixmap("../../frogpilot/assets/other_images/turn_icon.png", {widget_size, widget_size});

  // GIF animations are lazy-loaded in updateState() when their toggle is enabled
  // to avoid ~40MB+ baseline memory from unused QMovie frame buffers

  QObject::connect(animationTimer, &QTimer::timeout, [this] {
    if (totalFrames > 0) {
      animationFrameIndex = (animationFrameIndex + 1) % totalFrames;
    }
  });
  QObject::connect(frogpilotUIState(), &FrogPilotUIState::themeUpdated, this, &FrogPilotAnnotatedCameraWidget::updateSignals);
  QObject::connect(uiState(), &UIState::offroadTransition, [this] {
    standstillTimer.invalidate();

    QJsonObject stats = QJsonDocument::fromJson(QString::fromStdString(params.get("FrogPilotStats")).toUtf8()).object();
    stats["FrogHops"] = stats.value("FrogHops").toInt(0) + frogHopCount;
    params.putNonBlocking("FrogPilotStats", QJsonDocument(stats).toJson(QJsonDocument::Compact).toStdString());

    frogHopCount = 0;
  });
}

void FrogPilotAnnotatedCameraWidget::showEvent(QShowEvent *event) {
  updateSignals();
}

void FrogPilotAnnotatedCameraWidget::updateSignals() {
  fpUpdateStage = 50;  // updateSignals start
  animationFrameIndex = 0;

  QVector<QPixmap>().swap(blindspotImages);
  QVector<QPixmap>().swap(blindspotImagesFlipped);
  QVector<QPixmap>().swap(signalImages);
  QVector<QPixmap>().swap(signalImagesFlipped);

  fpUpdateStage = 51;  // loading signal files
  bool isGif = false;

  watchdog_kick(nanos_since_boot());
  fpUpdateStage = 5101; // QDir entryInfoList
  QFileInfoList files = QDir("../../frogpilot/assets/active_theme/signals/").entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
  fpUpdateStage = 5102; // signal files loop
  for (const QFileInfo &fileInfo : files) {
    QString fileName = fileInfo.fileName();
    QString filePath = fileInfo.absoluteFilePath();

    if (fileName.endsWith(".gif", Qt::CaseInsensitive)) {
      isGif = true;

      QMovie movie(filePath);
      movie.setCacheMode(QMovie::CacheNone);
      movie.start();

      int frameCount = movie.frameCount();
      signalImages.reserve(frameCount);

      for (int i = 0; i < frameCount; ++i) {
        movie.jumpToFrame(i);

        QImage image = movie.currentPixmap().toImage().convertToFormat(QImage::Format_Indexed8);
        QPixmap frame = QPixmap::fromImage(image);
        signalImages.append(frame);

        // Kick watchdog every 10 frames to prevent SIGKILL during long GIF decode
        if (i % 10 == 0) {
          watchdog_kick(nanos_since_boot());
          QCoreApplication::processEvents(QEventLoop::AllEvents, 50); // Keep Wayland connection alive
        }
      }

      movie.stop();
    } else if (fileName.endsWith(".png", Qt::CaseInsensitive)) {
      QVector<QPixmap> &targetList = fileName.contains("blindspot", Qt::CaseInsensitive) ? blindspotImages : signalImages;
      targetList.append(QPixmap::fromImage(QImage(filePath).convertToFormat(QImage::Format_Indexed8)));
    } else {
      QStringList parts = fileName.split('_');
      if (parts.size() == 2) {
        signalStyle = parts[0];
        signalAnimationLength = parts[1].toInt();
      }
    }
  }

  if (!signalImages.isEmpty()) {
    QPixmap &firstImage = signalImages.front();
    signalHeight = firstImage.height();
    signalWidth = firstImage.width();
    totalFrames = signalImages.size();

    if (isGif && signalStyle == "traditional") {
      signalMovement = (width() + signalWidth * 2) / totalFrames;
      signalStyle = "traditional_gif";
    } else {
      signalMovement = 0;
    }
  } else {
    signalAnimationLength = 0;
    signalHeight = 0;
    signalMovement = 0;
    signalWidth = 0;
    totalFrames = 0;

    signalStyle = "None";
  }

  fpUpdateStage = 52;  // pre-cache flipped
  // Pre-cache flipped versions to avoid transforms at 20Hz during paint
  watchdog_kick(nanos_since_boot());
  QTransform flipTransform;
  flipTransform.scale(-1, 1);

  signalImagesFlipped.reserve(signalImages.size());
  for (int i = 0; i < signalImages.size(); ++i) {
    signalImagesFlipped.append(signalImages[i].transformed(flipTransform));
    if (i % 10 == 0) {
      watchdog_kick(nanos_since_boot());
      QCoreApplication::processEvents(QEventLoop::AllEvents, 50); // Keep Wayland connection alive
    }
  }

  blindspotImagesFlipped.reserve(blindspotImages.size());
  for (const QPixmap &img : blindspotImages) {
    blindspotImagesFlipped.append(img.transformed(flipTransform));
  }
  fpUpdateStage = 0;  // updateSignals done
}

void FrogPilotAnnotatedCameraWidget::updateState(const UIState &s, const FrogPilotUIState &fs) {
  fpUpdateStage = 30;  // updateState start
  const UIScene &scene = s.scene;

  const SubMaster &sm = *(s.sm);
  const SubMaster &fpsm = *(fs.sm);

  const cereal::CarState::Reader &carState = sm["carState"].getCarState();
  const cereal::FrogPilotPlan::Reader &frogpilotPlan = fpsm["frogpilotPlan"].getFrogpilotPlan();
  const cereal::FrogPilotSelfdriveState::Reader &frogpilotSelfdriveState = fpsm["frogpilotSelfdriveState"].getFrogpilotSelfdriveState();
  const cereal::SelfdriveState::Reader &selfdriveState = sm["selfdriveState"].getSelfdriveState();

  if (scene.is_metric || frogpilot_toggles->value("use_si_metrics").toBool()) {
    leadDistanceUnit = tr(" meters");
    leadSpeedUnit = frogpilot_toggles->value("use_si_metrics").toBool() ? tr(" m/s") : tr(" km/h");
    speedUnit = scene.is_metric ? tr("km/h") : tr("mph");

    distanceConversion = 1.0f;
    speedConversion = scene.is_metric ? MS_TO_KPH : MS_TO_MPH;
    speedConversionMetrics = frogpilot_toggles->value("use_si_metrics").toBool() ? 1.0f : MS_TO_KPH;
  } else {
    leadDistanceUnit = tr(" feet");
    leadSpeedUnit = tr(" mph");
    speedUnit = tr("mph");

    distanceConversion = METER_TO_FOOT;
    speedConversion = MS_TO_MPH;
    speedConversionMetrics = MS_TO_MPH;
  }

  fpUpdateStage = 31;  // cereal reads
  desiredFollowDistance = frogpilotPlan.getDesiredFollowDistance();

  hideBottomIcons = selfdriveState.getAlertSize() != cereal::SelfdriveState::AlertSize::NONE;
  hideBottomIcons |= frogpilotSelfdriveState.getAlertSize() != cereal::FrogPilotSelfdriveState::AlertSize::NONE;
  hideBottomIcons |= signalStyle.startsWith("traditional") && (carState.getLeftBlinker() || carState.getRightBlinker());

  speedLimit = frogpilotPlan.getSlcOverriddenSpeed() != 0 ? frogpilotPlan.getSlcOverriddenSpeed() : frogpilotPlan.getSlcSpeedLimit();
  speedLimitChanged = frogpilotPlan.getSpeedLimitChanged();
  unconfirmedSpeedLimitValid = frogpilotPlan.getUnconfirmedSlcSpeedLimit() > 1;
  if (frogpilotPlan.getSlcOverriddenSpeed() == 0 && !frogpilot_toggles->value("show_speed_limit_offset").toBool()) {
    speedLimit += frogpilotPlan.getSlcSpeedLimitOffset();
  }
  speedLimit *= (scene.is_metric ? MS_TO_KPH : MS_TO_MPH);
  float speedLimitOffset = frogpilotPlan.getSlcSpeedLimitOffset() * speedConversion;
  speedLimitOffsetStr = (speedLimitOffset != 0) ? QString::number(speedLimitOffset, 'f', 0).prepend((speedLimitOffset > 0) ? "+" : "-") : "–";

  if (frogpilot_scene->standstill && frogpilot_toggles->value("stopped_timer").toBool()) {
    if (!standstillTimer.isValid()) {
      standstillTimer.start();
    } else {
      standstillDuration = frogpilot_scene->started_timer / UI_FREQ < 60 ? 0 : standstillTimer.elapsed() / 1000;
    }
  } else {
    standstillDuration = 0;
    standstillTimer.invalidate();
  }

  static int lastFrameIndex;
  if (lastFrameIndex > animationFrameIndex && frogpilot_toggles->value("signal_icons").toString() == "frog") {
    frogHopCount++;
  }
  lastFrameIndex = animationFrameIndex;

  fpUpdateStage = 32;  // cache/params
  // Cache toggle bools to avoid JSON string lookups in paint methods (20Hz)
  toggleCemStatus = frogpilot_toggles->value("cem_status").toBool();
  toggleCompass = frogpilot_toggles->value("compass").toBool();
  toggleCscStatus = frogpilot_toggles->value("csc_status").toBool();
  togglePedalsOnUi = frogpilot_toggles->value("pedals_on_ui").toBool();
  toggleRadarTracks = frogpilot_toggles->value("radar_tracks").toBool();
  toggleRoadNameUi = frogpilot_toggles->value("road_name_ui").toBool();
  toggleHideSpeedLimit = frogpilot_toggles->value("hide_speed_limit").toBool();
  toggleShowSpeedLimits = frogpilot_toggles->value("show_speed_limits").toBool();
  toggleSpeedLimitController = frogpilot_toggles->value("speed_limit_controller").toBool();
  toggleSpeedLimitSources = frogpilot_toggles->value("speed_limit_sources").toBool();
  toggleShowStoppingPoint = frogpilot_toggles->value("show_stopping_point").toBool();
  toggleBlindSpotPath = frogpilot_toggles->value("blind_spot_path").toBool();
  toggleAdjacentPathMetrics = frogpilot_toggles->value("adjacent_path_metrics").toBool();
  toggleShowStoppingPointMetrics = frogpilot_toggles->value("show_stopping_point_metrics").toBool();
  toggleShowSpeedLimitOffset = frogpilot_toggles->value("show_speed_limit_offset").toBool();
  toggleSpeedLimitVienna = frogpilot_toggles->value("speed_limit_vienna").toBool();
  toggleSlcPriorityMode = frogpilot_toggles->value("slc_priority_mode").toBool();
  toggleLaneDetectionWidth = frogpilot_toggles->value("lane_detection_width").toDouble();

  // Lazy-load GIF animations only when their feature is enabled
  if (toggleCemStatus) {
    if (!cemCurveIcon) {
      LOGW("Lazy-loading CEM GIFs (7 animations)");
      fpUpdateStage = 33; // load CEM GIFs
      watchdog_kick(nanos_since_boot());
      loadGif("../../frogpilot/assets/other_images/curve_icon.gif", cemCurveIcon, QSize(widget_size, widget_size), this);
      watchdog_kick(nanos_since_boot());
      loadGif("../../frogpilot/assets/other_images/lead_icon.gif", cemLeadIcon, QSize(widget_size, widget_size), this);
      watchdog_kick(nanos_since_boot());
      loadGif("../../frogpilot/assets/other_images/speed_icon.gif", cemSpeedIcon, QSize(widget_size, widget_size), this);
      watchdog_kick(nanos_since_boot());
      loadGif("../../frogpilot/assets/other_images/light_icon.gif", cemStopIcon, QSize(widget_size, widget_size), this);
      watchdog_kick(nanos_since_boot());
      loadGif("../../frogpilot/assets/other_images/turn_icon.gif", cemTurnIcon, QSize(widget_size, widget_size), this);
      watchdog_kick(nanos_since_boot());
      loadGif("../../frogpilot/assets/other_images/chill_mode_icon.gif", chillModeIcon, QSize(widget_size, widget_size), this);
      watchdog_kick(nanos_since_boot());
      loadGif("../../frogpilot/assets/other_images/experimental_mode_icon.gif", experimentalModeIcon, QSize(widget_size, widget_size), this);
      fpUpdateStage = 32; // back to cache/params
    }
  } else if (cemCurveIcon) {
    LOGW("Freeing CEM GIFs (toggle disabled)");
    cemCurveIcon.reset();
    cemLeadIcon.reset();
    cemSpeedIcon.reset();
    cemStopIcon.reset();
    cemTurnIcon.reset();
    chillModeIcon.reset();
    experimentalModeIcon.reset();
  }

  int weatherId = frogpilotPlan.getWeatherId();
  if (weatherId != 0) {
    if (!weatherClearDay) {
      LOGW("Lazy-loading weather GIFs (5 animations, weatherId=%d)", weatherId);
      fpUpdateStage = 34; // load Weather GIFs
      loadGif("../../frogpilot/assets/other_images/weather_clear_day.gif", weatherClearDay, QSize(widget_size, widget_size), this);
      loadGif("../../frogpilot/assets/other_images/weather_clear_night.gif", weatherClearNight, QSize(widget_size, widget_size), this);
      loadGif("../../frogpilot/assets/other_images/weather_low_visibility.gif", weatherLowVisibility, QSize(widget_size, widget_size), this);
      loadGif("../../frogpilot/assets/other_images/weather_rain.gif", weatherRain, QSize(widget_size, widget_size), this);
      loadGif("../../frogpilot/assets/other_images/weather_snow.gif", weatherSnow, QSize(widget_size, widget_size), this);
      fpUpdateStage = 32; // back to cache/params
    }
  } else if (weatherClearDay) {
    LOGW("Freeing weather GIFs (weatherId=0)");
    weatherClearDay.reset();
    weatherClearNight.reset();
    weatherLowVisibility.reset();
    weatherRain.reset();
    weatherSnow.reset();
  }

  // Cache values — only re-read once per second (not 20Hz) to avoid heap churn
  static int paramReadCounter = 0;
  if (++paramReadCounter >= UI_FREQ) {
    paramReadCounter = 0;
    if (toggleCompass) {
      double rawBearing = QJsonDocument::fromJson(QByteArray::fromStdString(params_memory.get("LastGPSPosition"))).object().value("bearing").toDouble(0.0);
      cachedBearing = qRound(fmod(rawBearing + 360.0, 360.0));
    }
    if (toggleRoadNameUi) {
      cachedRoadName = QString::fromStdString(params_memory.get("RoadName"));
    }
  }

  fpUpdateStage = 0;  // updateState done
  // Note: no update() needed here — painting is done via paintFrogPilotWidgets()
  // called from AnnotatedCameraWidget::paintEvent, which is already triggered
  // by camera frames and the UI timer. Calling update() here just schedules
  // a redundant no-op repaint (this widget has no paintEvent override).
}

void FrogPilotAnnotatedCameraWidget::mousePressEvent(QMouseEvent *mouseEvent) {
  if (speedLimitChanged && unconfirmedSpeedLimitValid && speedLimitRect.contains(mouseEvent->pos())) {
    params_memory.putBool("SpeedLimitAccepted", true);
    mouseEvent->accept();
    return;
  }

  mouseEvent->ignore();
}

void FrogPilotAnnotatedCameraWidget::paintFrogPilotWidgets(QPainter &p, UIState &s, SubMaster &sm) {
  FrogPilotUIState *fs = frogpilotUIState();

  SubMaster &fpsm = *(fs->sm);

  const cereal::CarState::Reader &carState = sm["carState"].getCarState();
  const cereal::FrogPilotCarState::Reader &frogpilotCarState = fpsm["frogpilotCarState"].getFrogpilotCarState();
  const cereal::FrogPilotPlan::Reader &frogpilotPlan = fpsm["frogpilotPlan"].getFrogpilotPlan();

  fpWidgetPaintStage = 1; // CEM
  if (!hideBottomIcons && toggleCemStatus) {
    paintCEMStatus(p, sm);
  } else {
    cemStatusPosition.setX(0);
    cemStatusPosition.setY(0);
  }

  fpWidgetPaintStage = 2; // Compass
  if (!hideBottomIcons && toggleCompass) {
    paintCompass(p);
  } else {
    compassPosition.setX(0);
    compassPosition.setY(0);
  }

  fpWidgetPaintStage = 3; // CSC
  if (frogpilotPlan.getForcingStop()) {
    paintForceStop(p, fpsm);
  } else if (frogpilotCarState.getManualStopAhead()) {
    paintManualStop(p, fpsm);
  } else if (!frogpilotPlan.getSpeedLimitChanged() && !(signalStyle == "static" && carState.getLeftBlinker()) && toggleCscStatus) {
    if (frogpilotPlan.getCscTraining()) {
      paintCurveSpeedControlTraining(p, fpsm);
    } else {
      glowTimer.invalidate();

      if (isCruiseSet && frogpilotPlan.getCscControllingSpeed()) {
        paintCurveSpeedControl(p, fpsm);
      }
    }
  } else {
    glowTimer.invalidate();
  }

  fpWidgetPaintStage = 4; // LatPaused
  if (!hideBottomIcons && frogpilotCarState.getPauseLateral()) {
    paintLateralPaused(p);
  } else {
    lateralPausedPosition.setX(0);
    lateralPausedPosition.setY(0);
  }

  fpWidgetPaintStage = 5; // LongPaused
  if (!hideBottomIcons && (frogpilotCarState.getForceCoast() || frogpilotCarState.getPauseLongitudinal())) {
    paintLongitudinalPaused(p);
  }

  fpWidgetPaintStage = 6; // Pedals
  if (togglePedalsOnUi) {
    paintPedalIcons(p, sm, fpsm);
  }

  fpWidgetPaintStage = 7; // PendingLimit
  if (speedLimitChanged && unconfirmedSpeedLimitValid) {
    if (!pendingLimitTimer.isValid()) {
      pendingLimitTimer.start();
    }
  } else {
    pendingLimitTimer.invalidate();
  }

  fpWidgetPaintStage = 8; // RadarTracks
  if (toggleRadarTracks) {
    paintRadarTracks(p);
  }

  fpWidgetPaintStage = 9; // RoadName
  if (toggleRoadNameUi) {
    paintRoadName(p);
  }

  fpWidgetPaintStage = 10; // SpeedLimit
  bool hideSpeedLimit = !frogpilotPlan.getSpeedLimitChanged() && toggleHideSpeedLimit;
  if (!hideSpeedLimit && (toggleShowSpeedLimits || toggleSpeedLimitController)) {
    paintSpeedLimit(p);
  } else {
    speedLimitHeight = 0;
  }

  fpWidgetPaintStage = 11; // SpeedLimitSources
  if (toggleSpeedLimitSources) {
    paintSpeedLimitSources(p, fpsm);
  }

  fpWidgetPaintStage = 12; // StandstillTimer
  if (standstillDuration != 0 && frogpilot_scene->started_timer / UI_FREQ >= 60) {
    paintStandstillTimer(p);
  }

  fpWidgetPaintStage = 13; // StoppingPoint
  if (track_vertices.length() >= 1 && frogpilotPlan.getRedLight() && toggleShowStoppingPoint) {
    paintStoppingPoint(p, sm);
  }

  fpWidgetPaintStage = 14; // TurnSignals
  if ((carState.getLeftBlinker() || carState.getRightBlinker()) && signalStyle != "None") {
    if (!animationTimer->isActive()) {
      animationTimer->start(signalAnimationLength);
    }
    paintTurnSignals(p, sm);
  } else if (animationTimer->isActive()) {
    animationTimer->stop();
  }

  fpWidgetPaintStage = 15; // Weather
  if (!hideBottomIcons) {
    paintWeather(p, fpsm);
  }

  fpWidgetPaintStage = 0;
}

void FrogPilotAnnotatedCameraWidget::paintAdjacentPaths(QPainter &p, SubMaster &sm, SubMaster &fpsm) {
  const cereal::CarState::Reader &carState = sm["carState"].getCarState();
  const cereal::FrogPilotPlan::Reader &frogpilotPlan = fpsm["frogpilotPlan"].getFrogpilotPlan();

  for (int i = 0; i < 2; ++i) {
    bool isLeft = (i == 0);
    bool isBlindSpot = isLeft ? carState.getLeftBlindspot() : carState.getRightBlindspot();

    float laneWidth = isLeft ? frogpilotPlan.getLaneWidthLeft() : frogpilotPlan.getLaneWidthRight();

    if (laneWidth == 0.0f) {
      continue;
    }

    p.save();

    // Use static gradients to avoid reconstructing QLinearGradient every frame
    static QLinearGradient blindSpotGradient(0, 0, 0, 0);
    static bool blindSpotGradientInit = false;
    if (!blindSpotGradientInit) {
      blindSpotGradient.setColorAt(0.0f, QColor::fromHslF(0.0f, 0.75f, 0.5f, 0.6f));
      blindSpotGradient.setColorAt(0.5f, QColor::fromHslF(0.0f, 0.75f, 0.5f, 0.4f));
      blindSpotGradient.setColorAt(1.0f, QColor::fromHslF(0.0f, 0.75f, 0.5f, 0.2f));
      blindSpotGradientInit = true;
    }

    QLinearGradient gradient(0, height(), 0, 0);
    if (isBlindSpot && toggleBlindSpotPath) {
      gradient = blindSpotGradient;
      gradient.setStart(0, height());
      gradient.setFinalStop(0, 0);
    } else {
      float ratio = std::clamp(laneWidth / toggleLaneDetectionWidth, 0.0, 1.0);
      float hue = (ratio * ratio) * (120.0f / 360.0f);

      gradient.setColorAt(0.0f, QColor::fromHslF(hue, 0.75f, 0.5f, 0.6f));
      gradient.setColorAt(0.5f, QColor::fromHslF(hue, 0.75f, 0.5f, 0.4f));
      gradient.setColorAt(1.0f, QColor::fromHslF(hue, 0.75f, 0.5f, 0.2f));
    }

    p.setBrush(gradient);
    p.drawPolygon(track_adjacent_vertices[i]);

    if (toggleAdjacentPathMetrics) {
      const QPolygonF &path = track_adjacent_vertices[i];
      if (path.isEmpty()) {
        p.restore();
        continue;
      }

      QString text;
      if (isBlindSpot && toggleBlindSpotPath) {
        text = tr("Vehicle in blind spot");
      } else {
        text = QString::number(laneWidth * distanceConversion, 'f', 2) + leadDistanceUnit;
      }

      int midIndex = path.size() / 2;
      QPointF anchorPoint = isLeft ? path[midIndex / 2] : path[midIndex + (path.size() - midIndex) / 2];

      p.setFont(InterFont(45, QFont::DemiBold));
      QFontMetrics metrics(p.font());

      int textXPosition = isLeft ? anchorPoint.x() - metrics.horizontalAdvance(text) - 10 : anchorPoint.x() + 10;
      int textYPosition = anchorPoint.y() - metrics.height() / 2 + metrics.ascent();

      // Drop shadow instead of expensive strokePath
      p.setPen(QColor(0, 0, 0, 200));
      p.drawText(textXPosition + 2, textYPosition + 2, text);
      p.setPen(whiteColor());
      p.drawText(textXPosition, textYPosition, text);
    }

    p.restore();
  }
}

void FrogPilotAnnotatedCameraWidget::paintBlindSpotPath(QPainter &p, SubMaster &sm) {
  const cereal::CarState::Reader &carState = sm["carState"].getCarState();

  p.save();

  static const QColor bsColor06 = QColor::fromHslF(0.0f, 0.75f, 0.5f, 0.6f);
  static const QColor bsColor04 = QColor::fromHslF(0.0f, 0.75f, 0.5f, 0.4f);
  static const QColor bsColor02 = QColor::fromHslF(0.0f, 0.75f, 0.5f, 0.2f);
  QLinearGradient bs(0, height(), 0, 0);
  bs.setColorAt(0.0f, bsColor06);
  bs.setColorAt(0.5f, bsColor04);
  bs.setColorAt(1.0f, bsColor02);
  p.setBrush(bs);

  if (track_adjacent_vertices[0].boundingRect().width() > 0 && carState.getLeftBlindspot()) {
    p.drawPolygon(track_adjacent_vertices[0]);
  }
  if (track_adjacent_vertices[1].boundingRect().width() > 0 && carState.getRightBlindspot()) {
    p.drawPolygon(track_adjacent_vertices[1]);
  }

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintCEMStatus(QPainter &p, SubMaster &sm) {
  if (dmIconPosition == QPoint(0, 0)) {
    return;
  }

  p.save();

  cemStatusPosition.setX(dmIconPosition.x() + (rightHandDM ? -img_size - widget_size : widget_size));
  cemStatusPosition.setY(dmIconPosition.y() - widget_size / 2);

  QRect cemWidget(cemStatusPosition, QSize(widget_size, widget_size));

  p.setBrush(blackColor(166));
  if (frogpilot_scene->conditional_status == 1) {
    p.setPen(QPen(QColor(bg_colors[STATUS_CONDITIONAL_OVERRIDDEN]), 10));
  } else if (frogpilot_scene->enabled && sm["selfdriveState"].getSelfdriveState().getExperimentalMode()) {
    p.setPen(QPen(QColor(bg_colors[STATUS_EXPERIMENTAL_MODE_ENABLED]), 10));
  } else {
    p.setPen(QPen(blackColor(), 10));
  }
  p.drawRoundedRect(cemWidget, 24, 24);

  QSharedPointer<QMovie> icon = chillModeIcon;
  if (frogpilot_scene->enabled && sm["selfdriveState"].getSelfdriveState().getExperimentalMode()) {
    if (frogpilot_scene->conditional_status == 1) {
      icon = chillModeIcon;
    } else if (frogpilot_scene->conditional_status == 2) {
      icon = experimentalModeIcon;
    } else if (frogpilot_scene->conditional_status == 3) {
      icon = cemCurveIcon;
    } else if (frogpilot_scene->conditional_status == 4) {
      icon = cemLeadIcon;
    } else if (frogpilot_scene->conditional_status == 5) {
      icon = cemTurnIcon;
    } else if (frogpilot_scene->conditional_status == 6 || frogpilot_scene->conditional_status == 7) {
      icon = cemSpeedIcon;
    } else if (frogpilot_scene->conditional_status == 8) {
      icon = cemStopIcon;
    } else {
      icon = experimentalModeIcon;
    }
  }
  if (icon) {
    fpWidgetPaintStage = 101; // CEM currentPixmap
    p.drawPixmap(cemWidget, icon->currentPixmap());
    fpWidgetPaintStage = 1; // back to CEM top level
  }

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintCompass(QPainter &p) {
  if (dmIconPosition == QPoint(0, 0)) {
    return;
  }

  p.save();

  constexpr double PIXELS_PER_DEGREE = 2.5;

  constexpr int BASE_RIBBON_WIDTH = static_cast<int>(360 * PIXELS_PER_DEGREE);
  constexpr int BORDER_WIDTH = 10;
  constexpr int MARGIN = 5;
  constexpr int TRIANGLE_SIZE = 40;

  static QPixmap compassRibbon = [&]() {
    QPixmap ribbon(BASE_RIBBON_WIDTH * 2, widget_size);
    ribbon.fill(Qt::transparent);

    QPainter ribbonPainter(&ribbon);
    ribbonPainter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QFont font = InterFont(65, QFont::Bold);
    ribbonPainter.setFont(font);
    QFontMetrics fm(font);

    QMap<int, QString> directionLabels = {{0, "N"}, {45, "NE"}, {90, "E"}, {135, "SE"}, {180, "S"}, {225, "SW"}, {270, "W"}, {315, "NW"}, {360, "N"}};

    for (int cycle = 0; cycle < 2; ++cycle) {
      int xOffset = cycle * 360;

      for (int degree = 0; degree < 360; ++degree) {
        int x = qRound((xOffset + degree) * PIXELS_PER_DEGREE);

        if (directionLabels.contains(degree)) {
          QString label = directionLabels[degree];
          ribbonPainter.setPen(whiteColor());
          ribbonPainter.drawText(x - fm.horizontalAdvance(label) / 2, fm.ascent(), label);
        }

        int notchHeight = (degree % 45 == 0) ? 35 : (degree % 15 == 0) ? 25 : 15;
        int notchWidth = (degree % 45 == 0) ? 5 : (degree % 15 == 0) ? 4 : 3;

        ribbonPainter.setPen(QPen(whiteColor(), notchWidth));
        ribbonPainter.drawLine(x, widget_size - notchHeight - MARGIN, x, widget_size);
      }
    }

    return ribbon;
  }();

  compassPosition.rx() = rightHandDM ? UI_BORDER_SIZE + widget_size / 2 : width() - UI_BORDER_SIZE - btn_size;
  compassPosition.ry() = dmIconPosition.y() - widget_size / 2;

  QRect compassWidget(compassPosition, QSize(widget_size, widget_size));

  p.setBrush(blackColor(166));
  p.setPen(QPen(blackColor(), BORDER_WIDTH));
  p.drawRoundedRect(compassWidget, 24, 24);

  QPainterPath clipPath;
  clipPath.addRoundedRect(compassWidget.adjusted(MARGIN, MARGIN, -MARGIN, -MARGIN), 24, 24);
  p.setClipPath(clipPath);

  // Use cached bearing from updateState() to avoid JSON parsing at 20Hz
  int offset = qRound(cachedBearing * PIXELS_PER_DEGREE) % BASE_RIBBON_WIDTH;
  int drawX = compassWidget.center().x() - offset;

  p.drawPixmap(drawX - BASE_RIBBON_WIDTH, compassWidget.top() + MARGIN, compassRibbon);
  p.drawPixmap(drawX, compassWidget.top() + MARGIN, compassRibbon);

  int triangleX = compassWidget.center().x();
  int triangleY = compassWidget.bottom() - TRIANGLE_SIZE;
  QPolygon triangle({
    QPoint(triangleX, triangleY - TRIANGLE_SIZE),
    QPoint(triangleX - TRIANGLE_SIZE / 1.5, triangleY),
    QPoint(triangleX + TRIANGLE_SIZE / 1.5, triangleY)
  });

  p.setBrush(whiteColor());
  p.setPen(Qt::NoPen);
  p.drawPolygon(triangle);

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintCurveSpeedControl(QPainter &p, SubMaster &fpsm) {
  const cereal::FrogPilotPlan::Reader &frogpilotPlan = fpsm["frogpilotPlan"].getFrogpilotPlan();

  p.save();

  QRect curveSpeedRect(QPoint(setSpeedRect.right() + UI_BORDER_SIZE, setSpeedRect.top()), QSize(defaultSize.width() * 1.25, defaultSize.width() * 1.25));

  // Use pre-cached flipped icon to avoid transform at 20Hz
  const QPixmap &curveSpeedImage = frogpilotPlan.getRoadCurvature() < 0 ? curveSpeedIcon : curveSpeedIconFlipped;
  QSize curveSpeedSize = curveSpeedImage.size();
  QPoint curveSpeedPoint(curveSpeedRect.x() + (curveSpeedRect.width()  - curveSpeedSize.width())  / 2, curveSpeedRect.y() + (curveSpeedRect.height() - curveSpeedSize.height()) / 2);

  p.setOpacity(1.0);

  QRect cscRect(curveSpeedRect.topLeft() + QPoint(0, curveSpeedRect.height() + 10), QSize(curveSpeedRect.width(), 100));

  p.setBrush(blueColor(166));
  p.setFont(InterFont(45, QFont::Bold));
  p.setPen(QPen(blueColor(), 10));

  p.drawRoundedRect(cscRect, 24, 24);
  p.setPen(QPen(whiteColor(), 6));
  p.drawText(cscRect.adjusted(20, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, QString::number(std::nearbyint(fmin(speed, frogpilotPlan.getCscSpeed() * speedConversion))) + speedUnit);

  p.drawPixmap(curveSpeedPoint, curveSpeedImage);

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintCurveSpeedControlTraining(QPainter &p, SubMaster &fpsm) {
  const cereal::FrogPilotPlan::Reader &frogpilotPlan = fpsm["frogpilotPlan"].getFrogpilotPlan();

  p.save();

  if (!glowTimer.isValid()) {
    glowTimer.start();
  }

  QRect curveSpeedRect(QPoint(setSpeedRect.right() + UI_BORDER_SIZE, setSpeedRect.top()), QSize(defaultSize.width() * 1.25, defaultSize.width() * 1.25));
  // Use pre-cached flipped icon to avoid transform at 20Hz
  const QPixmap &curveSpeedImage = frogpilotPlan.getRoadCurvature() < 0 ? curveSpeedIcon : curveSpeedIconFlipped;

  qreal phase = (glowTimer.elapsed() % 2000) / 2000.0 * 2 * M_PI;
  qreal alphaFactor = 0.5 + 0.5 * sin(phase);

  QColor glowColor = blueColor();
  glowColor.setAlphaF(0.3 + 0.7 * alphaFactor);

  int glowWidth = 8 + static_cast<int>(2 * alphaFactor);

  p.setOpacity(1.0);

  p.setBrush(blackColor(166));
  p.setPen(QPen(glowColor, glowWidth));
  p.drawRoundedRect(curveSpeedRect, 24, 24);

  QSize curveSpeedSize = curveSpeedImage.size();
  QPoint curveSpeedPoint(curveSpeedRect.x() + (curveSpeedRect.width()  - curveSpeedSize.width())  / 2, curveSpeedRect.y() + (curveSpeedRect.height() - curveSpeedSize.height()) / 2);
  p.drawPixmap(curveSpeedPoint, curveSpeedImage);

  QRect textRect(curveSpeedRect.topLeft() + QPoint(0, curveSpeedRect.height() + 10), QSize(curveSpeedRect.width(), 50));
  p.setBrush(blackColor(166));
  p.setPen(QPen(blackColor(), 10));
  p.drawRoundedRect(textRect, 24, 24);

  p.setFont(InterFont(35, QFont::Bold));
  p.setPen(QPen(whiteColor(), 6));
  p.drawText(textRect.adjusted(20, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, "Training...");

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintForceStop(QPainter &p, SubMaster &fpsm) {
  const cereal::FrogPilotPlan::Reader &frogpilotPlan = fpsm["frogpilotPlan"].getFrogpilotPlan();

  p.save();

  QRect curveSpeedRect(QPoint(setSpeedRect.right() + UI_BORDER_SIZE, setSpeedRect.top()), QSize(defaultSize.width() * 1.25, defaultSize.width() * 1.25));

  p.setOpacity(1.0);

  QRect cscRect(curveSpeedRect.topLeft() + QPoint(0, curveSpeedRect.height() + 10), QSize(curveSpeedRect.width(), 100));

  p.setBrush(redColor(166));
  p.setFont(InterFont(45, QFont::Bold));
  p.setPen(QPen(QColor(255, 150, 150), 10));

  p.drawRoundedRect(cscRect, 24, 24);
  p.setPen(QPen(whiteColor(), 6));
  p.drawText(cscRect.adjusted(20, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, QString::number(std::nearbyint(frogpilotPlan.getForcingStopLength() * distanceConversion)) + leadDistanceUnit);

  QSize imgSize = forceStopImg.size();
  QPoint imgPoint(curveSpeedRect.x() + (curveSpeedRect.width()  - imgSize.width())  / 2, curveSpeedRect.y() + (curveSpeedRect.height() - imgSize.height()) / 2);
  p.drawPixmap(imgPoint, forceStopImg);

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintManualStop(QPainter &p, SubMaster &fpsm) {
  p.save();

  QRect curveSpeedRect(QPoint(setSpeedRect.right() + UI_BORDER_SIZE, setSpeedRect.top()), QSize(defaultSize.width() * 1.25, defaultSize.width() * 1.25));

  p.setOpacity(1.0);

  QSize imgSize = manualStopImg.size();
  QPoint imgPoint(curveSpeedRect.x() + (curveSpeedRect.width()  - imgSize.width())  / 2, curveSpeedRect.y() + (curveSpeedRect.height() - imgSize.height()) / 2);
  p.drawPixmap(imgPoint, manualStopImg);

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintLateralPaused(QPainter &p) {
  if (dmIconPosition == QPoint(0, 0)) {
    return;
  }

  p.save();

  if (cemStatusPosition != QPoint(0, 0)) {
    lateralPausedPosition = cemStatusPosition;
  } else {
    lateralPausedPosition.rx() = dmIconPosition.x();
    lateralPausedPosition.ry() = dmIconPosition.y() - widget_size / 2;
  }
  lateralPausedPosition.rx() += rightHandDM ? -UI_BORDER_SIZE - widget_size - UI_BORDER_SIZE : UI_BORDER_SIZE + widget_size + UI_BORDER_SIZE;

  QRect lateralWidget(lateralPausedPosition, QSize(widget_size, widget_size));

  p.setBrush(blackColor(166));
  p.setPen(QPen(QColor(bg_colors[STATUS_TRAFFIC_MODE_ENABLED]), 10));
  p.drawRoundedRect(lateralWidget, 24, 24);

  p.setOpacity(0.5);
  p.drawPixmap(lateralWidget, turnIcon);
  p.setOpacity(0.75);
  p.drawPixmap(lateralWidget, pausedIcon);

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintLeadMetrics(QPainter &p, bool adjacent, QPointF *chevron, const cereal::RadarState::LeadData::Reader &lead_data) {
  float leadDistance = lead_data.getDRel() + (adjacent ? std::abs(lead_data.getYRel()) : 0.0f);
  float leadSpeed = std::max(lead_data.getVLead(), 0.0f);

  QString distanceString = QString::number(qRound(leadDistance * distanceConversion));
  QString speedString = QString::number(qRound(leadSpeed * speedConversionMetrics));

  QVector<QString> textLines;
  textLines.reserve(3);
  if (adjacent) {
    textLines.append(QString("%1 %2").arg(distanceString, leadDistanceUnit));
    textLines.append(QString("%1 %2").arg(speedString, leadSpeedUnit));
  } else {
    if (frogpilot_toggles->value("openpilot_longitudinal").toBool()) {
      int desiredDistance = std::max(0, qRound(desiredFollowDistance * distanceConversion));
      textLines.append(QString("%1 %2 (%3)").arg(distanceString, leadDistanceUnit, tr("Desired: %1").arg(desiredDistance)));
    } else {
      textLines.append(QString("%1 %2").arg(distanceString, leadDistanceUnit));
    }
    textLines.append(QString("%1 %2").arg(speedString, leadSpeedUnit));

    float timeGap = leadDistance / std::max(speed / speedConversion, 1.0f);
    textLines.append(QString("%1 %2").arg(QString::number(timeGap, 'f', 2), tr("seconds")));
  }

  p.setFont(InterFont(45, QFont::DemiBold));
  p.setPen(whiteColor());

  QFontMetrics metrics(p.font());
  int lineHeight = metrics.lineSpacing();

  int maxTextWidth = 0;
  for (QString &line : textLines) {
    maxTextWidth = std::max(maxTextWidth, metrics.horizontalAdvance(line));
  }

  int centerX = (chevron[2].x() + chevron[0].x()) / 2;
  int startY = chevron[0].y() + lineHeight + 5;

  int xMargin = maxTextWidth * 0.1;
  int yMargin = lineHeight * 0.1;

  QRect textRect(centerX - maxTextWidth / 2, startY - lineHeight, maxTextWidth, textLines.size() * lineHeight);
  textRect.adjust(-xMargin, -yMargin, xMargin, yMargin);

  if (adjacent) {
    if (textRect.intersects(adjacentLeadTextRect) || textRect.intersects(leadTextRect)) {
      return;
    }
    adjacentLeadTextRect = textRect;
  } else {
    leadTextRect = textRect;
  }

  for (int i = 0; i < textLines.size(); ++i) {
    int lineX = centerX - metrics.horizontalAdvance(textLines[i]) / 2;
    int lineY = startY + (i * lineHeight);

    // Drop shadow instead of expensive strokePath
    p.setPen(QColor(0, 0, 0, 200));
    p.drawText(lineX + 2, lineY + 2, textLines[i]);
    p.setPen(whiteColor());
    p.drawText(lineX, lineY, textLines[i]);
  }
}

void FrogPilotAnnotatedCameraWidget::paintLongitudinalPaused(QPainter &p) {
  if (dmIconPosition == QPoint(0, 0)) {
    return;
  }

  p.save();

  QPoint longitudinalIconPosition;
  if (lateralPausedPosition != QPoint(0, 0)) {
    longitudinalIconPosition = lateralPausedPosition;
  } else if (cemStatusPosition != QPoint(0, 0)) {
    longitudinalIconPosition = cemStatusPosition;
  } else {
    longitudinalIconPosition.rx() = dmIconPosition.x();
    longitudinalIconPosition.ry() = dmIconPosition.y() - widget_size / 2;
  }
  longitudinalIconPosition.rx() += rightHandDM ? -UI_BORDER_SIZE - widget_size - UI_BORDER_SIZE : UI_BORDER_SIZE + widget_size + UI_BORDER_SIZE;

  QRect longitudinalWidget(longitudinalIconPosition, QSize(widget_size, widget_size));

  p.setBrush(blackColor(166));
  p.setPen(QPen(QColor(bg_colors[STATUS_TRAFFIC_MODE_ENABLED]), 10));
  p.drawRoundedRect(longitudinalWidget, 24, 24);

  p.setOpacity(0.5);
  p.drawPixmap(longitudinalWidget, speedIcon);
  p.setOpacity(0.75);
  p.drawPixmap(longitudinalWidget, pausedIcon);

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintPathEdges(QPainter &p, SubMaster &sm) {
  p.save();

  std::function<void(QLinearGradient&, const QColor&)> setPathEdgeColors = [&](QLinearGradient &gradient, QColor baseColor) {
    baseColor.setAlphaF(1.0f); gradient.setColorAt(0.0f, baseColor);
    baseColor.setAlphaF(0.5f); gradient.setColorAt(0.5f, baseColor);
    baseColor.setAlphaF(0.1f); gradient.setColorAt(1.0f, baseColor);
  };

  QLinearGradient pe(0, height(), 0, 0);
  if (frogpilot_scene->always_on_lateral_active) {
    setPathEdgeColors(pe, bg_colors[STATUS_ALWAYS_ON_LATERAL_ACTIVE]);
  } else if (frogpilot_scene->conditional_status == 1) {
    setPathEdgeColors(pe, bg_colors[STATUS_CONDITIONAL_OVERRIDDEN]);
  } else if (sm["selfdriveState"].getSelfdriveState().getExperimentalMode()) {
    setPathEdgeColors(pe, bg_colors[STATUS_EXPERIMENTAL_MODE_ENABLED]);
  } else if (frogpilot_scene->traffic_mode_enabled) {
    setPathEdgeColors(pe, bg_colors[STATUS_TRAFFIC_MODE_ENABLED]);
  } else if (frogpilot_toggles->value("color_scheme").toString() != "stock") {
    setPathEdgeColors(pe, QColor(frogpilot_toggles->value("path_edges_color").toString()));
  } else {
    pe.setColorAt(0.0f, QColor::fromHslF(148 / 360.0f, 0.94f, 0.41f, 1.0f));
    pe.setColorAt(0.5f, QColor::fromHslF(112 / 360.0f, 1.00f, 0.54f, 0.5f));
    pe.setColorAt(1.0f, QColor::fromHslF(112 / 360.0f, 1.00f, 0.54f, 0.1f));
  }

  QPainterPath path;
  path.addPolygon(track_vertices);
  path.addPolygon(track_edge_vertices);
  p.setBrush(pe);
  p.drawPath(path);

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintPedalIcons(QPainter &p, SubMaster &sm, SubMaster &fpsm) {
  p.save();

  const cereal::CarState::Reader &carState = sm["carState"].getCarState();
  const cereal::FrogPilotCarState::Reader &frogpilotCarState = fpsm["frogpilotCarState"].getFrogpilotCarState();

  float brakeOpacity = 1.0f;
  float gasOpacity = 1.0f;

  if (frogpilot_toggles->value("dynamic_pedals_on_ui").toBool()) {  // rare sub-toggle, keep as-is
    brakeOpacity = frogpilot_scene->standstill ? 1.0f : carState.getAEgo() < -0.25f ? std::max(0.25f, std::abs(carState.getAEgo())) : 0.25f;
    gasOpacity = std::max(0.25f, carState.getAEgo());
  } else if (frogpilot_toggles->value("static_pedals_on_ui").toBool()) {  // rare sub-toggle, keep as-is
    brakeOpacity = frogpilot_scene->standstill || frogpilotCarState.getBrakeLights() || carState.getAEgo() < -0.25f ? 1.0f : 0.25f;
    gasOpacity = carState.getAEgo() > 0.25 ? 1.0f : 0.25f;
  }

  int startX = experimentalButtonPosition.x();
  int startY = experimentalButtonPosition.y() + btn_size + UI_BORDER_SIZE;

  p.setOpacity(brakeOpacity);
  p.drawPixmap(startX, startY, brakePedalImg);

  p.setOpacity(gasOpacity);
  p.drawPixmap(startX + btn_size / 2, startY, gasPedalImg);

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintSpeedLimit(QPainter &p) {
  if (setSpeedRect.isEmpty()) {
    return;
  }

  p.save();

  SubMaster &fpsm = *frogpilotUIState()->sm;
  const cereal::FrogPilotPlan::Reader &frogpilotPlan = fpsm["frogpilotPlan"].getFrogpilotPlan();

  bool isFlashingPending = speedLimitChanged && unconfirmedSpeedLimitValid && (pendingLimitTimer.isValid() && pendingLimitTimer.elapsed() % 1000 >= 500);

  QString speedLimitStr;
  if (isFlashingPending) {
    speedLimitStr = QString::number(std::nearbyint(frogpilotPlan.getUnconfirmedSlcSpeedLimit() * speedConversion));
  } else {
    speedLimitStr = (speedLimit > 1) ? QString::number(std::nearbyint(speedLimit)) : "–";
  }

  QColor borderColor = isFlashingPending ? redColor() : blackColor();
  QColor textColor = isFlashingPending ? redColor() : blackColor();

  bool hasUsSpeedLimit = !toggleSpeedLimitVienna;
  bool hasEuSpeedLimit = !hasUsSpeedLimit;

  int euSignSize = 176;
  int usSignHeight = 186;
  int signMargin = 12;

  if (hasUsSpeedLimit) {
    speedLimitHeight = usSignHeight + signMargin;
  } else if (hasEuSpeedLimit) {
    speedLimitHeight = euSignSize + signMargin;
  }

  QRect signRect;
  if (hasUsSpeedLimit) {
    signRect = QRect(setSpeedRect.x() + signMargin, setSpeedRect.bottom() - speedLimitHeight, setSpeedRect.width() - 2 * signMargin, usSignHeight);
  } else if (hasEuSpeedLimit) {
    signRect = QRect(setSpeedRect.x() + signMargin, setSpeedRect.bottom() - speedLimitHeight, setSpeedRect.width() - 2 * signMargin, euSignSize);
  }
  speedLimitRect = signRect;

  if (hasUsSpeedLimit) {
    p.setPen(Qt::NoPen);
    p.setBrush(whiteColor());
    p.drawRoundedRect(signRect, 24, 24);
    p.setPen(QPen(borderColor, 6));
    p.drawRoundedRect(signRect.adjusted(9, 9, -9, -9), 16, 16);

    p.setOpacity(frogpilotPlan.getSlcOverriddenSpeed() == 0 ? 1.0 : 0.25);
    p.setPen(textColor);
    if (frogpilotPlan.getSlcOverriddenSpeed() == 0 && toggleShowSpeedLimitOffset) {
      p.setFont(InterFont(28, QFont::DemiBold));
      p.drawText(signRect.adjusted(0, 22, 0, 0), Qt::AlignTop | Qt::AlignHCenter, isFlashingPending ? tr("PENDING") : tr("LIMIT"));
      p.setFont(InterFont(70, QFont::Bold));
      p.drawText(signRect.adjusted(0, 51, 0, 0), Qt::AlignTop | Qt::AlignHCenter, speedLimitStr);
      p.setFont(InterFont(50, QFont::DemiBold));
      if (!isFlashingPending) {
        p.drawText(signRect.adjusted(0, 120, 0, 0), Qt::AlignTop | Qt::AlignHCenter, speedLimitOffsetStr);
      }
    } else {
      p.setFont(InterFont(28, QFont::DemiBold));
      p.drawText(signRect.adjusted(0, 22, 0, 0), Qt::AlignTop | Qt::AlignHCenter, isFlashingPending ? tr("PENDING") : tr("SPEED"));
      p.drawText(signRect.adjusted(0, 51, 0, 0), Qt::AlignTop | Qt::AlignHCenter, tr("LIMIT"));
      p.setFont(InterFont(70, QFont::Bold));
      p.drawText(signRect.adjusted(0, 85, 0, 0), Qt::AlignTop | Qt::AlignHCenter, speedLimitStr);
    }
  }

  if (hasEuSpeedLimit) {
    p.setPen(Qt::NoPen);
    p.setBrush(whiteColor());
    p.drawEllipse(signRect);
    p.setPen(QPen(Qt::red, 20));
    p.drawEllipse(signRect.adjusted(16, 16, -16, -16));

    p.setOpacity(frogpilotPlan.getSlcOverriddenSpeed() == 0 ? 1.0 : 0.25);
    p.setPen(textColor);
    if (toggleShowSpeedLimitOffset) {
      p.setFont(InterFont((speedLimitStr.size() >= 3) ? 60 : 70, QFont::Bold));
      p.drawText(signRect.adjusted(0, -25, 0, 0), Qt::AlignCenter, speedLimitStr);
      p.setFont(InterFont(40, QFont::DemiBold));
      if (!isFlashingPending) {
        p.drawText(signRect.adjusted(0, 100, 0, 0), Qt::AlignTop | Qt::AlignHCenter, speedLimitOffsetStr);
      }
    } else {
      p.setFont(InterFont((speedLimitStr.size() >= 3) ? 60 : 70, QFont::Bold));
      p.drawText(signRect, Qt::AlignCenter, speedLimitStr);
    }
  }

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintSpeedLimitSources(QPainter &p, SubMaster &fpsm) {
  p.save();

  const cereal::FrogPilotCarState::Reader &frogpilotCarState = fpsm["frogpilotCarState"].getFrogpilotCarState();
  const cereal::FrogPilotPlan::Reader &frogpilotPlan = fpsm["frogpilotPlan"].getFrogpilotPlan();

  std::function<void(QRect&, QPixmap&, const QString&, const double)> drawSource = [&](QRect &rect, QPixmap &icon, QString title, double speedLimitValue) {
    bool isActive = QString::fromUtf8(frogpilotPlan.getSlcSpeedLimitSource().cStr()) == title && speedLimitValue != 0;

    if (isActive) {
      p.setBrush(redColor(166));
      p.setFont(InterFont(35, QFont::Bold));
      p.setPen(QPen(redColor(), 10));
    } else {
      p.setBrush(blackColor(166));
      p.setFont(InterFont(35, QFont::DemiBold));
      p.setPen(QPen(blackColor(), 10));
    }

    QRect iconRect(rect.x() + 20, rect.y() + (rect.height() - img_size / 4) / 2, img_size / 4, img_size / 4);

    QString speedText;
    if (speedLimitValue != 0) {
      speedText = QString::number(std::nearbyint(speedLimitValue)) + speedUnit;
    } else {
      speedText = "N/A";
    }

    QString fullText = tr(title.toUtf8().constData()) + " - " + speedText;

    p.setOpacity(1.0);
    p.drawRoundedRect(rect, 24, 24);
    p.drawPixmap(iconRect, icon);

    p.setPen(QPen(whiteColor(), 6));
    QRect textRect(iconRect.right() + 10, rect.y(), rect.width() - iconRect.width() - 30, rect.height());

    if (isActive) {
      QFontMetrics fm(p.font());
      int textYPosition = textRect.y() + (textRect.height() - fm.height()) / 2 + fm.ascent();

      // Drop shadow instead of expensive strokePath
      p.setPen(QColor(0, 0, 0, 200));
      p.drawText(textRect.x() + 2, textYPosition + 2, fullText);
      p.setPen(QPen(whiteColor(), 6));
      p.drawText(textRect.x(), textYPosition, fullText);
    } else {
      p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, fullText);
    }
  };

  int signMargin = 12;

  if (toggleSlcPriorityMode) {
    QString activeSource = QString::fromUtf8(frogpilotPlan.getSlcSpeedLimitSource().cStr());
    bool isOverridden = frogpilotPlan.getSlcOverriddenSpeed() > 0;

    auto drawBox = [&](QRect r, QPixmap *icon, QString name, bool isRed) {
      if (isRed) {
        p.setBrush(redColor(166));
        p.setPen(QPen(redColor(), 10));
      } else {
        p.setBrush(blackColor(166));
        p.setPen(QPen(blackColor(), 10));
      }
      p.setOpacity(1.0);
      p.drawRoundedRect(r, 24, 24);

      if (icon) {
        p.setFont(InterFont(35, QFont::Bold));
        QFontMetrics fm(p.font());
        int textWidth = fm.horizontalAdvance(name);
        int iconSize = img_size / 4;
        int gap = 10;
        int totalContentWidth = iconSize + gap + textWidth;

        int startX = r.x() + (r.width() - totalContentWidth) / 2;
        int contentY = r.y() + (r.height() - iconSize) / 2;

        QRect iconRect(startX, contentY, iconSize, iconSize);
        p.drawPixmap(iconRect, *icon);

        QRect textRect(startX + iconSize + gap, r.y(), textWidth, r.height());
        p.setPen(QPen(whiteColor(), 6));
        p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, name);
      } else {
        int iconSize = img_size / 4;
        int startX = r.x() + (r.width() - iconSize) / 2;
        int startY = r.y() + (r.height() - iconSize) / 2;
        QRect iconRect(startX, startY, iconSize, iconSize);

        p.setPen(QPen(redColor(), 5));
        p.drawLine(iconRect.topLeft(), iconRect.bottomRight());
        p.drawLine(iconRect.topRight(), iconRect.bottomLeft());
      }
    };

    QPixmap *systemIcon = nullptr;
    QString systemName;

    if (activeSource == "Dashboard") {
      systemIcon = &dashboardIconScaled;
      systemName = "Dash";
    } else if (activeSource == "Map Data") {
      systemIcon = &mapDataIconScaled;
      systemName = "MapD";
    } else if (activeSource == "Mapbox") {
      systemIcon = &mapboxIconScaled;
      systemName = "MapB";
    } else if (activeSource == "Upcoming") {
      systemIcon = &nextMapsIconScaled;
      systemName = "Next";
    }

    // Determine best available source from raw values (independent of accept/deny state)
    QPixmap *availableIcon = nullptr;
    QString availableName;

    if (frogpilotCarState.getDashboardSpeedLimit() > 0) {
      availableIcon = &dashboardIconScaled;
      availableName = "Dash";
    } else if (frogpilotPlan.getSlcMapSpeedLimit() > 0) {
      availableIcon = &mapDataIconScaled;
      availableName = "MapD";
    } else if (frogpilotPlan.getSlcMapboxSpeedLimit() > 0) {
      availableIcon = &mapboxIconScaled;
      availableName = "MapB";
    } else if (frogpilotPlan.getSlcNextSpeedLimit() > 0) {
      availableIcon = &nextMapsIconScaled;
      availableName = "Next";
    }

    QRect rect1(speedLimitRect.x(), speedLimitRect.y() + speedLimitRect.height() + UI_BORDER_SIZE, speedLimitRect.width(), 60);

    if (isOverridden) {
      drawBox(rect1, &gasPedalImgScaled, "User", true);
      QRect rect2(rect1.x(), rect1.bottom() + UI_BORDER_SIZE / 2, rect1.width(), 60);
      drawBox(rect2, systemIcon, systemName, false);
    } else if (systemIcon != nullptr) {
      drawBox(rect1, systemIcon, systemName, true);
    } else if (frogpilot_scene->enabled) {
      // OP engaged, no accepted SLC speed - show "User" + available source or red X
      drawBox(rect1, &gasPedalImgScaled, "User", true);
      QRect rect2(rect1.x(), rect1.bottom() + UI_BORDER_SIZE / 2, rect1.width(), 60);
      drawBox(rect2, availableIcon, availableName, false);
    } else {
      drawBox(rect1, systemIcon, systemName, false);
    }
  } else {
    QRect dashboardRect(speedLimitRect.x() - signMargin, speedLimitRect.y() + speedLimitRect.height() + UI_BORDER_SIZE, 450, 60);
    QRect mapDataRect(dashboardRect.x(), dashboardRect.y() + dashboardRect.height() + UI_BORDER_SIZE / 2, 450, 60);
    QRect mapboxRect(mapDataRect.x(), mapDataRect.y() + mapDataRect.height() + UI_BORDER_SIZE / 2, 450, 60);
    QRect nextLimitRect(mapboxRect.x(), mapboxRect.y() + mapboxRect.height() + UI_BORDER_SIZE / 2, 450, 60);

    drawSource(dashboardRect, dashboardIconScaled, "Dashboard", frogpilotCarState.getDashboardSpeedLimit() * speedConversion);
    drawSource(mapDataRect, mapDataIconScaled, "Map Data", frogpilotPlan.getSlcMapSpeedLimit() * speedConversion);
    drawSource(mapboxRect, mapboxIconScaled, "Mapbox", frogpilotPlan.getSlcMapboxSpeedLimit() * speedConversion);
    drawSource(nextLimitRect, nextMapsIconScaled, "Upcoming", frogpilotPlan.getSlcNextSpeedLimit() * speedConversion);
  }

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintStandstillTimer(QPainter &p) {
  p.save();

  float transition = 0.0f;

  QColor startColor, endColor;
  if (standstillDuration < 60) {
    startColor = endColor = bg_colors[STATUS_ENGAGED];
  } else if (standstillDuration < 150) {
    startColor = bg_colors[STATUS_ENGAGED];
    endColor = bg_colors[STATUS_CONDITIONAL_OVERRIDDEN];

    transition = (standstillDuration - 60) / 150.0f;
  } else if (standstillDuration < 300) {
    startColor = bg_colors[STATUS_CONDITIONAL_OVERRIDDEN];
    endColor = bg_colors[STATUS_TRAFFIC_MODE_ENABLED];

    transition = (standstillDuration - 150) / 150.0f;
  } else {
    startColor = endColor = bg_colors[STATUS_TRAFFIC_MODE_ENABLED];

    transition = 0.0f;
  }

  QColor blendedColor(
    startColor.red() + transition * (endColor.red() - startColor.red()),
    startColor.green() + transition * (endColor.green() - startColor.green()),
    startColor.blue() + transition * (endColor.blue() - startColor.blue())
  );

  int minutes = standstillDuration / 60;
  int seconds = standstillDuration % 60;

  p.setFont(InterFont(176, QFont::Bold));
  {
    QString minuteStr = (minutes == 1) ? tr("1 minute") : QString(tr("%1 minutes")).arg(minutes);
    QRect textRect = p.fontMetrics().boundingRect(minuteStr);
    textRect.moveCenter({rect().center().x(), 210 - textRect.height() / 2});
    p.setPen(QPen(blendedColor));
    p.drawText(textRect.x(), textRect.bottom(), minuteStr);
  }

  p.setFont(InterFont(66));
  {
    QString secondStr = (seconds == 1) ? tr("1 second") : QString(tr("%1 seconds")).arg(seconds);
    QRect textRect = p.fontMetrics().boundingRect(secondStr);
    textRect.moveCenter({rect().center().x(), 290 - textRect.height() / 2});
    p.setPen(QPen(whiteColor()));
    p.drawText(textRect.x(), textRect.bottom(), secondStr);
  }

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintStoppingPoint(QPainter &p, SubMaster &sm) {
  p.save();

  const cereal::ModelDataV2::Reader &modelV2 = sm["modelV2"].getModelV2();

  QPointF centerPoint = (track_vertices.first() + track_vertices.last()) / 2.0f;
  QPointF stopSignPosition = centerPoint - QPointF(stopSignImg.width() / 2.0f, stopSignImg.height());
  p.drawPixmap(stopSignPosition, stopSignImg);

  if (toggleShowStoppingPointMetrics) {
    float stoppingDistance = modelV2.getPosition().getX()[33 - 1] * distanceConversion;
    QString distanceText = QString::number(std::nearbyint(stoppingDistance)) + leadDistanceUnit;

    QFont font = InterFont(45, QFont::DemiBold);
    QFontMetrics fm(font);

    QPointF textPosition(centerPoint.x() - fm.horizontalAdvance(distanceText) / 2.0f, centerPoint.y() - stopSignImg.height() - 35);

    // Drop shadow instead of expensive strokePath
    p.setFont(font);
    p.setPen(QColor(0, 0, 0, 200));
    p.drawText(textPosition + QPointF(2, 2), distanceText);
    p.setPen(whiteColor());
    p.drawText(textPosition, distanceText);
  }

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintTurnSignals(QPainter &p, SubMaster &sm) {
  if (signalImages.isEmpty() || signalImagesFlipped.isEmpty() ||
      animationFrameIndex >= signalImages.size() || animationFrameIndex >= signalImagesFlipped.size()) {
    return;
  }

  const cereal::CarState::Reader &carState = sm["carState"].getCarState();

  p.save();

  bool leftBlinker = carState.getLeftBlinker();
  bool blindspotActive = leftBlinker ? carState.getLeftBlindspot() : carState.getRightBlindspot();

  if (signalStyle == "static") {
    int signalXPosition = leftBlinker ? (rect().center().x() * 0.75) - signalWidth : rect().center().x() * 1.25;
    int signalYPosition = signalHeight / 2;

    // Use pre-cached flipped images to avoid transforms at 20Hz
    if (blindspotActive && !blindspotImages.empty()) {
      p.drawPixmap(signalXPosition, signalYPosition, signalWidth, signalHeight, leftBlinker ? blindspotImages[0] : blindspotImagesFlipped[0]);
    } else {
      p.drawPixmap(signalXPosition, signalYPosition, signalWidth, signalHeight, leftBlinker ? signalImages[animationFrameIndex] : signalImagesFlipped[animationFrameIndex]);
    }
  } else {
    int signalXPosition;
    if (signalStyle == "traditional_gif") {
      signalXPosition = leftBlinker ? width() - (animationFrameIndex * signalMovement) + signalWidth : (animationFrameIndex * signalMovement) - signalWidth;
    } else {
      signalXPosition = leftBlinker ? width() - ((animationFrameIndex + 1) * signalWidth) : animationFrameIndex * signalWidth;
    }
    int signalYPosition = height() - signalHeight - alertHeight;

    // Use pre-cached flipped images to avoid transforms at 20Hz
    if (blindspotActive && !blindspotImages.empty()) {
      p.drawPixmap(leftBlinker ? width() - signalWidth : 0, signalYPosition, signalWidth, signalHeight, leftBlinker ? blindspotImages[0] : blindspotImagesFlipped[0]);
    } else {
      p.drawPixmap(signalXPosition, signalYPosition, signalWidth, signalHeight, leftBlinker ? signalImages[animationFrameIndex] : signalImagesFlipped[animationFrameIndex]);
    }
  }

  p.restore();
}

void FrogPilotAnnotatedCameraWidget::paintWeather(QPainter &p, SubMaster &fpsm) {
  const cereal::FrogPilotPlan::Reader &frogpilotPlan = fpsm["frogpilotPlan"].getFrogpilotPlan();
  int weatherId = frogpilotPlan.getWeatherId();
  if (weatherId == 0) {
    return;
  }

  p.save();

  QPoint weatherIconPosition;
  if (compassPosition != QPoint(0, 0)) {
    weatherIconPosition = compassPosition;
    weatherIconPosition.rx() += (rightHandDM ? UI_BORDER_SIZE + widget_size + UI_BORDER_SIZE : -UI_BORDER_SIZE - widget_size - UI_BORDER_SIZE);
  } else {
    weatherIconPosition.rx() = rightHandDM ? UI_BORDER_SIZE + widget_size / 2 : width() - UI_BORDER_SIZE - btn_size;
    weatherIconPosition.ry() = dmIconPosition.y() - widget_size / 2;
  }

  QRect weatherRect(weatherIconPosition, QSize(widget_size, widget_size));

  p.setBrush(blackColor(166));
  p.setPen(QPen(blackColor(), 10));
  p.drawRoundedRect(weatherRect, 24, 24);

  QSharedPointer<QMovie> icon = weatherClearDay;
  if ((weatherId >= 200 && weatherId <= 232) || (weatherId >= 300 && weatherId <= 321) || (weatherId >= 500 && weatherId <= 531)) {
    icon = weatherRain;
  } else if (weatherId >= 600 && weatherId <= 622) {
    icon = weatherSnow;
  } else if (weatherId >= 701 && weatherId <= 762) {
    icon = weatherLowVisibility;
  } else if (weatherId == 800) {
    icon = frogpilotPlan.getWeatherDaytime() ? weatherClearDay : weatherClearNight;
  }

  if (icon) {
    fpWidgetPaintStage = 1501; // Weather currentPixmap
    p.drawPixmap(weatherRect, icon->currentPixmap());
    fpWidgetPaintStage = 15; // back to Weather top level
  }

  p.restore();
}
