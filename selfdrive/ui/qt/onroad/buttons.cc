#include "selfdrive/ui/qt/onroad/buttons.h"

#include <QPainter>

#include "selfdrive/ui/qt/util.h"

void drawIcon(QPainter &p, const QPoint &center, const QPixmap &img, const QBrush &bg, float opacity) {
  p.setRenderHint(QPainter::Antialiasing);
  p.setOpacity(1.0);  // bg dictates opacity of ellipse
  p.setPen(Qt::NoPen);
  p.setBrush(bg);
  p.drawEllipse(center, btn_size / 2, btn_size / 2);
  p.setOpacity(opacity);
  p.drawPixmap(center - QPoint(img.width() / 2, img.height() / 2), img);
  p.setOpacity(1.0);
}

// ExperimentalButton
ExperimentalButton::ExperimentalButton(QWidget *parent) : experimental_mode(false), engageable(false), QPushButton(parent) {
  setFixedSize(btn_size, btn_size);

  engage_img = loadPixmap("../assets/icons/chffr_wheel.png", {img_size, img_size});
  experimental_img = loadPixmap("../assets/icons/experimental.svg", {img_size, img_size});
  QObject::connect(this, &QPushButton::clicked, this, &ExperimentalButton::changeMode);

  // FrogPilot variables
}

void ExperimentalButton::changeMode() {
  const auto cp = (*uiState()->sm)["carParams"].getCarParams();
  bool can_change = hasLongitudinalControl(cp) && params.getBool("ExperimentalModeConfirmed");
  if (can_change) {
    // FrogPilot variables
    params.putBool("ExperimentalMode", !experimental_mode);
  }
}

void ExperimentalButton::updateState(const UIState &s, const FrogPilotUIState &fs) {
  const auto cs = (*s.sm)["selfdriveState"].getSelfdriveState();
  bool eng = cs.getEngageable() || cs.getEnabled() || fs.frogpilot_scene.always_on_lateral_active;
  if ((cs.getExperimentalMode() != experimental_mode) || (eng != engageable)) {
    engageable = eng;
    experimental_mode = cs.getExperimentalMode();
    update();
  }

  // FrogPilot variables
  const cereal::CarState::Reader &carState = (*s.sm)["carState"].getCarState();
}

void ExperimentalButton::paintEvent(QPaintEvent *event) {
  updateBackgroundColor();

  QPainter p(this);

  QPainterPath clip_path;
  clip_path.addEllipse(QPoint(btn_size / 2, btn_size / 2), btn_size / 2, btn_size / 2);
  p.setClipPath(clip_path);

  QPixmap img = experimental_mode ? experimental_img : engage_img;
  drawIcon(p, QPoint(btn_size / 2, btn_size / 2), img, background_color, (isDown() || !engageable) ? 0.6 : 1.0);

  p.setClipping(false);
}

// FrogPilot variables
void ExperimentalButton::showEvent(QShowEvent *event) {
}

void ExperimentalButton::updateBackgroundColor() {
  static const QMap<QString, QColor> status_color_map {
    {"default", QColor(0, 0, 0, 166)},
    {"always_on_lateral_active", bg_colors[STATUS_ALWAYS_ON_LATERAL_ACTIVE]},
    {"experimental_mode_enabled", bg_colors[STATUS_EXPERIMENTAL_MODE_ENABLED]}
  };

  if (isDown() || !engageable) {
    background_color = status_color_map["default"];
    return;
  }

  if (frogpilot_scene.always_on_lateral_active) {
    background_color = status_color_map["always_on_lateral_active"];
  } else if (experimental_mode) {
    background_color = status_color_map["experimental_mode_enabled"];
  } else {
    background_color = status_color_map["default"];
  }
}
