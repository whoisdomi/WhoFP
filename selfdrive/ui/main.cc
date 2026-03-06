#include <sys/resource.h>
#include <csignal>
#include <fcntl.h>
#include <unistd.h>

#include <QApplication>
#include <QTranslator>

#include "system/hardware/hw.h"
#include "selfdrive/ui/qt/qt_window.h"
#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/window.h"

extern volatile int modelDrawStage;
extern volatile int fpWidgetPaintStage;

static void crash_handler(int sig) {
  const char *sig_name = (sig == SIGSEGV) ? "SIGSEGV" : (sig == SIGABRT) ? "SIGABRT" : "SIGFPE";
  char buf[256];
  int len = snprintf(buf, sizeof(buf),
    "UI CRASH: %s | modelDrawStage=%d | fpWidgetPaintStage=%d\n",
    sig_name, (int)modelDrawStage, (int)fpWidgetPaintStage);
  if (len > 0) write(STDERR_FILENO, buf, len);
  int fd = open("/data/ui_crash.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (fd >= 0) {
    write(fd, buf, len);
    close(fd);
  }
  signal(sig, SIG_DFL);
  raise(sig);
}

int main(int argc, char *argv[]) {
  setpriority(PRIO_PROCESS, 0, -20);

  signal(SIGSEGV, crash_handler);
  signal(SIGABRT, crash_handler);
  signal(SIGFPE, crash_handler);

  qInstallMessageHandler(swagLogMessageHandler);
  initApp(argc, argv);

  QTranslator translator;
  QString translation_file = QString::fromStdString(Params().get("LanguageSetting"));
  if (!translator.load(QString(":/%1").arg(translation_file)) && translation_file.length()) {
    qCritical() << "Failed to load translation file:" << translation_file;
  }

  QApplication a(argc, argv);
  a.installTranslator(&translator);

  MainWindow w;
  setMainWindow(&w);
  a.installEventFilter(&w);
  return a.exec();
}
