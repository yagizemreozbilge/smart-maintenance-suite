/* This file is a template for logger.c. Content will be filled by yagiz on 2025-12-29. */
#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static LogLevel current_log_level = LOG_LEVEL_INFO;

// Zaman damgası al
static void print_timestamp(FILE *out) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  fprintf(out, "[%02d:%02d:%02d] ", t->tm_hour, t->tm_min, t->tm_sec);
}

void logger_set_level(LogLevel level) {
  current_log_level = level;
}

LogLevel logger_get_level(void) {
  return current_log_level;
}

void log_error(const char *format, ...) {
  if (current_log_level >= LOG_LEVEL_ERROR) {
    va_list args;
    print_timestamp(stderr);
    fprintf(stderr, "[ERROR] ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
  }
}

void log_warn(const char *format, ...) {
  if (current_log_level >= LOG_LEVEL_WARN) {
    va_list args;
    print_timestamp(stderr);
    fprintf(stderr, "[WARN] ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
  }
}

void log_info(const char *format, ...) {
  if (current_log_level >= LOG_LEVEL_INFO) {
    va_list args;
    print_timestamp(stdout);
    fprintf(stdout, "[INFO] ");
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
  }
}

void log_debug(const char *format, ...) {
  if (current_log_level >= LOG_LEVEL_DEBUG) {
    va_list args;
    print_timestamp(stdout);
    fprintf(stdout, "[DEBUG] ");
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
  }
}
