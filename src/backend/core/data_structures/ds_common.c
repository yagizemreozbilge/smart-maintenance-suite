/* This file is a template for ds_common.c. Content will be filled by yagiz on 2025-12-29. */
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "ds_common.h"

void printSectionHeader(const char *title) {
  printf("\n========================================\n");
  printf("   %s\n", title);
  printf("========================================\n");
}

void getCurrentTimeStamp(char *buffer, int bufferSize) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  // Format: 2025-01-01 12:00:00
  strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S", t);
}

void logMessage(const char *module_name, const char *message) {
  char timeBuf[30];
  getCurrentTimeStamp(timeBuf, 30);
  printf("[%s] [%-10s]: %s\n", timeBuf, module_name, message);
}
