/* This file is a template for queue.c. Content will be filled by yagiz on 2025-12-29. */
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Internal Helper for Mutex
static void lockMutex(Mutex *m) {
#if defined(_WIN32) || defined(_WIN64)
  EnterCriticalSection(m);
#else
  pthread_mutex_lock(m);
#endif
}

static void unlockMutex(Mutex *m) {
#if defined(_WIN32) || defined(_WIN64)
  LeaveCriticalSection(m);
#else
  pthread_mutex_unlock(m);
#endif
}

static void initMutex(Mutex *m) {
#if defined(_WIN32) || defined(_WIN64)
  InitializeCriticalSection(m);
#else
  pthread_mutex_init(m, NULL);
#endif
}

static void destroyMutex(Mutex *m) {
#if defined(_WIN32) || defined(_WIN64)
  DeleteCriticalSection(m);
#else
  pthread_mutex_destroy(m);
#endif
}

// Function to initialize the queue
void initQueue(Queue *q) {
  q->head = 0;
  q->tail = 0;
  q->count = 0;
  // Initialize standard mutex
  initMutex(&q->lock);
}

// Helper to destroy queue resources
void destroyQueue(Queue *q) {
  destroyMutex(&q->lock);
}

// Is Queue empty? (Not thread-safe on its own, usually called within lock or for checks)
// But for strict safety, we can lock it too.
bool isQueueEmpty(Queue *q) {
  return (q->count == 0);
}

// Is Queue full?
bool isQueueFull(Queue *q) {
  return (q->count == MAX_QUEUE_SIZE);
}

// Enqueue element (Thread-safe)
bool enqueue(Queue *q, SensorData data) {
  lockMutex(&q->lock);

  if (isQueueFull(q)) {
    printf("ERROR: Queue is full! Data lost: %.2f\n", data.value);
    unlockMutex(&q->lock);
    return false;
  }

  // Write data to tail
  q->items[q->tail] = data;
  // Circular Buffer logic
  q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
  q->count++;
  unlockMutex(&q->lock);
  return true;
}

// Dequeue element (Thread-safe)
bool dequeue(Queue *q, SensorData *outData) {
  lockMutex(&q->lock);

  if (isQueueEmpty(q)) {
    // printf("Queue is empty.\n"); // Logging might be noisy
    unlockMutex(&q->lock);
    return false;
  }

  // Read data from head
  *outData = q->items[q->head];
  // Circular Buffer logic
  q->head = (q->head + 1) % MAX_QUEUE_SIZE;
  q->count--;
  unlockMutex(&q->lock);
  return true;
}

int getQueueSize(Queue *q) {
  int size;
  lockMutex(&q->lock);
  size = q->count;
  unlockMutex(&q->lock);
  return size;
}
