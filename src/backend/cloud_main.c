// src/backend/cloud_main.c
// Bu dosya Cloud Run ortami icin main() saglar.


#include "database/db_connection.h"
#include "api/http_server.h"
#include "database/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static volatile int keep_running = 1;

void handle_signal(int sig) {
  (void)sig;
  keep_running = 0;
}

int main() {
  signal(SIGTERM, handle_signal);
  signal(SIGINT, handle_signal);
  // PORT env variable oku (Cloud Run bunu set eder)
  char *port_env = getenv("PORT");
  int port = port_env ? atoi(port_env) : 8080;
  DatabaseConfig cfg = {0};
  strcpy(cfg.host,     getenv("DB_HOST")     ? getenv("DB_HOST")     : "localhost");
  cfg.port           = getenv("DB_PORT")     ? atoi(getenv("DB_PORT")) : 5432;
  strcpy(cfg.dbname,   getenv("DB_NAME")     ? getenv("DB_NAME")     : "smart_maintenance");
  strcpy(cfg.user,     getenv("DB_USER")     ? getenv("DB_USER")     : "postgres");
  strcpy(cfg.password, getenv("DB_PASSWORD") ? getenv("DB_PASSWORD") : "");
  strcpy(cfg.sslmode,  getenv("DB_SSLMODE")  ? getenv("DB_SSLMODE")  : "require");
  cfg.connect_timeout = 5;
  cfg.pool_min = 2;
  cfg.pool_max = 5;

  if (!db_pool_init(&cfg, BLOCK_WITH_TIMEOUT, 2000)) {
    LOG_ERROR("DB pool init failed.");
    return EXIT_FAILURE;
  }

  if (!start_http_server(port)) {
    LOG_ERROR("HTTP server failed to start on port %d", port);
    db_pool_destroy();
    return EXIT_FAILURE;
  }

  LOG_INFO("Server running on port %d", port);

  // Cloud Run: process'i ayakta tut, terminal bekleme yok
  while (keep_running) {
    sleep(1);
  }

  stop_http_server();
  db_pool_destroy();
  return EXIT_SUCCESS;
}
