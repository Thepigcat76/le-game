#include "../include/log.h"
#include "lilc/ansi.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static void log_level_print(LogLevel level, char *buf) {
  switch (level) {
  case LOG_LEVEL_DEBUG: {
    strcpy(buf, ANSI_GREEN "DEBUG" ANSI_RESET);
    break;
  }
  case LOG_LEVEL_INFO: {
    strcpy(buf, ANSI_BLUE "INFO" ANSI_RESET);
    break;
  }
  case LOG_LEVEL_WARN: {
    strcpy(buf, ANSI_YELLOW "WARN" ANSI_RESET);
    break;
  }
  case LOG_LEVEL_ERROR: {
    strcpy(buf, ANSI_RED "ERROR" ANSI_RESET);
    break;
  }
  }
}

#if defined(__GNUC__) || defined(__clang__)
__attribute__((format(printf, 2, 3)))
#endif
void log_msg(LogLevel level, const char *format, ...) {
  char level_buf[128];
  log_level_print(level, level_buf);
  printf("[%s] ", level_buf);

  va_list ap;
  va_start(ap, format);
  vprintf(format, ap);
  va_end(ap);

  puts("");
}
