/* This file is a template for ds_common.h. Content will be filled by yagiz on 2025-12-29. */
#ifndef DS_COMMON_H
#define DS_COMMON_H

#include <stdbool.h>

// Utility Functions used across all Data Structures

// Print a formatted header for console output
void printSectionHeader(const char *title);

// Get current time as string (Format: YYYY-MM-DD HH:MM:SS)
void getCurrentTimeStamp(char *buffer, int bufferSize);

// Simple logging helper with timestamp
void logMessage(const char *module_name, const char *message);

#endif
