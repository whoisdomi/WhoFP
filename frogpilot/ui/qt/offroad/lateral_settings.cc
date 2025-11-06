#include "frogpilot/ui/qt/offroad/lateral_settings.h"

FrogPilotLateralPanel::FrogPilotLateralPanel(FrogPilotSettingsWindow *parent) : FrogPilotListWidget(parent), parent(parent) {
  QJsonObject shownDescriptions = QJsonDocument::fromJson(QString::fromStdString(params.get("ShownToggleDescriptions")).toUtf8()).object();
  QString className = this->metaObject()->className();

  if (!shownDescriptions.value(className).toBool(false)) {
    forceOpenDescriptions = true;
    shownDescriptions.insert(className, true);
    params.put("ShownToggleDescriptions", QJsonDocument(shownDescriptions).toJson(QJsonDocument::Compact).toStdString());
  }

  QStackedLayout *lateralLayout = new QStackedLayout();
  addItem(lateralLayout);

  FrogPilotListWidget *lateralList = new FrogPilotListWidget(this);

  ScrollView *lateralPanel = new ScrollView(lateralList, this);

  lateralLayout->addWidget(lateralPanel);

  FrogPilotListWidget *advancedLateralTuneList = new FrogPilotListWidget(this);
  FrogPilotListWidget *advancedTurnDesiresList = new FrogPilotListWidget(this);
  FrogPilotListWidget *aolList = new FrogPilotListWidget(this);
  FrogPilotListWidget *laneChangeList = new FrogPilotListWidget(this);
  FrogPilotListWidget *lateralTuneList = new FrogPilotListWidget(this);
  FrogPilotListWidget *qolList = new FrogPilotListWidget(this);

  ScrollView *advancedLateralTunePanel = new ScrollView(advancedLateralTuneList, this);
  ScrollView *advancedTurnDesiresPanel = new ScrollView(advancedTurnDesiresList, this);
  ScrollView *aolPanel = new ScrollView(aolList, this);
  ScrollView *laneChangePanel = new ScrollView(laneChangeList, this);
  ScrollView *lateralTunePanel = new ScrollView(lateralTuneList, this);
  ScrollView *qolPanel = new ScrollView(qolList, this);

  lateralLayout->addWidget(advancedLateralTunePanel);
  lateralLayout->addWidget(advancedTurnDesiresPanel);
  lateralLayout->addWidget(aolPanel);
  lateralLayout->addWidget(laneChangePanel);
  lateralLayout->addWidget(lateralTunePanel);
  lateralLayout->addWidget(qolPanel);

  const std::vector<std::tuple<QString, QString, QString, QString>> lateralToggles {
    {"AdvancedLateralTune", tr("Advanced Lateral Tuning"), tr("<b>Advanced steering control changes to fine-tune how openpilot drives.</b>"), "../../frogpilot/assets/toggle_icons/icon_advanced_lateral_tune.png"},
    {"SteerDelay", steerActuatorDelay != 0 ? QString(tr("Actuator Delay (Default: %1)")).arg(QString::number(steerActuatorDelay, 'f', 2)) : tr("Actuator Delay"), tr("<b>The time between openpilot's steering command and the vehicle's response.</b> Increase if the vehicle reacts late; decrease if it feels jumpy. Auto-learned by default."), ""},
    {"SteerFriction", friction != 0 ? QString(tr("Friction (Default: %1)")).arg(QString::number(friction, 'f', 2)) : tr("Friction"), tr("<b>Compensates for steering friction.</b> Increase if the wheel sticks near center; decrease if it jitters. Auto-learned by default."), ""},
    {"SteerKP", steerKp != 0 ? QString(tr("Kp Factor (Default: %1)")).arg(QString::number(steerKp, 'f', 2)) : tr("Kp Factor"), tr("<b>How strongly openpilot corrects lane position.</b> Higher is tighter but twitchier; lower is smoother but slower. Auto-learned by default."), ""},
    {"SteerKF", steerKf != 0 ? QString(tr("Kf Factor (Default: %1)")).arg(QString::number(steerKf, 'f', 2)) : tr("Kf Factor"), tr("<b>Feedforward gain for steering control.</b> Adjusts how proactively openpilot steers."), ""},
    {"SteerKI", steerKi != 0 ? QString(tr("Ki Factor (Default: %1)")).arg(QString::number(steerKi, 'f', 2)) : tr("Ki Factor"), tr("<b>Corrects persistent steering errors.</b> Higher values reduce steady-state error but can cause oscillations."), ""},
    {"SteerKD", steerKd != 0 ? QString(tr("Kd Factor (Default: %1)")).arg(QString::number(steerKd, 'f', 2)) : tr("Kd Factor"), tr("<b>Dampens steering oscillations.</b> Higher values reduce overshoot but can make the steering feel sluggish."), ""},
    {"SteerLatAccel", latAccelFactor != 0 ? QString(tr("Lateral Acceleration (Default: %1)")).arg(QString::number(latAccelFactor, 'f', 2)) : tr("Lateral Acceleration"), tr("<b>Maps steering torque to turning response.</b> Increase for sharper turns; decrease for gentler steering. Auto-learned by default."), ""},
    {"SteerRatio", steerRatio != 0 ? QString(tr("Steer Ratio (Default: %1)")).arg(QString::number(steerRatio, 'f', 2)) : tr("Steer Ratio"), tr("<b>The relationship between steering wheel rotation and road wheel angle.</b> Increase if steering feels too quick or twitchy; decrease if it feels too slow or weak. Auto-learned by default."), ""},
    {"ForceAutoTune", tr("Force Auto-Tune On"), tr("<b>Force-enable openpilot's live auto-tuning for \"Friction\" and \"Lateral Acceleration\".</b>"), ""},
    {"ForceAutoTuneOff", tr("Force Auto-Tune Off"), tr("<b>Force-disable openpilot's live auto-tuning for \"Friction\" and \"Lateral Acceleration\" and use the set value instead.</b>"), ""},
    {"ForceTorqueController", tr("Force Torque Controller"), tr("<b>Use torque-based steering control instead of angle-based control for smoother lane keeping, especially in curves.</b>"), ""},

    {"AdvancedTurnDesires", tr("Advanced Turn Desires"), tr("<b>Enhanced turn handling with curvature bias and faster steering response for sharper turns at low speeds.</b>"), "../../frogpilot/assets/toggle_icons/icon_lateral_tune.png"},
    {"ATDSpeedMax", tr("Speed Max"), tr("<b>Maximum speed ATD will operate at.</b> Higher speed = ATD activates at higher speeds. Lower speed = ATD only works at lower speeds."), ""},
    {"ATDSteeringMin", tr("Steering Min"), tr("<b>Minimum steering angle needed for ATD to operate.</b> Higher angle = ATD requires sharper turns. Lower angle = ATD activates on gentler turns."), ""},
    {"ATDLeftTurnBiasPercent", tr("Left Turn Bias Percent"), tr("<b>Percent of additional left curvature to pull turns inward.</b> More negative = tighter left turns. Less negative = gentler left turns."), ""},
    {"ATDRightTurnBiasPercent", tr("Right Turn Bias Percent"), tr("<b>Percent of additional right curvature to pull turns inward.</b> More positive = tighter right turns. Less positive = gentler right turns."), ""},
    {"ATDMinBiasAbsolute", tr("Min Bias Absolute"), tr("<b>Minimum absolute bias to introduce for gentle curves.</b> Higher value = stronger minimum turn adjustment. Lower value = more subtle adjustments."), ""},
    {"ATDTurnLatSmooth", tr("Turn Lateral Smoothness"), tr("<b>Lateral smoothness factor during active turns.</b> Less = faster/more responsive steering. More = smoother/gentler steering."), ""},
    {"ATDPostTurnFrames", tr("Post Turn Frames"), tr("<b>Number of frames to maintain fast smoothing after turn completes.</b> Higher = longer fast response after turn. Lower = quicker return to normal."), ""},
    {"ATDDeactivationSteeringExtreme", tr("Deactivation Steering Extreme"), tr("<b>Steering angle threshold for extreme deactivation.</b> Higher = allows more extreme angles before deactivating. Lower = deactivates earlier."), ""},
    {"ATDDeactivationSteeringEarly", tr("Deactivation Steering Early"), tr("<b>Steering angle threshold for early deactivation.</b> Higher = allows tighter turns. Lower = deactivates on gentler curves."), ""},
    {"ATDSteeringDecreaseRatio", tr("Steering Decrease Ratio"), tr("<b>Ratio for detecting when steering is straightening out.</b> Higher = requires more straightening to deactivate. Lower = deactivates with less straightening."), ""},
    {"ATDDebugEnabled", tr("Debug Enabled"), tr("<b>Prints ATD information for debugging purposes.</b>"), ""},

    {"AlwaysOnLateral", tr("Always On Lateral"), tr("<b>openpilot's steering remains active even when the accelerator or brake pedals are pressed.</b>"), "../../frogpilot/assets/toggle_icons/icon_always_on_lateral.png"},
    {"AlwaysOnLateralMain", tr("Enable With Cruise Control"), tr("<b>Enable \"Always On Lateral\" whenever \"Cruise Control\" is on, even when openpilot is not engaged.</b>"), ""},
    {"AlwaysOnLateralLKAS", tr("Enable With LKAS"), tr("<b>Enable \"Always On Lateral\" whenever \"LKAS\" is on, even when openpilot is not engaged.</b>"), ""},
    {"PauseAOLOnBrake", tr("Pause on Brake Press Below"), tr("<b>Pause \"Always On Lateral\" below the set speed while the brake pedal is pressed.</b>"), ""},

    {"LaneChanges", tr("Lane Changes"), tr("<b>Allow openpilot to change lanes.</b>"), "../../frogpilot/assets/toggle_icons/icon_lane.png"},
    {"NudgelessLaneChange", tr("Automatic Lane Changes"), tr("<b>When the turn signal is on, openpilot will automatically change lanes.</b> No steering-wheel nudge required!"), ""},
    {"LaneChangeTime", tr("Lane Change Delay"), tr("<b>Delay between turn signal activation and the start of an automatic lane change.</b>"), ""},
    {"MinimumLaneChangeSpeed", tr("Minimum Lane Change Speed"), tr("<b>Lowest speed at which openpilot will change lanes.</b>"), ""},
    {"LaneDetectionWidth", tr("Minimum Lane Width"), tr("<b>Prevent automatic lane changes into lanes narrower than the set width.</b>"), ""},
    {"OneLaneChange", tr("One Lane Change Per Signal"), tr("<b>Limit automatic lane changes to one per turn-signal activation.</b>"), ""},

    {"LateralTune", tr("Lateral Tuning"), tr("<b>Miscellaneous steering control changes</b> to fine-tune how openpilot drives."), "../../frogpilot/assets/toggle_icons/icon_lateral_tune.png"},
    {"TurnDesires", tr("Force Turn Desires Below Lane Change Speed"), tr("<b>While driving below the minimum lane change speed with an active turn signal, instruct openpilot to turn left/right.</b>"), ""},
    {"NNFF", tr("Neural Network Feedforward (NNFF)"), tr("<b>Twilsonco's \"Neural Network FeedForward\" model controller for smoother, model-based steering trained on your vehicle's data.</b>"), ""},
    {"NNFFLite", tr("Smooth Curve Handling"), tr("<b>Twilsonco's torque-based adjustments to smoothen out steering in curves.</b>"), ""},

    {"QOLLateral", tr("Quality of Life"), tr("<b>Steering control changes to fine-tune how openpilot drives.</b>"), "../../frogpilot/assets/toggle_icons/icon_quality_of_life.png"},
    {"PauseLateralSpeed", tr("Pause Steering Below"), tr("<b>Pause steering below the set speed.</b>"), ""},

    {"IgnoreMe", "Ignore Me", "This is simply used to fix the layout when the user opens the descriptions and the menu gets wonky. No idea why it happens, but I can't be asked to properly fix it so whatever. Sue me.", ""},
    {"IgnoreMe2", "Ignore Me", "This is simply used to fix the layout when the user opens the descriptions and the menu gets wonky. No idea why it happens, but I can't be asked to properly fix it so whatever. Sue me.", ""}
  };

  for (const auto &[param, title, desc, icon] : lateralToggles) {
    AbstractControl *lateralToggle;

    if (param == "AdvancedLateralTune") {
      FrogPilotManageControl *advancedLateralTuneToggle = new FrogPilotManageControl(param, title, desc, icon);
      QObject::connect(advancedLateralTuneToggle, &FrogPilotManageControl::manageButtonClicked, [lateralLayout, advancedLateralTunePanel]() {
        lateralLayout->setCurrentWidget(advancedLateralTunePanel);
      });
      lateralToggle = advancedLateralTuneToggle;
    } else if (param == "SteerDelay") {
      std::vector<QString> steerDelayButton{"Reset"};
      lateralToggle = new FrogPilotParamValueButtonControl(param, title, desc, icon, 0.01, 1, QString(), std::map<float, QString>(), 0.01, false, {}, steerDelayButton, false, false);
    } else if (param == "SteerFriction") {
      std::vector<QString> steerFrictionButton{"Reset"};
      lateralToggle = new FrogPilotParamValueButtonControl(param, title, desc, icon, 0, 0.5, QString(), std::map<float, QString>(), 0.01, false, {}, steerFrictionButton, false, false);
    } else if (param == "SteerKP") {
      std::vector<QString> steerKPButton{"Reset"};
      lateralToggle = new FrogPilotParamValueButtonControl(param, title, desc, icon, steerKp * 0.25, steerKp * 3.0, QString(), std::map<float, QString>(), 0.01, false, {}, steerKPButton, false, false);
    } else if (param == "SteerKI") {
      std::vector<QString> steerKIButton{"Reset"};
      lateralToggle = new FrogPilotParamValueButtonControl(param, title, desc, icon, 0, 1.5, QString(), std::map<float, QString>(), 0.01, false, {}, steerKIButton, false, false);
    } else if (param == "SteerKF") {
      std::vector<QString> steerKFButton{"Reset"};
      lateralToggle = new FrogPilotParamValueButtonControl(param, title, desc, icon, 0, 1.5, QString(), std::map<float, QString>(), 0.01, false, {}, steerKFButton, false, false);
    } else if (param == "SteerKD") {
      std::vector<QString> steerKDButton{"Reset"};
      lateralToggle = new FrogPilotParamValueButtonControl(param, title, desc, icon, 0, 1.5, QString(), std::map<float, QString>(), 0.01, false, {}, steerKDButton, false, false);
    } else if (param == "SteerLatAccel") {
      std::vector<QString> steerLatAccelButton{"Reset"};
      lateralToggle = new FrogPilotParamValueButtonControl(param, title, desc, icon, latAccelFactor * 0.75, latAccelFactor * 1.25, QString(), std::map<float, QString>(), 0.01, false, {}, steerLatAccelButton, false, false);
    } else if (param == "SteerRatio") {
      std::vector<QString> steerRatioButton{"Reset"};
      lateralToggle = new FrogPilotParamValueButtonControl(param, title, desc, icon, steerRatio * 0.5, steerRatio * 1.5, QString(), std::map<float, QString>(), 0.01, false, {}, steerRatioButton, false, false);

    } else if (param == "AdvancedTurnDesires") {
      FrogPilotManageControl *advancedTurnDesiresToggle = new FrogPilotManageControl(param, title, desc, icon);
      QObject::connect(advancedTurnDesiresToggle, &FrogPilotManageControl::manageButtonClicked, [lateralLayout, advancedTurnDesiresPanel]() {
        lateralLayout->setCurrentWidget(advancedTurnDesiresPanel);
      });
      lateralToggle = advancedTurnDesiresToggle;
    } else if (param == "ATDSpeedMax") {
      std::map<float, QString> speedMaxLabels;
      for (int i = 1; i <= 20; ++i) {
        speedMaxLabels[i] = QString::number(i) + tr(" m/s");
      }
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 1, 20, QString(), speedMaxLabels, 1);
    } else if (param == "ATDSteeringMin") {
      std::map<float, QString> steeringMinLabels;
      for (int i = 1; i <= 45; ++i) {
        steeringMinLabels[i] = QString::number(i) + tr(" degrees");
      }
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 1, 45, QString(), steeringMinLabels, 1);
    } else if (param == "ATDLeftTurnBiasPercent") {
      std::map<float, QString> leftTurnBiasLabels;
      for (int i = 0; i >= -30; --i) {
        leftTurnBiasLabels[i] = QString::number(i) + tr("%");
      }
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, -30, 0, QString(), leftTurnBiasLabels, 1);
    } else if (param == "ATDRightTurnBiasPercent") {
      std::map<float, QString> rightTurnBiasLabels;
      for (int i = 0; i <= 30; ++i) {
        rightTurnBiasLabels[i] = QString::number(i) + tr("%");
      }
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 0, 30, QString(), rightTurnBiasLabels, 1);
    } else if (param == "ATDMinBiasAbsolute") {
      std::map<float, QString> minBiasLabels;
      for (int i = 0; i <= 20; ++i) {
        float value = i * 0.001f;
        minBiasLabels[value] = QString::number(value, 'f', 3);
      }
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 0.000, 0.020, QString(), minBiasLabels, 0.001);
    } else if (param == "ATDTurnLatSmooth") {
      std::map<float, QString> turnLatSmoothLabels;
      for (int i = 1; i <= 20; ++i) {
        float value = i * 0.01f;
        turnLatSmoothLabels[value] = QString::number(value, 'f', 2) + tr(" s");
      }
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 0.01, 0.20, QString(), turnLatSmoothLabels, 0.01);
    } else if (param == "ATDPostTurnFrames") {
      std::map<float, QString> postTurnFramesLabels;
      for (int i = 30; i <= 60; i += 10) {
        postTurnFramesLabels[i] = QString::number(i) + tr(" frames");
      }
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 30, 60, QString(), postTurnFramesLabels, 10);
    } else if (param == "ATDDeactivationSteeringExtreme") {
      std::map<float, QString> deactivationExtremeLabels;
      for (int i = 100; i <= 500; i += 10) {
        deactivationExtremeLabels[i] = QString::number(i) + tr(" degrees");
      }
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 100, 500, QString(), deactivationExtremeLabels, 10);
    } else if (param == "ATDDeactivationSteeringEarly") {
      std::map<float, QString> deactivationEarlyLabels;
      for (int i = 50; i <= 200; i += 10) {
        deactivationEarlyLabels[i] = QString::number(i) + tr(" degrees");
      }
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 50, 200, QString(), deactivationEarlyLabels, 10);
    } else if (param == "ATDSteeringDecreaseRatio") {
      std::map<float, QString> steeringDecreaseLabels;
      for (int i = 20; i <= 100; i += 5) {
        float value = i * 0.01f;
        steeringDecreaseLabels[value] = QString::number(value, 'f', 2);
      }
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 0.20, 1.00, QString(), steeringDecreaseLabels, 0.05);

    } else if (param == "AlwaysOnLateral") {
      FrogPilotManageControl *aolToggle = new FrogPilotManageControl(param, title, desc, icon);
      QObject::connect(aolToggle, &FrogPilotManageControl::manageButtonClicked, [lateralLayout, aolPanel]() {
        lateralLayout->setCurrentWidget(aolPanel);
      });
      lateralToggle = aolToggle;
    } else if (param == "PauseAOLOnBrake") {
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 0, 99, QString(), std::map<float, QString>(), 1, true);

    } else if (param == "LaneChanges") {
      FrogPilotManageControl *laneChangeToggle = new FrogPilotManageControl(param, title, desc, icon);
      QObject::connect(laneChangeToggle, &FrogPilotManageControl::manageButtonClicked, [lateralLayout, laneChangePanel]() {
        lateralLayout->setCurrentWidget(laneChangePanel);
      });
      lateralToggle = laneChangeToggle;
    } else if (param == "LaneChangeTime") {
      std::map<float, QString> laneChangeTimeLabels;
      for (float i = 0; i <= 5; i += 0.1) {
        laneChangeTimeLabels[i] = i == 0 ? tr("Instant") : std::lround(i / 0.1) == 1 / 0.1 ? QString::number(i, 'f', 1) + tr(" second") : QString::number(i, 'f', 1) + tr(" seconds");
      }
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 0, 5, QString(), laneChangeTimeLabels, 0.1);
    } else if (param == "LaneDetectionWidth") {
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 0, 15, QString(), std::map<float, QString>(), 0.1, true);
    } else if (param == "MinimumLaneChangeSpeed") {
      lateralToggle = new FrogPilotParamValueControl(param, title, desc, icon, 0, 99, QString(), std::map<float, QString>(), 1, true);

    } else if (param == "LateralTune") {
      FrogPilotManageControl *lateralTuneToggle = new FrogPilotManageControl(param, title, desc, icon);
      QObject::connect(lateralTuneToggle, &FrogPilotManageControl::manageButtonClicked, [lateralLayout, lateralTunePanel]() {
        lateralLayout->setCurrentWidget(lateralTunePanel);
      });
      lateralToggle = lateralTuneToggle;

    } else if (param == "QOLLateral") {
      FrogPilotManageControl *qolLateralToggle = new FrogPilotManageControl(param, title, desc, icon);
      QObject::connect(qolLateralToggle, &FrogPilotManageControl::manageButtonClicked, [lateralLayout, qolPanel]() {
        lateralLayout->setCurrentWidget(qolPanel);
      });
      lateralToggle = qolLateralToggle;
    } else if (param == "PauseLateralSpeed") {
      std::vector<QString> pauseLateralToggles{"PauseLateralOnSignal"};
      std::vector<QString> pauseLateralToggleNames{tr("Turn Signal Only")};
      lateralToggle = new FrogPilotParamValueButtonControl(param, title, desc, icon, 0, 99, QString(), std::map<float, QString>(), 1, true, pauseLateralToggles, pauseLateralToggleNames, true);

    } else {
      lateralToggle = new ParamControl(param, title, desc, icon);
    }

    toggles[param] = lateralToggle;

    if (advancedLateralTuneKeys.contains(param)) {
      advancedLateralTuneList->addItem(lateralToggle);
    } else if (advancedTurnDesiresKeys.contains(param)) {
      advancedTurnDesiresList->addItem(lateralToggle);
    } else if (aolKeys.contains(param)) {
      aolList->addItem(lateralToggle);
    } else if (laneChangeKeys.contains(param)) {
      laneChangeList->addItem(lateralToggle);
    } else if (lateralTuneKeys.contains(param)) {
      lateralTuneList->addItem(lateralToggle);
    } else if (qolKeys.contains(param)) {
      qolList ->addItem(lateralToggle);
    } else {
      lateralList->addItem(lateralToggle);

      parentKeys.insert(param);
    }

    if (FrogPilotManageControl *frogPilotManageToggle = qobject_cast<FrogPilotManageControl*>(lateralToggle)) {
      QObject::connect(frogPilotManageToggle, &FrogPilotManageControl::manageButtonClicked, [this]() {
        emit openSubPanel();
        openDescriptions(forceOpenDescriptions, toggles);
      });
    }

    QObject::connect(lateralToggle, &AbstractControl::hideDescriptionEvent, [this]() {
      update();
    });
    QObject::connect(lateralToggle, &AbstractControl::showDescriptionEvent, [this]() {
      update();
    });
  }

  QSet<QString> forceUpdateKeys = {"ForceAutoTune", "ForceAutoTuneOff", "LateralTune", "NNFF", "NudgelessLaneChange"};
  for (const QString &key : forceUpdateKeys) {
    QObject::connect(static_cast<ToggleControl*>(toggles[key]), &ToggleControl::toggleFlipped, this, &FrogPilotLateralPanel::updateToggles);
  }

  QSet<QString> rebootKeys = {"AlwaysOnLateral", "ForceTorqueController", "NNFF", "NNFFLite"};
  for (const QString &key : rebootKeys) {
    QObject::connect(static_cast<ToggleControl*>(toggles[key]), &ToggleControl::toggleFlipped, [key, this](bool state) {
      if (started) {
        if (key == "AlwaysOnLateral" && state) {
          if (FrogPilotConfirmationDialog::toggleReboot(this)) {
            Hardware::reboot();
          }
        } else if (key == "NNFF" || key == "NNFFLite") {
          if (!isTorqueCar) {
            if (FrogPilotConfirmationDialog::toggleReboot(this)) {
              Hardware::reboot();
            }
          }
        } else if (key != "AlwaysOnLateral") {
          if (FrogPilotConfirmationDialog::toggleReboot(this)) {
            Hardware::reboot();
          }
        }
      }
    });
  }

  steerDelayToggle = static_cast<FrogPilotParamValueButtonControl*>(toggles["SteerDelay"]);
  QObject::connect(steerDelayToggle, &FrogPilotParamValueButtonControl::buttonClicked, [this]() {
    if (FrogPilotConfirmationDialog::yesorno(tr("Reset <b>Actuator Delay</b> to its default value?"), this)) {
      params.putFloat("SteerDelay", steerActuatorDelay);
      steerDelayToggle->refresh();
    }
  });

  steerFrictionToggle = static_cast<FrogPilotParamValueButtonControl*>(toggles["SteerFriction"]);
  QObject::connect(steerFrictionToggle, &FrogPilotParamValueButtonControl::buttonClicked, [this]() {
    if (FrogPilotConfirmationDialog::yesorno(tr("Reset <b>Friction</b> to its default value?"), this)) {
      params.putFloat("SteerFriction", friction);
      steerFrictionToggle->refresh();
    }
  });

  steerKPToggle = static_cast<FrogPilotParamValueButtonControl*>(toggles["SteerKP"]);
  QObject::connect(steerKPToggle, &FrogPilotParamValueButtonControl::buttonClicked, [this]() {
    if (FrogPilotConfirmationDialog::yesorno(tr("Reset <b>Kp Factor</b> to its default value?"), this)) {
      params.putFloat("SteerKP", steerKp);
      steerKPToggle->refresh();
    }
  });
  
  steerKIToggle = static_cast<FrogPilotParamValueButtonControl*>(toggles["SteerKI"]);
  QObject::connect(steerKIToggle, &FrogPilotParamValueButtonControl::buttonClicked, [this]() {
    if (FrogPilotConfirmationDialog::yesorno(tr("Reset <b>Ki Factor</b> to its default value?"), this)) {
      params.putFloat("SteerKI", steerKi);
      steerKIToggle->refresh();
    }
  });

  steerKFToggle = static_cast<FrogPilotParamValueButtonControl*>(toggles["SteerKF"]);
  QObject::connect(steerKFToggle, &FrogPilotParamValueButtonControl::buttonClicked, [this]() {
    if (FrogPilotConfirmationDialog::yesorno(tr("Reset <b>Kf Factor</b> to its default value?"), this)) {
      params.putFloat("SteerKF", steerKf);
      steerKFToggle->refresh();
    }
  });

  steerKDToggle = static_cast<FrogPilotParamValueButtonControl*>(toggles["SteerKD"]);
  QObject::connect(steerKDToggle, &FrogPilotParamValueButtonControl::buttonClicked, [this]() {
    if (FrogPilotConfirmationDialog::yesorno(tr("Reset <b>Kd Factor</b> to its default value?"), this)) {
      params.putFloat("SteerKD", steerKd);
      steerKDToggle->refresh();
    }
  });
  steerLatAccelToggle = static_cast<FrogPilotParamValueButtonControl*>(toggles["SteerLatAccel"]);
  QObject::connect(steerLatAccelToggle, &FrogPilotParamValueButtonControl::buttonClicked, [this]() {
    if (FrogPilotConfirmationDialog::yesorno(tr("Reset <b>Lateral Accel</b> to its default value?"), this)) {
      params.putFloat("SteerLatAccel", latAccelFactor);
      steerLatAccelToggle->refresh();
    }
  });

  steerRatioToggle = static_cast<FrogPilotParamValueButtonControl*>(toggles["SteerRatio"]);
  QObject::connect(steerRatioToggle, &FrogPilotParamValueButtonControl::buttonClicked, [this]() {
    if (FrogPilotConfirmationDialog::yesorno(tr("Reset <b>Steer Ratio</b> to its default value?"), this)) {
      params.putFloat("SteerRatio", steerRatio);
      steerRatioToggle->refresh();
    }
  });

  openDescriptions(forceOpenDescriptions, toggles);

  QObject::connect(parent, &FrogPilotSettingsWindow::closeSubPanel, [lateralLayout, lateralPanel, this] {
    openDescriptions(forceOpenDescriptions, toggles);
    lateralLayout->setCurrentWidget(lateralPanel);
  });
  QObject::connect(parent, &FrogPilotSettingsWindow::updateMetric, this, &FrogPilotLateralPanel::updateMetric);
  QObject::connect(uiState(), &UIState::uiUpdate, this, &FrogPilotLateralPanel::updateState);
}

