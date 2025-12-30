/* This file is a template for queue.h. Content will be filled by yagiz on 2025-12-29. */
#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "../../common/types.h"

// Check if Windows
#if defined(_WIN32) || defined(_WIN64)
  #include <windows.h>
  typedef CRITICAL_SECTION Mutex;
#else
  #include <pthread.h>
  typedef pthread_mutex_t Mutex;
#endif

// Fixed size for factory scenario
#define MAX_QUEUE_SIZE 100

typedef struct {
  // Fixed array to hold data (Array Implementation)
  SensorData items[MAX_QUEUE_SIZE];

  int head;  // Read index
  int tail;  // Write index
  int count; // Current element count

  // Mutex for Thread Safety
  Mutex lock;
} Queue;

// Function Prototypes
void initQueue(Queue *q);
bool isQueueEmpty(Queue *q);
bool isQueueFull(Queue *q);
bool enqueue(Queue *q, SensorData data);
bool dequeue(Queue *q, SensorData *outData); // Extracts data via pointer
int getQueueSize(Queue *q);

// Helper for cleaning up
void destroyQueue(Queue *q);

#endif
