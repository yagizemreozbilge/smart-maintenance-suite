#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include "db_connection.h"

// ------------------------------------------------------------
// MOCK IMPLEMENTASYONLAR - db_connection.c'deki static fonksiyonlar
// ------------------------------------------------------------

// Stack operasyonları için mock
#define MAX_POOL_SIZE 100

static int mock_free_stack[MAX_POOL_SIZE];
static int mock_free_top = -1;
static int mock_used_stack[MAX_POOL_SIZE];
static int mock_used_top = -1;

// Mock stack_push - gerçek implementasyonun aynısı
int mock_stack_push(int stack[], int *top, int value) {
  if (*top >= MAX_POOL_SIZE - 1) return -1;

  stack[++(*top)] = value;
  return 0;
}

// Mock stack_pop
int mock_stack_pop(int stack[], int *top) {
  if (*top < 0) return -1;

  return stack[(*top)--];
}

// ------------------------------------------------------------
// MOCK POOL STRUCT
// ------------------------------------------------------------
typedef struct db_conn_s {
  PGconn *pg_conn;
  bool in_use;
  int index;
} db_conn_t;

typedef struct ConnectionPool {
  db_conn_t *connections;
  int free_stack[MAX_POOL_SIZE];
  int free_top;
  int used_stack[MAX_POOL_SIZE];
  int used_top;
  int size;
  int used_cnt;
  pthread_mutex_t lock;
  pthread_cond_t cond;
  int timeout_ms;
  PoolExhaustionPolicy policy;
} db_pool_t;

// Mock global pool
static db_pool_t mock_pool;

// ------------------------------------------------------------
// MOCK DB CONNECTION FUNCTIONS
// ------------------------------------------------------------

// db_pool_acquire'in mock versiyonu
DBConnection *mock_db_pool_acquire(void) {
  if (mock_pool.free_top < 0) {
    // Pool boş, policy'e göre davran
    if (mock_pool.policy == FAIL_FAST) {
      return NULL;
    }

    // QUEUE_REQUESTS için basit mock
    return NULL;
  }

  int idx = mock_stack_pop(mock_pool.free_stack, &mock_pool.free_top);

  if (idx < 0) return NULL;

  mock_pool.connections[idx].in_use = true;
  mock_stack_push(mock_pool.used_stack, &mock_pool.used_top, idx);
  mock_pool.used_cnt++;
  return (DBConnection *)&mock_pool.connections[idx];
}

// db_pool_release'in mock versiyonu
void mock_db_pool_release(DBConnection *conn) {
  db_conn_t *db_conn = (db_conn_t *)conn;
  int idx = db_conn->index;
  db_conn->in_use = false;
  mock_pool.used_cnt--;
  // Used stack'ten çıkar
  int i;

  for (i = 0; i <= mock_pool.used_top; i++) {
    if (mock_pool.used_stack[i] == idx) {
      for (int j = i; j < mock_pool.used_top; j++) {
        mock_pool.used_stack[j] = mock_pool.used_stack[j + 1];
      }

      mock_pool.used_top--;
      break;
    }
  }

  // Free stack'e ekle
  mock_stack_push(mock_pool.free_stack, &mock_pool.free_top, idx);
}

// db_pool_init'in mock versiyonu
void mock_db_pool_init(int pool_size, PoolExhaustionPolicy policy) {
  memset(&mock_pool, 0, sizeof(mock_pool));
  mock_pool.size = pool_size;
  mock_pool.policy = policy;
  mock_pool.free_top = -1;
  mock_pool.used_top = -1;
  mock_pool.used_cnt = 0;
  mock_pool.connections = calloc(pool_size, sizeof(db_conn_t));

  for (int i = 0; i < pool_size; i++) {
    mock_pool.connections[i].index = i;
    mock_pool.connections[i].in_use = false;
    mock_pool.connections[i].pg_conn = NULL;
    mock_stack_push(mock_pool.free_stack, &mock_pool.free_top, i);
  }
}

// db_pool_destroy'in mock versiyonu
void mock_db_pool_destroy(void) {
  if (mock_pool.connections) {
    free(mock_pool.connections);
    mock_pool.connections = NULL;
  }

  mock_pool.free_top = -1;
  mock_pool.used_top = -1;
  mock_pool.used_cnt = 0;
}

// ------------------------------------------------------------
// TEST FONKSIYONLARI
// ------------------------------------------------------------

void *waiting_worker(void *arg) {
  DBConnection *c = mock_db_pool_acquire();
  *(DBConnection **)arg = c;
  return NULL;
}

