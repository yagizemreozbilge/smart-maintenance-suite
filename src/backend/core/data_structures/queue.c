/* This file is a template for queue.c. Content will be filled by yagiz on 2025-12-29. */
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Function to initialize the queue
void initQueue(Queue *q) {
  q->head = 0;
  q->tail = 0;
  q->count = 0;
  q->lock = 0; // Lock is open
}

// Is Queue empty?
bool isQueueEmpty(Queue *q) {
  // Checking count is sufficient
  return (q->count == 0);
}

// Is Queue full?
bool isQueueFull(Queue *q) {
  return (q->count == MAX_QUEUE_SIZE);
}

// Enqueue element (Thread-safe simulation)
bool enqueue(Queue *q, SensorData data) {
  // 1. LOCK (Mutex Lock)
  // pthread_mutex_lock(&q->lock);
  if (isQueueFull(q)) {
    printf("ERROR: Queue is full! Data lost: %.2f\n", data.value);
    // UNLOCK
    return false;
  }

  // 2. Write data to tail
  q->items[q->tail] = data;
  // 3. Circular Buffer logic: Wrap around if at end
  q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
  // 4. Increment count
  q->count++;
  // 5. UNLOCK (Mutex Unlock)
  // pthread_mutex_unlock(&q->lock);
  return true;
}

// Dequeue element
bool dequeue(Queue *q, SensorData *outData) {
  // LOCK
  if (isQueueEmpty(q)) {
    printf("ERROR: Queue is empty, cannot read.\n");
    // UNLOCK
    return false;
  }

  // 1. Read data from head
  *outData = q->items[q->head];
  // 2. Circular Buffer logic: Advance head
  q->head = (q->head + 1) % MAX_QUEUE_SIZE;
  // 3. Decrement count
  q->count--;
  // UNLOCK
  return true;
}

int getQueueSize(Queue *q) {
  return q->count;
}
