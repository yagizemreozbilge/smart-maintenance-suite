#include "jwt.h"
#include "db_connection.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Basit bir Secret Key - Gerçek projelerde bu .env dosyasında olur
#define APP_SECRET_KEY "maintenance_secret_2026"

char *generate_auth_token(int user_id, const char *username, const char *role) {
  char *token = (char *)malloc(128);
  time_t now = time(NULL);
  long expires = (long)now + (3600 * 24); // 24 saat geçerli
  // Student-style "Homemade" JWT: user:role:expires:hash
  // Daha sonra bunu base64 yaparak daha şık hale getirebiliriz.
  snprintf(token, 128, "token_%d_%ld_%s", user_id, expires, role);
  return token;
}

int verify_user_credentials(const char *username, const char *password, char *out_role) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return -1;

  char query[512];
  snprintf(query, sizeof(query),
           "SELECT id, role FROM users WHERE username = '%s' AND password_hash = '%s';",
           username, password);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
    LOG_ERROR("Login failed for user: %s", username);
    PQclear(res);
    db_pool_release(conn_wrapper);
    return -1;
  }

  int uid = atoi(PQgetvalue(res, 0, 0));

  if (out_role) {
    strncpy(out_role, PQgetvalue(res, 0, 1), 19);
    out_role[19] = '\0';
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  LOG_INFO("User %s (ID: %d) successfully logged in.", username, uid);
  return uid;
}

bool validate_auth_token(const char *token_str) {
  if (!token_str || strlen(token_str) < 10) return false;

  // Basit validation: "token_" ile başlamalı
  if (strncmp(token_str, "token_", 6) == 0) {
    return true;
  }

  return false;
}
