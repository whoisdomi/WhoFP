#include <sys/resource.h>
#include <csignal>
#include <fcntl.h>
#include <unistd.h>
#include <execinfo.h>
#include <dlfcn.h>
#include <time.h>
#include <thread>
#include <chrono>

#include <QApplication>
#include <QTranslator>

#include "system/hardware/hw.h"
#include "common/util.h"
#include "selfdrive/ui/qt/qt_window.h"
#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/window.h"
#include "selfdrive/ui/ui.h"

extern volatile int modelDrawStage;
extern volatile int fpWidgetPaintStage;
extern volatile int fpUpdateStage;

// Track when the UI started (monotonic) for uptime calculation
static struct timespec ui_start_time;

// Signal-safe: get uptime in seconds since UI started
static int ui_uptime_sec() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return (int)(now.tv_sec - ui_start_time.tv_sec);
}

// Signal-safe: write a timestamp prefix like "[2026-03-26 14:30:05 uptime=123s] "
static int write_timestamp(char *buf, int bufsize) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  struct tm tm;
  localtime_r(&ts.tv_sec, &tm);
  return snprintf(buf, bufsize, "[%04d-%02d-%02d %02d:%02d:%02d up=%ds] ",
    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
    tm.tm_hour, tm.tm_min, tm.tm_sec, ui_uptime_sec());
}

// Signal-safe: read VmRSS from /proc/self/status
static int get_rss_mb() {
  char buf[1024];
  int fd = open("/proc/self/status", O_RDONLY);
  if (fd < 0) return -1;
  int n = read(fd, buf, sizeof(buf) - 1);
  close(fd);
  if (n <= 0) return -1;
  buf[n] = '\0';
  // Find "VmRSS:" line
  const char *p = buf;
  while (*p) {
    if (p[0] == 'V' && p[1] == 'm' && p[2] == 'R' && p[3] == 'S' && p[4] == 'S') {
      p += 6; // skip "VmRSS:"
      while (*p == ' ' || *p == '\t') p++;
      int kb = 0;
      while (*p >= '0' && *p <= '9') { kb = kb * 10 + (*p - '0'); p++; }
      return kb / 1024;
    }
    while (*p && *p != '\n') p++;
    if (*p == '\n') p++;
  }
  return -1;
}

// Helper: write a log line with timestamp to crash log and stderr
static void log_crash_event(const char *msg) {
  char buf[512];
  int tlen = write_timestamp(buf, sizeof(buf));
  int mlen = snprintf(buf + tlen, sizeof(buf) - tlen, "%s (rss=%dMB)\n", msg, get_rss_mb());
  int total = tlen + mlen;
  if (total > 0) {
    write(STDERR_FILENO, buf, total);
    int fd = open("/data/ui_crash.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd >= 0) { write(fd, buf, total); close(fd); }
  }
}

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
    log_crash_event("UI WAYLAND DISCONNECT: exiting cleanly for restart");
    _exit(0);  // Clean exit before Qt calls abort()
  }

  // Non-Wayland fatal: let Qt abort normally so crash_handler captures it
}

// Check if a backtrace frame is inside a Wayland/EGL library
static bool is_wayland_frame(void *addr) {
  Dl_info info;
  if (dladdr(addr, &info) && info.dli_fname) {
    const char *name = info.dli_fname;
    // Check for wayland client, EGL wayland driver, or Qt wayland plugin
    for (const char *needle : {"wayland", "Wayland", "eglSub"}) {
      for (const char *p = name; *p; ++p) {
        const char *n = needle;
        const char *q = p;
        while (*n && *q == *n) { ++q; ++n; }
        if (!*n) return true;
      }
    }
  }
  return false;
}

