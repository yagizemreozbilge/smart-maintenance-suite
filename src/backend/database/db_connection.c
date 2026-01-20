/*
 * db_connection.c
 * ----------------
 * A learning‑focused implementation of a PostgreSQL connection pool.
 * The code is heavily commented so that a newcomer can follow each
 * design decision and understand how the pool works in a real‑world
 * factory setting (50+ machines sending sensor data).
 *
 * NOTE: This file pairs with `db_connection.h` which declares the public
 * API and data structures.
 */

/* ------------------------------------------------------------ */
/* 1.  Standard library includes                                 */
/* ------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <libpq-fe.h>          /* libpq – PostgreSQL C client library */
#include "logger.h"
#include "db_connection.h"


/* ------------------------------------------------------------ */
/* 5.  The pool structure (private – defined here for simplicity) */
/* ------------------------------------------------------------ */
struct ConnectionPool {
  DBConnection *connections;          /* array of size max_size */
  int           max_size;            /* e.g. 25 */
  int           min_size;            /* e.g. 5  */
  int           used_cnt;            /* how many are currently in_use */
  int          *free_stack;          /* stack of free indices */
  int           free_top;            /* index of the top element */
  size_t acquire_cnt;
  size_t release_cnt;
  double total_wait_ms;
  pthread_mutex_t lock;              /* protects all mutable fields */
  pthread_cond_t  free_cond;         /* signalled when a slot becomes free */
  PoolExhaustionPolicy policy;       /* chosen by the integrator */
  int           timeout_ms;          /* used when policy == BLOCK_WITH_TIMEOUT */
  DatabaseConfig cfg;                /* stored so we can lazily create connections */
};

/* --- FIFO Queue Structure for Waiting Threads --- */
typedef struct WaitNode {
  pthread_cond_t cond;
  struct WaitNode *next;
} WaitNode;

static WaitNode *wait_head = NULL;
static WaitNode *wait_tail = NULL;

/* ------------------------------------------------------------ */
/* 6.  Global pool instance – one pool per process                */
/* ------------------------------------------------------------ */
static ConnectionPool g_pool = {0};

/* ------------------------------------------------------------ */
/* 7.  Helper: build a libpq connection string from DatabaseConfig */
/* ------------------------------------------------------------ */
static char *build_conn_str(const DatabaseConfig *cfg) {
  /* The string is allocated on the heap; the caller must free it. */
  size_t needed = 256 + strlen(cfg->host) + strlen(cfg->dbname) +
                  strlen(cfg->user) + strlen(cfg->password) + strlen(cfg->sslmode);
  char *buf = (char *)malloc(needed);

  if (!buf) return NULL;

  snprintf(buf, needed,
           "host=%s port=%d dbname=%s user=%s password=%s sslmode=%s connect_timeout=%d",
           cfg->host, cfg->port, cfg->dbname, cfg->user, cfg->password,
           cfg->sslmode, cfg->connect_timeout);
  return buf;
}

/* ------------------------------------------------------------ */
/* 8.  Stack utilities – very small, lock‑free because the pool lock
 *    already protects them.
 * ------------------------------------------------------------ */
static inline void stack_push(int *stack, int *top, int value) {
  stack[(*top)++] = value;   /* store then increment */
}

static inline int stack_pop(int *stack, int *top) {
  return stack[--(*top)];    /* decrement then retrieve */
}

static inline bool stack_is_empty(int top) {
  return top == 0;
}

