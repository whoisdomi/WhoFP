#include <sys/resource.h>
#include <csignal>
#include <fcntl.h>
#include <unistd.h>
#include <execinfo.h>

#include <QApplication>
#include <QTranslator>

#include "system/hardware/hw.h"
#include "selfdrive/ui/qt/qt_window.h"
#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/window.h"

extern volatile int modelDrawStage;
extern volatile int fpWidgetPaintStage;
extern volatile int fpUpdateStage;

// Intercept Qt fatal messages from Wayland disconnect and exit cleanly
// instead of letting Qt call abort(). The manager will restart the UI.
// Qt 5.12.8 doesn't support QT_WAYLAND_RECONNECT, so this is the only way.
static void waylandAwareMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  // Forward non-fatal messages to the normal handler
  if (type != QtMsgType::QtFatalMsg) {
    swagLogMessageHandler(type, context, msg);
    return;
  }

  // Log the fatal message
  swagLogMessageHandler(type, context, msg);

  // Check if this is a Wayland disconnect fatal
  QByteArray msgBytes = msg.toUtf8();
  bool isWaylandFatal = msgBytes.contains("ayland") || msgBytes.contains("wl_display");

  if (isWaylandFatal) {
    // Log to crash file and exit cleanly — manager will restart us
    const char logmsg[] = "UI WAYLAND DISCONNECT: exiting cleanly for restart\n";
    int fd = open("/data/ui_crash.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd >= 0) { write(fd, logmsg, sizeof(logmsg) - 1); close(fd); }
    write(STDERR_FILENO, logmsg, sizeof(logmsg) - 1);
    _exit(0);  // Clean exit before Qt calls abort()
  }

  // Non-Wayland fatal: let Qt abort normally so crash_handler captures it
}

static void crash_handler(int sig) {
  const char *sig_name = (sig == SIGSEGV) ? "SIGSEGV" : (sig == SIGABRT) ? "SIGABRT" : "SIGFPE";
  char buf[512];
  int len = snprintf(buf, sizeof(buf),
    "UI CRASH: %s | modelDraw=%d | fpPaint=%d | fpUpdate=%d\n",
    sig_name, (int)modelDrawStage, (int)fpWidgetPaintStage, (int)fpUpdateStage);

  int fd = open("/data/ui_crash.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (len > 0) {
    write(STDERR_FILENO, buf, len);
    if (fd >= 0) write(fd, buf, len);
  }

  // Capture backtrace
  void *bt[32];
  int bt_size = backtrace(bt, 32);
  if (fd >= 0) {
    const char hdr[] = "--- backtrace ---\n";
    write(fd, hdr, sizeof(hdr) - 1);
    backtrace_symbols_fd(bt, bt_size, fd);
    const char end[] = "--- end ---\n";
    write(fd, end, sizeof(end) - 1);
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

  qInstallMessageHandler(waylandAwareMessageHandler);
  initApp(argc, argv);

  QTranslator translator;
  QString translation_file = QString::fromStdString(Params().get("LanguageSetting"));
  if (!translator.load(QString(":/%1").arg(translation_file)) && translation_file.length()) {
    qCritical() << "Failed to load translation file:" << translation_file;
  }

  QApplication a(argc, argv);
  a.installTranslator(&translator);

  // Log Qt version and Wayland platform info to crash log for debugging
  {
    char info[512];
    int len = snprintf(info, sizeof(info),
      "UI STARTED: Qt %s | platform=%s | pid=%d\n",
      qVersion(), qApp->platformName().toUtf8().constData(),
      getpid());
    if (len > 0) {
      fprintf(stderr, "%s", info);
      int fd = open("/data/ui_crash.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
      if (fd >= 0) { write(fd, info, len); close(fd); }
    }
  }

  MainWindow w;
  setMainWindow(&w);
  a.installEventFilter(&w);
  return a.exec();
}