static void crash_handler(int sig) {
  const char *sig_name = (sig == SIGSEGV) ? "SIGSEGV" : (sig == SIGABRT) ? "SIGABRT" : "SIGFPE";

  // Check backtrace for Wayland-related crash (EGL thread SIGSEGV)
  void *bt[32];
  int bt_size = backtrace(bt, 32);

  if (sig == SIGSEGV) {
    for (int i = 0; i < bt_size; ++i) {
      if (is_wayland_frame(bt[i])) {
        log_crash_event("UI WAYLAND EGL CRASH: exiting cleanly for restart");
        _exit(0);
      }
    }
  }

  // Non-Wayland crash: log full details
  char buf[512];
  int tlen = write_timestamp(buf, sizeof(buf));
  int mlen = snprintf(buf + tlen, sizeof(buf) - tlen,
    "UI CRASH: %s | modelDraw=%d | fpPaint=%d | fpUpdate=%d | rss=%dMB\n",
    sig_name, (int)modelDrawStage, (int)fpWidgetPaintStage, (int)fpUpdateStage, get_rss_mb());
  int total = tlen + mlen;

  int fd = open("/data/ui_crash.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (total > 0) {
    write(STDERR_FILENO, buf, total);
    if (fd >= 0) write(fd, buf, total);
  }

  // Capture backtrace
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

  clock_gettime(CLOCK_MONOTONIC, &ui_start_time);

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

  // Log startup info with timestamp
  {
    char ts[128];
    write_timestamp(ts, sizeof(ts));
    char info[512];
    int len = snprintf(info, sizeof(info),
      "%sUI STARTED: Qt %s | platform=%s | pid=%d | rss=%dMB\n",
      ts, qVersion(), qApp->platformName().toUtf8().constData(),
      getpid(), get_rss_mb());
    if (len > 0) {
      fprintf(stderr, "%s", info);
      int fd = open("/data/ui_crash.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
      if (fd >= 0) { write(fd, info, len); close(fd); }
    }
  }

  // Helper: safe read of thread stack from proc
static void log_thread_stack(int log_fd) {
  char path[128];
  snprintf(path, sizeof(path), "/proc/self/task/%d/stack", getpid());
  int fd = open(path, O_RDONLY);
  if (fd >= 0) {
    const char hdr[] = "--- kernel stack ---\n";
    write(log_fd, hdr, sizeof(hdr) - 1);
    char buf[4096];
    int n = read(fd, buf, sizeof(buf));
    if (n > 0) write(log_fd, buf, n);
    close(fd);
  }
}

// Monitor for stalls (>2s) to capture stage info before manager watchdog SIGKILL
  std::thread monitor([] {
    uint64_t last_monitor_t = nanos_since_boot();
    while (true) {
      uint64_t last_t = last_ui_frame_t.load();
      uint64_t now = nanos_since_boot();
      
      // Clock jump protection: if system time jumps backwards or more than 5 minutes forward, 
      // reset the monitor instead of triggering a false stall.
      uint64_t monitor_dt = (now > last_monitor_t) ? (now - last_monitor_t) : 0;
      if (now < last_monitor_t || monitor_dt > 300000000000ULL) {
        last_ui_frame_t = now;
        last_monitor_t = now;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
      }
      last_monitor_t = now;

      if (last_t > 0 && (now - last_t) > 2000000000ULL) {
        char ts[128];
        write_timestamp(ts, sizeof(ts));
        char buf[512];
        int len = snprintf(buf, sizeof(buf), "%sUI STALL DETECTED (>2s): modelDraw=%d | fpPaint=%d | fpUpdate=%d | rss=%dMB\n",
          ts, (int)modelDrawStage, (int)fpWidgetPaintStage, (int)fpUpdateStage, get_rss_mb());
        if (len > 0) {
          write(STDERR_FILENO, buf, len);
          
          // Write to kernel log to bypass eMMC filesystem locks
          int kmsg_fd = open("/dev/kmsg", O_WRONLY);
          if (kmsg_fd >= 0) { 
            write(kmsg_fd, buf, len); 
            close(kmsg_fd); 
          }
          
          int fd = open("/data/ui_crash.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
          if (fd >= 0) { 
            write(fd, buf, len); 
            log_thread_stack(fd);
            close(fd); 
          }

          // Proactive restart: don't wait for manager's 5s watchdog SIGKILL.
          // Exit now to trigger an immediate restart, which is safer for control logic.
          _exit(0);
        }
        std::this_thread::sleep_for(std::chrono::seconds(5)); // Avoid spamming during stall
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  });
  monitor.detach();

  MainWindow w;
  setMainWindow(&w);
  a.installEventFilter(&w);
  return a.exec();
}