void FrogPilotLateralPanel::showEvent(QShowEvent *event) {
  frogpilotToggleLevels = parent->frogpilotToggleLevels;
  friction = parent->friction;
  hasAutoTune = parent->hasAutoTune;
  hasNNFFLog = parent->hasNNFFLog;
  hasOpenpilotLongitudinal = parent->hasOpenpilotLongitudinal;
  isAngleCar = parent->isAngleCar;
  isHKGCanFd = parent->isHKGCanFd;
  isTorqueCar = parent->isTorqueCar;
  latAccelFactor = parent->latAccelFactor;
  steerActuatorDelay = parent->steerActuatorDelay;
  steerKp = parent->steerKp;
  steerKi = parent->steerKi;
  steerKf = parent->steerKf;
  steerKd = parent->steerKd;
  steerRatio = parent->steerRatio;
  tuningLevel = parent->tuningLevel;

  steerDelayToggle->setTitle(QString(tr("Actuator Delay (Default: %1)")).arg(QString::number(steerActuatorDelay, 'f', 2)));
  steerFrictionToggle->setTitle(QString(tr("Friction (Default: %1)")).arg(QString::number(friction, 'f', 2)));
  steerKPToggle->setTitle(QString(tr("Kp Factor (Default: %1)")).arg(QString::number(steerKp, 'f', 2)));
  steerKPToggle->updateControl(steerKp * 0.25, steerKp * 3.0);
  steerLatAccelToggle->setTitle(QString(tr("Lateral Accel (Default: %1)")).arg(QString::number(latAccelFactor, 'f', 2)));
  steerKIToggle->setTitle(QString(tr("Ki Factor (Default: %1)")).arg(QString::number(steerKi, 'f', 2)));
  steerKFToggle->setTitle(QString(tr("Kf Factor (Default: %1)")).arg(QString::number(steerKf, 'f', 2)));
  steerKDToggle->setTitle(QString(tr("Kd Factor (Default: %1)")).arg(QString::number(steerKd, 'f', 2)));
  steerLatAccelToggle->updateControl(latAccelFactor * 0.75, latAccelFactor * 1.25);
  steerRatioToggle->setTitle(QString(tr("Steer Ratio (Default: %1)")).arg(QString::number(steerRatio, 'f', 2)));
  steerRatioToggle->updateControl(steerRatio * 0.5, steerRatio * 1.5);

  updateToggles();
}