/* ------------------------------------------------------------ */
/* 9.  Public API – initialise the pool                         */
/* ------------------------------------------------------------ */
ConnectionPool *db_pool_init(const DatabaseConfig *cfg,
                             PoolExhaustionPolicy policy,
                             int timeout_ms) {
  /* 1️ Allocate the connection array */
  g_pool.acquire_cnt = 0;
  g_pool.release_cnt = 0;
  g_pool.total_wait_ms = 0;
  // Konfigürasyondan boyutları oku, belirtilmemişse varsayılanları kullan
  g_pool.max_size = (cfg->pool_max > 0) ? cfg->pool_max : 25;
  g_pool.min_size = (cfg->pool_min > 0) ? cfg->pool_min : 5;
  g_pool.used_cnt = 0;
  g_pool.policy    = policy;
  g_pool.timeout_ms = timeout_ms;
  g_pool.cfg = *cfg;             /* copy – we own the data */
  g_pool.connections = calloc(g_pool.max_size, sizeof(DBConnection));

  if (!g_pool.connections) return NULL;

  /* 2️ Allocate the free‑slot stack (size = max_size) */
  g_pool.free_stack = malloc(g_pool.max_size *sizeof(int));

  if (!g_pool.free_stack) {
    free(g_pool.connections);
    return NULL;
  }

  g_pool.free_top = 0;
  /* 3️ Initialise mutex / condition variable */
  pthread_mutex_init(&g_pool.lock, NULL);
  pthread_cond_init(&g_pool.free_cond, NULL);
  /* 4 Eagerly create the *minimum* number of connections */
  char *conn_str = build_conn_str(&g_pool.cfg);

  if (!conn_str) return NULL;   /* out‑of‑memory */

  for (int i = 0; i < g_pool.min_size; ++i) {
    PGconn *c = PQconnectdb(conn_str);

    if (PQstatus(c) != CONNECTION_OK) {
      LOG_ERROR("[db_pool_init] failed to connect: %s", PQerrorMessage(c));
      PQfinish(c);
      continue;   /* we keep the slot NULL – lazy creation later */
    }

    g_pool.connections[i].pg_conn = c;
    g_pool.connections[i].in_use = false;
    g_pool.connections[i].index   = i;
    stack_push(g_pool.free_stack, &g_pool.free_top, i);
  }

  free(conn_str);

  /* 5️ Remaining slots stay NULL (lazy) */
  for (int i = g_pool.min_size; i < g_pool.max_size; ++i) {
    g_pool.connections[i].pg_conn = NULL;
    g_pool.connections[i].in_use = false;
    g_pool.connections[i].index   = i;
  }

  return &g_pool;
}

/* ------------------------------------------------------------ */
/* 10. Acquire a connection – respects the exhaustion policy      */
/* ------------------------------------------------------------ */
DBConnection *db_pool_acquire(void) {
  pthread_mutex_lock(&g_pool.lock);

  /* Fast path – a free slot is already on the stack */
  if (!stack_is_empty(g_pool.free_top)) {
    int idx = stack_pop(g_pool.free_stack, &g_pool.free_top);
    DBConnection *c = &g_pool.connections[idx];
    c->in_use = true;
    g_pool.used_cnt++;
    pthread_mutex_unlock(&g_pool.lock);
    return c;
  }

  /* No free slot – try lazy creation if we still have capacity */
  for (int i = g_pool.min_size; i < g_pool.max_size; ++i) {
    if (g_pool.connections[i].pg_conn == NULL) {
      char *cs = build_conn_str(&g_pool.cfg);

      if (!cs) break;   /* out‑of‑memory */

      PGconn *c = PQconnectdb(cs);
      free(cs);

      if (PQstatus(c) != CONNECTION_OK) {
        LOG_ERROR("[db_pool_acquire] lazy connect failed: %s", PQerrorMessage(c));
        PQfinish(c);
        continue;   /* treat as exhausted for now */
      }

      g_pool.connections[i].pg_conn = c;
      g_pool.connections[i].in_use = true;
      g_pool.connections[i].index   = i;
      g_pool.used_cnt++;
      pthread_mutex_unlock(&g_pool.lock);
      return &g_pool.connections[i];
    }
  }

  /* -------------------------------------------------------- */
  /* 3 Pool exhausted – apply the selected policy            */
  /* -------------------------------------------------------- */
  switch (g_pool.policy) {
    case BLOCK_WITH_TIMEOUT: {
      /* Dynamically adjust timeout based on current load.
       * The more connections in use, the longer we are willing to wait.
       * Simple heuristic: base_timeout * (1 + used_cnt / max_size). */
      double load_factor = (double)g_pool.used_cnt / (double)g_pool.max_size;
      int effective_timeout_ms = (int)(g_pool.timeout_ms * (1.0 + load_factor));
      struct timespec ts;
      clock_gettime(CLOCK_REALTIME, &ts);
      ts.tv_sec  += effective_timeout_ms / 1000;
      ts.tv_nsec += (effective_timeout_ms % 1000) * 1000000;

      if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
      }

      /* Measure how long we actually waited. */
      struct timespec wait_start, wait_end;
      clock_gettime(CLOCK_REALTIME, &wait_start);
      int rc = 0;

      while (rc != ETIMEDOUT && stack_is_empty(g_pool.free_top)) {
        rc = pthread_cond_timedwait(&g_pool.free_cond, &g_pool.lock, &ts);
      }

      clock_gettime(CLOCK_REALTIME, &wait_end);
      double waited_ms = (wait_end.tv_sec - wait_start.tv_sec) * 1000.0 +
                         (wait_end.tv_nsec - wait_start.tv_nsec) / 1000000.0;
      g_pool.total_wait_ms += waited_ms;

      if (!stack_is_empty(g_pool.free_top)) {
        int idx = stack_pop(g_pool.free_stack, &g_pool.free_top);
        DBConnection *c = &g_pool.connections[idx];
        c->in_use = true;
        g_pool.used_cnt++;
        g_pool.acquire_cnt++;
        pthread_mutex_unlock(&g_pool.lock);
        return c;
      }

      /* timeout – fall through to failure */
      break;
    }

    case QUEUE_REQUESTS: {
      /* FIFO Bekleme Kuyruğu Uygulaması:
       * Her thread kendi condition variable'ı ile kuyruğa eklenir. */
      WaitNode *my_node = malloc(sizeof(WaitNode));
      pthread_cond_init(&my_node->cond, NULL);
      my_node->next = NULL;

      if (wait_tail == NULL) {
        wait_head = my_node;
      } else {
        wait_tail->next = my_node;
      }

      wait_tail = my_node;

      while (stack_is_empty(g_pool.free_top) || wait_head != my_node) {
        pthread_cond_wait(&my_node->cond, &g_pool.lock);
      }

      int idx = stack_pop(g_pool.free_stack, &g_pool.free_top);
      DBConnection *c = &g_pool.connections[idx];
      c->in_use = true;
      g_pool.used_cnt++;
      g_pool.acquire_cnt++;
      /* Kendini kuyruktan çıkar */
      wait_head = my_node->next;

      if (wait_head == NULL) wait_tail = NULL;

      pthread_cond_destroy(&my_node->cond);
      free(my_node);
      pthread_mutex_unlock(&g_pool.lock);
      return c;
    }

    case FAIL_FAST:
    default:
      /* Immediate failure – caller must decide what to do */
      break;
  }

  pthread_mutex_unlock(&g_pool.lock);
  return NULL;   /* pool exhausted and policy dictated failure */
}