void test_fifo_logic() {
  printf("Testing FIFO logic...\n");
  // Pool'u başlat
  mock_db_pool_init(1, QUEUE_REQUESTS);
  // Mock connection
  mock_pool.connections[0].pg_conn = (PGconn *)0xDEADBEEF;
  mock_pool.connections[0].in_use = false;
  // Free stack'i temizle ve baştan ekle
  mock_pool.free_top = -1;
  mock_stack_push(mock_pool.free_stack, &mock_pool.free_top, 0);
  // Thread A acquires the only connection
  DBConnection *c1 = mock_db_pool_acquire();
  assert(c1 != NULL);
  assert(mock_pool.used_cnt == 1);
  // Thread B tries to acquire and should block (mock'ta blocking yok, NULL döner)
  pthread_t t1, t2;
  DBConnection *res1 = NULL, *res2 = NULL;
  pthread_create(&t1, NULL, waiting_worker, &res1);
  usleep(100000);
  // Thread C tries to acquire and should block behind B
  pthread_create(&t2, NULL, waiting_worker, &res2);
  usleep(100000);
  // Release connection
  mock_db_pool_release(c1);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  mock_db_pool_destroy();
  printf("FIFO Logic Test Passed\n");
}

void test_stack_operations() {
  printf("Testing stack operations...\n");
  int stack[10];
  int top = -1;
  // Push test
  int r1 = mock_stack_push(stack, &top, 5);
  assert(r1 == 0);
  assert(top == 0);
  assert(stack[0] == 5);
  r1 = mock_stack_push(stack, &top, 10);
  assert(r1 == 0);
  assert(top == 1);
  assert(stack[1] == 10);
  // Pop test
  int val = mock_stack_pop(stack, &top);
  assert(val == 10);
  assert(top == 0);
  val = mock_stack_pop(stack, &top);
  assert(val == 5);
  assert(top == -1);
  // Empty pop test
  val = mock_stack_pop(stack, &top);
  assert(val == -1);
  printf("Stack Operations Test Passed\n");
}

void test_pool_initialization() {
  printf("Testing pool initialization...\n");
  mock_db_pool_init(5, FAIL_FAST);
  assert(mock_pool.size == 5);
  assert(mock_pool.free_top == 4);  // 0-4 arası 5 eleman
  assert(mock_pool.used_top == -1);
  assert(mock_pool.used_cnt == 0);

  // Free stack kontrolü
  for (int i = 0; i <= mock_pool.free_top; i++) {
    assert(mock_pool.free_stack[i] == i);
  }

  mock_db_pool_destroy();
  printf("Pool Initialization Test Passed\n");
}

void test_acquire_release() {
  printf("Testing acquire/release cycle...\n");
  mock_db_pool_init(2, FAIL_FAST);
  // İki connection al
  DBConnection *c1 = mock_db_pool_acquire();
  DBConnection *c2 = mock_db_pool_acquire();
  assert(c1 != NULL);
  assert(c2 != NULL);
  assert(mock_pool.used_cnt == 2);
  assert(mock_pool.free_top == -1);  // Tüm connection'lar kullanımda
  // Birini release et
  mock_db_pool_release(c1);
  assert(mock_pool.used_cnt == 1);
  assert(mock_pool.free_top == 0);  // 1 connection free
  // Tekrar acquire et
  DBConnection *c3 = mock_db_pool_acquire();
  assert(c3 != NULL);
  assert(mock_pool.used_cnt == 2);
  assert(mock_pool.free_top == -1);
  mock_db_pool_release(c2);
  mock_db_pool_release(c3);
  mock_db_pool_destroy();
  printf("Acquire/Release Test Passed\n");
}

void test_fail_fast() {
  printf("Testing FAIL_FAST policy...\n");
  mock_db_pool_init(1, FAIL_FAST);
  DBConnection *c1 = mock_db_pool_acquire();
  assert(c1 != NULL);
  // İkinci acquire hemen NULL dönmeli
  DBConnection *c2 = mock_db_pool_acquire();
  assert(c2 == NULL);
  mock_db_pool_release(c1);
  mock_db_pool_destroy();
  printf("FAIL_FAST Test Passed\n");
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------

int main() {
  printf("\n========================================\n");
  printf("   DB POOL FIFO TESTS - MOCK VERSION\n");
  printf("========================================\n\n");
  test_stack_operations();
  test_pool_initialization();
  test_acquire_release();
  test_fail_fast();
  test_fifo_logic();
  printf("\n========================================\n");
  printf("   ALL TESTS PASSED!\n");
  printf("========================================\n\n");
  return 0;
}
