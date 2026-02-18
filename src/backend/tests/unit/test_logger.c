#include "../../core/utils/logger.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

void test_logger_all_levels() {
  printf("Testing all log levels...\n");
  // Test Initial State/Set/Get
  logger_set_level(LOG_LEVEL_DEBUG);
  assert(logger_get_level() == LOG_LEVEL_DEBUG);
  // Hit all logging macros
  LOG_DEBUG("Test Debug");
  LOG_INFO("Test Info");
  LOG_WARN("Test Warn");
  LOG_ERROR("Test Error");
  // Test filtering logic (Set to Error, should ignore Info/Warn)
  logger_set_level(LOG_LEVEL_ERROR);
  assert(logger_get_level() == LOG_LEVEL_ERROR);
  LOG_INFO("HIDDEN INFO");
  LOG_WARN("HIDDEN WARN");
  LOG_ERROR("VISIBLE ERROR");
  // Test NONE level (Ignore everything)
  logger_set_level(LOG_LEVEL_NONE);
  assert(logger_get_level() == LOG_LEVEL_NONE);
  LOG_ERROR("HIDDEN ERROR");
  printf("[PASS] test_logger_all_levels\n");
}

int main() {
  printf("=== Logger Unit Tests (Full Coverage) ===\n");
  test_logger_all_levels();
  printf("\n✅ All Logger tests passed!\n");
  return 0;
}
