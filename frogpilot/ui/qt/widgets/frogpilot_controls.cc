#include "selfdrive/ui/ui.h"

#include "frogpilot/ui/frogpilot_ui.h"

void clearMovie(QSharedPointer<QMovie> &movie, QWidget *parent) {
  if (movie) {
    QObject::disconnect(movie.data(), &QMovie::frameChanged, parent, nullptr);
    movie->stop();
    movie.reset();
  }
}

void loadGif(const QString &gifPath, QSharedPointer<QMovie> &movie, const QSize &size, QWidget *parent) {
  clearMovie(movie, parent);

  movie = QSharedPointer<QMovie>::create(gifPath, QByteArray(), parent);
  movie->setCacheMode(QMovie::CacheAll);
  movie->setScaledSize(size);

  QObject::connect(movie.data(), &QMovie::frameChanged, parent, [parent]() {
    if (parent->isVisible()) {
      parent->update();
    }
  }, Qt::UniqueConnection);

  movie->start();
}

void loadImage(const QString &basePath, QPixmap &pixmap, QSharedPointer<QMovie> &movie, const QSize &size, QWidget *parent) {
  QString gifPath = basePath + ".gif";

  if (QFileInfo::exists(gifPath)) {
    loadGif(gifPath, movie, size, parent);
    pixmap = QPixmap();
  } else {
    clearMovie(movie, parent);
    pixmap = QPixmap(basePath + ".png").scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }

  parent->update();
}
