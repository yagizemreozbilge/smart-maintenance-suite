/*
 * db_connection.h
 * ----------------
 * Public interface for the PostgreSQL connection‑pool used in the
 * smart‑maintenance‑suite backend.  The header contains only the
 * declarations that other modules need; the implementation lives in
 * db_connection.c.
 */

#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <stdbool.h>
#include <libpq-fe.h>
#include <pthread.h>

/* ------------------------------------------------------------ */
/* 1.  Exhaustion policy enum                                   */
/* ------------------------------------------------------------ */
typedef enum {
  BLOCK_WITH_TIMEOUT,   /* Wait up to a timeout, then fail */
  QUEUE_REQUESTS,       /* Enqueue the caller and wake later   */
  FAIL_FAST             /* Return NULL immediately            */
} PoolExhaustionPolicy;

/* ------------------------------------------------------------ */
/* 2.  Configuration structure – typically filled from .env    */
/* ------------------------------------------------------------ */
typedef struct {
  char host[64];
  int  port;
  char dbname[64];
  char user[64];
  char password[64];   /* kept only while building the conn string */
  char sslmode[16];
  int  connect_timeout; /* seconds */
} DatabaseConfig;

/* ------------------------------------------------------------ */
/* 3.  Per‑connection wrapper                                   */
/* ------------------------------------------------------------ */
typedef struct {
  PGconn *pg_conn;   /* libpq connection handle */
  bool    in_use;    /* true while handed out */
  int     index;     /* position in the pool array */
} DBConnection;

/* ------------------------------------------------------------ */
/* 4.  Opaque pool handle – callers never see the internals      */
/* ------------------------------------------------------------ */
typedef struct ConnectionPool ConnectionPool;

/* ------------------------------------------------------------ */
/* 5.  Public API                                               */
/* ------------------------------------------------------------ */
/**
 * Initialise a connection pool.
 * @param cfg          Database configuration.
 * @param policy       Behaviour when the pool is exhausted.
 * @param timeout_ms   Timeout (ms) used only with BLOCK_WITH_TIMEOUT.
 * @return Pointer to a pool instance, or NULL on fatal error.
 */
ConnectionPool *db_pool_init(const DatabaseConfig *cfg,
                             PoolExhaustionPolicy policy,
                             int timeout_ms);

/** Acquire a connection from the pool. */
DBConnection *db_pool_acquire(void);

/** Release a previously‑acquired connection back to the pool. */
void db_pool_release(DBConnection *conn);

/** Destroy the pool and free all resources. */
void db_pool_destroy(void);

#endif /* DB_CONNECTION_H */