void FrogPilotLateralPanel::updateState(const UIState &s) {
  if (!isVisible()) return;

  started = s.scene.started;
}

void FrogPilotLateralPanel::updateMetric(bool metric, bool bootRun) {
  static bool previousMetric;
  if (metric != previousMetric && !bootRun) {
    double distanceConversion = metric ? FOOT_TO_METER : METER_TO_FOOT;
    double speedConversion = metric ? MILE_TO_KM : KM_TO_MILE;

    params.putFloatNonBlocking("LaneDetectionWidth", params.getFloat("LaneDetectionWidth") * distanceConversion);

    params.putIntNonBlocking("MinimumLaneChangeSpeed", params.getInt("MinimumLaneChangeSpeed") * speedConversion);
    params.putIntNonBlocking("PauseAOLOnBrake", params.getInt("PauseAOLOnBrake") * speedConversion);
    params.putIntNonBlocking("PauseLateralSpeed", params.getInt("PauseLateralSpeed") * speedConversion);
  }
  previousMetric = metric;

  static std::map<float, QString> imperialDistanceLabels;
  static std::map<float, QString> imperialSpeedLabels;
  static std::map<float, QString> metricDistanceLabels;
  static std::map<float, QString> metricSpeedLabels;

  static bool labelsInitialized = false;
  if (!labelsInitialized) {
    for (int i = 0; i <= 150; ++i) {
      float key = i / 10.0f;
      imperialDistanceLabels[key] = key == 0 ? tr("Off") : i == 1 ? QString::number(i) + tr(" foot") : QString::number(key, 'f', 1) + tr(" feet");
    }

    for (int i = 0; i <= 99; ++i) {
      imperialSpeedLabels[i] = i == 0 ? tr("Off") : QString::number(i) + tr(" mph");
    }

    for (int i = 0; i <= 50; ++i) {
      float key = i / 10.0f;
      metricDistanceLabels[key] = key == 0 ? tr("Off") : i == 1 ? QString::number(i) + tr(" meter") : QString::number(key, 'f', 1) + tr(" meters");
    }

    for (int i = 0; i <= 150; ++i) {
      metricSpeedLabels[i] = i == 0 ? tr("Off") : QString::number(i) + tr(" km/h");
    }

    labelsInitialized = true;
  }

  FrogPilotParamValueControl *laneWidthToggle = static_cast<FrogPilotParamValueControl*>(toggles["LaneDetectionWidth"]);
  FrogPilotParamValueControl *minimumLaneChangeSpeedToggle = static_cast<FrogPilotParamValueControl*>(toggles["MinimumLaneChangeSpeed"]);
  FrogPilotParamValueControl *pauseAOLOnBrakeToggle = static_cast<FrogPilotParamValueControl*>(toggles["PauseAOLOnBrake"]);
  FrogPilotParamValueControl *pauseLateralToggle = static_cast<FrogPilotParamValueControl*>(toggles["PauseLateralSpeed"]);

  if (metric) {
    laneWidthToggle->updateControl(0, 5, metricDistanceLabels);

    minimumLaneChangeSpeedToggle->updateControl(0, 150, metricSpeedLabels);
    pauseAOLOnBrakeToggle->updateControl(0, 150, metricSpeedLabels);
    pauseLateralToggle->updateControl(0, 150, metricSpeedLabels);
  } else {
    laneWidthToggle->updateControl(0, 15, imperialDistanceLabels);

    minimumLaneChangeSpeedToggle->updateControl(0, 99, imperialSpeedLabels);
    pauseAOLOnBrakeToggle->updateControl(0, 99, imperialSpeedLabels);
    pauseLateralToggle->updateControl(0, 99, imperialSpeedLabels);
  }
}

