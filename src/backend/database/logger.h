#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
#include <time.h>
// ERROR Logu: Her zaman STDERR'ye (hata kanalına) basar ve zaman ekler
#define LOG_ERROR(msg, ...) { \
    time_t t = time(NULL); \
    char *ts = ctime(&t); \
    ts[24] = '\0'; /* Satır sonundaki \n karakterini temizle */ \
    fprintf(stderr, "[%s] [ERROR] " msg "\n", ts, ##__VA_ARGS__); \
  }
// INFO Logu: Bilgilendirme amaçlı STDOUT'a basar
#define LOG_INFO(msg, ...) { \
    printf("[INFO] " msg "\n", ##__VA_ARGS__); \
  }
#endif
