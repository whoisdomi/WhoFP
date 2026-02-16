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

// Crash stage trackers set by paint methods — async-signal-safe read from handler
// model.cc stages: 101=update_model, 102=laneLines, 103=drawPath, 104=leads, 105=adjLeads, 106=drawLeadAdj, 107=radar
// frogpilot_annotated_camera.cc stages: 1=CEM, 2=Compass, 3=CSC, 4=LatPause, 5=LonPause,
//   6=Pedals, 7=PendingSL, 8=Radar, 9=Road, 10=SL, 11=SLSrc, 12=Standstill, 13=StopPt, 14=Signals, 15=Weather
extern volatile int modelDrawStage;
extern volatile int fpWidgetPaintStage;

static void crash_handler(int sig) {
  // Only use async-signal-safe functions here (write, open, close, _exit)
  const char *sig_name = (sig == SIGSEGV) ? "SIGSEGV" : (sig == SIGABRT) ? "SIGABRT" : "SIGFPE";

  char buf[256];
  int len = snprintf(buf, sizeof(buf),
    "UI CRASH: %s | modelDrawStage=%d | fpWidgetPaintStage=%d\n",
    sig_name, (int)modelDrawStage, (int)fpWidgetPaintStage);

  // Write to stderr (journal)
  if (len > 0) write(STDERR_FILENO, buf, len);

  // Write to persistent file — survives reboots, check with: cat /data/ui_crash.log
  int fd = open("/data/ui_crash.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (fd >= 0) {
    write(fd, buf, len);
    close(fd);
  }

  // Re-raise to get the default handler (core dump / tombstone)
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
