#pragma once

typedef enum {
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
} LogLevel;

#if defined(__GNUC__) || defined(__clang__)
__attribute__((format(printf, 2, 3)))
#endif
void log_msg(LogLevel level, const char *format, ...);

#define log_debug(fmt, ...) log_msg(LOG_LEVEL_DEBUG, fmt __VA_OPT__(,) __VA_ARGS__)

#define log_info(fmt, ...) log_msg(LOG_LEVEL_INFO, fmt __VA_OPT__(,) __VA_ARGS__)

#define log_warn(fmt, ...) log_msg(LOG_LEVEL_WARN, fmt __VA_OPT__(,) __VA_ARGS__)

#define log_error(fmt, ...) log_msg(LOG_LEVEL_ERROR, fmt __VA_OPT__(,) __VA_ARGS__)
