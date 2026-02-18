#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

/**
 * @brief Log levels for the application
 */
typedef enum {
  LOG_LEVEL_NONE = 0,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WARN,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG
} LogLevel;

/**
 * @brief Set the current log level
 */
void logger_set_level(LogLevel level);

/**
 * @brief Get the current log level
 */
LogLevel logger_get_level(void);

/**
 * @brief Log functions for different levels
 */
void log_error(const char *format, ...);
void log_warn(const char *format, ...);
void log_info(const char *format, ...);
void log_debug(const char *format, ...);

// Legacy macro support for existing code
#define LOG_ERROR(fmt, ...) log_error(fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) log_warn(fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) log_info(fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) log_debug(fmt, ##__VA_ARGS__)

#endif