/* ------------------------------------------------------------ */
/* 11. Release a connection back to the pool                     */
/* ------------------------------------------------------------ */
void db_pool_release(DBConnection *conn) {
  if (!conn) return;

  pthread_mutex_lock(&g_pool.lock);
  int idx = conn->index;
  conn->in_use = false;
  stack_push(g_pool.free_stack, &g_pool.free_top, idx);
  g_pool.used_cnt--;
  g_pool.release_cnt++;

  /* Kuyruktaki ilk kişiyi uyandır (FIFO) */
  if (wait_head != NULL) {
    pthread_cond_signal(&wait_head->cond);
  } else {
    /* Kimse kuyrukta değilse BLOCK_WITH_TIMEOUT bekleyenleri uyar */
    pthread_cond_signal(&g_pool.free_cond);
  }

  pthread_mutex_unlock(&g_pool.lock);
}

/* ------------------------------------------------------------ */
/* 12. Destroy the pool – close all libpq connections          */
/* ------------------------------------------------------------ */
void db_pool_destroy(void) {
  /* Student‑style cleanup: close all libpq connections and free memory. */
  pthread_mutex_lock(&g_pool.lock);

  for (int i = 0; i < g_pool.max_size; ++i) {
    if (g_pool.connections[i].pg_conn) {
      PQfinish(g_pool.connections[i].pg_conn);
      g_pool.connections[i].pg_conn = NULL;
    }
  }

  free(g_pool.connections);
  free(g_pool.free_stack);
  pthread_mutex_unlock(&g_pool.lock);
  pthread_mutex_destroy(&g_pool.lock);
  pthread_cond_destroy(&g_pool.free_cond);
  memset(&g_pool, 0, sizeof(g_pool));
}

/* ------------------------------------------------------------ */
/* 12.5 Get current metrics                                     */
/* ------------------------------------------------------------ */
PoolMetrics db_pool_get_metrics(void) {
  PoolMetrics m;
  pthread_mutex_lock(&g_pool.lock);
  m.acquire_cnt = g_pool.acquire_cnt;
  m.release_cnt = g_pool.release_cnt;
  m.used_cnt    = g_pool.used_cnt;
  m.total_wait_ms = g_pool.total_wait_ms;
  pthread_mutex_unlock(&g_pool.lock);
  return m;
}

/* ------------------------------------------------------------ */
/* 13. Example usage (commented out – keep it for learning)   */
/* ------------------------------------------------------------ */
/*
int main(void)
{
    DatabaseConfig cfg = {"localhost", 5432, "factorydb", "admin", "secret", "require", 5, 5, 25};
    ConnectionPool *p = db_pool_init(&cfg, BLOCK_WITH_TIMEOUT, 200);
    if (!p) { LOG_ERROR("Failed to init pool"); return 1; }

    DBConnection *c = db_pool_acquire();
    if (!c) { LOG_ERROR("Could not acquire connection"); return 1; }

    // Use the connection, e.g. PQexec(c->pg_conn, "SELECT 1");

    db_pool_release(c);
    db_pool_destroy();
    return 0;
}
*/