void FrogPilotLateralPanel::updateToggles() {
  for (auto &[key, toggle] : toggles) {
    if (parentKeys.contains(key)) {
      toggle->setVisible(false);
    }
  }

  bool forcingAutoTune = !hasAutoTune && params.getBool("ForceAutoTune");
  bool forcingAutoTuneOff = hasAutoTune && params.getBool("ForceAutoTuneOff");
  bool forcingTorqueController = !isAngleCar && params.getBool("ForceTorqueController");
  bool usingNNFF = hasNNFFLog && params.getBool("LateralTune") && params.getBool("NNFF");

  for (auto &[key, toggle] : toggles) {
    if (parentKeys.contains(key)) {
      continue;
    }

    bool setVisible = tuningLevel >= frogpilotToggleLevels[key].toDouble();

    if (key == "AlwaysOnLateralLKAS") {
      setVisible &= isHKGCanFd;
      setVisible &= !hasOpenpilotLongitudinal;
    }

    else if (key == "AlwaysOnLateralMain") {
      setVisible &= !isHKGCanFd;
      setVisible |= hasOpenpilotLongitudinal;
    }

    else if (key == "ForceAutoTune") {
      setVisible &= !hasAutoTune;
      setVisible &= !isAngleCar;
      setVisible &= isTorqueCar || forcingTorqueController;
    }

    else if (key == "ForceAutoTuneOff") {
      setVisible &= hasAutoTune;
    }

    else if (key == "ForceTorqueController") {
      setVisible &= !isAngleCar;
      setVisible &= !isTorqueCar;
    }

    else if (key == "LaneChangeTime") {
      setVisible &= params.getBool("LaneChanges") && params.getBool("NudgelessLaneChange");
    }

    else if (key == "LaneDetectionWidth") {
      setVisible &= params.getBool("LaneChanges") && params.getBool("NudgelessLaneChange");
    }

    else if (key == "NNFF") {
      setVisible &= hasNNFFLog;
      setVisible &= !isAngleCar;
    }

    else if (key == "NNFFLite") {
      setVisible &= !usingNNFF;
      setVisible &= !isAngleCar;
    }

    else if (key == "SteerDelay") {
      setVisible &= steerActuatorDelay != 0;
    }

    else if (key == "SteerFriction") {
      setVisible &= friction != 0;
      setVisible &= hasAutoTune ? forcingAutoTuneOff : !forcingAutoTune;
      setVisible &= isTorqueCar || forcingTorqueController;
      setVisible &= !usingNNFF;
    }

    else if (key == "SteerKP") {
      setVisible &= steerKp != 0;
      setVisible &= isTorqueCar || forcingTorqueController;
    }

    else if (key == "SteerKI") {
      setVisible &= steerKi != 0;
      setVisible &= isTorqueCar || forcingTorqueController;
    }

    else if (key == "SteerKF") {
      setVisible &= steerKf != 0;
      setVisible &= isTorqueCar || forcingTorqueController;
    }

    else if (key == "SteerKD") {
      setVisible &= steerKd != 0;  
      setVisible &= isTorqueCar || forcingTorqueController;
    }

    else if (key == "SteerLatAccel") {
      setVisible &= latAccelFactor != 0;
      setVisible &= hasAutoTune ? forcingAutoTuneOff : !forcingAutoTune;
      setVisible &= isTorqueCar || forcingTorqueController;
      setVisible &= !usingNNFF;
    }

    else if (key == "SteerRatio") {
      setVisible &= steerRatio != 0;
      setVisible &= hasAutoTune ? forcingAutoTuneOff : !forcingAutoTune;
    }

    toggle->setVisible(setVisible);

    if (setVisible) {
      if (advancedLateralTuneKeys.contains(key)) {
        toggles["AdvancedLateralTune"]->setVisible(true);
      } else if (aolKeys.contains(key)) {
        toggles["AlwaysOnLateral"]->setVisible(true);
      } else if (laneChangeKeys.contains(key)) {
        toggles["LaneChanges"]->setVisible(true);
      } else if (lateralTuneKeys.contains(key)) {
        toggles["LateralTune"]->setVisible(true);
      } else if (qolKeys.contains(key)) {
        toggles["QOLLateral"]->setVisible(true);
      }
    }
  }

  openDescriptions(forceOpenDescriptions, toggles);

  update();
}
