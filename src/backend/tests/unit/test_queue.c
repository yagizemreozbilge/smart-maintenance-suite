#include <stdio.h>
#include <assert.h>
#include "../../core/data_structures/queue.h"

void test_queue_init() {
  Queue q;
  initQueue(&q);
  assert(q.head == 0);
  assert(q.tail == 0);
  assert(q.count == 0);
  printf("Queue Init Test Passed\n");
}

void test_queue_enqueue_dequeue() {
  Queue q;
  initQueue(&q);
  SensorData d1 = {1, 25.5, 1000};
  SensorData d2 = {2, 26.0, 1001};
  assert(enqueue(&q, d1) == true);
  assert(getQueueSize(&q) == 1);
  assert(enqueue(&q, d2) == true);
  assert(getQueueSize(&q) == 2);
  SensorData out;
  assert(dequeue(&q, &out) == true);
  assert(out.sensor_id == 1);
  assert(getQueueSize(&q) == 1);
  assert(dequeue(&q, &out) == true);
  assert(out.sensor_id == 2);
  assert(getQueueSize(&q) == 0);
  assert(dequeue(&q, &out) == false);
  printf("Queue Enqueue/Dequeue Test Passed\n");
}

void run_queue_tests() {
  printf("Running Queue Tests...\n");
  test_queue_init();
  test_queue_enqueue_dequeue();
  printf("All Queue Tests Passed!\n");
}
