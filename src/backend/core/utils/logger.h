#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>

// ============= DOĞRUDAN ÇALIŞAN MAKROLAR =============
// Bu makrolar fonksiyon çağırmadan direkt çalışır!

#define LOG_ERROR(fmt, ...) \
  fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
  fprintf(stderr, "[WARN] " fmt "\n", ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) \
  printf("[INFO] " fmt "\n", ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...) \
  do { if (0) {} } while(0)  /* DEBUG kapalı, hiçbir şey yapma */

#endif
