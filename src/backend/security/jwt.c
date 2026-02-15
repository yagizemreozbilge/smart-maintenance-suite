#include "jwt.h"
#include "db_connection.h"
#include <cjson/cJSON.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>



char *generate_auth_token(int user_id, const char *username, const char *role) {
  if (!username || !role) {
    fprintf(stderr, "generate_auth_token: NULL parameter\n");
    return NULL;
  }

  cJSON *json = cJSON_CreateObject();

  if (!json) {
    fprintf(stderr, "generate_auth_token: Failed to create JSON\n");
    return NULL;
  }

  cJSON_AddNumberToObject(json, "user_id", user_id);
  cJSON_AddStringToObject(json, "username", username);
  cJSON_AddStringToObject(json, "role", role);
  cJSON_AddNumberToObject(json, "exp", time(NULL) + 86400);
  char *json_str = cJSON_PrintUnformatted(json);
  cJSON_Delete(json);

  if (!json_str) {
    fprintf(stderr, "generate_auth_token: Failed to print JSON\n");
    return NULL;
  }

  char *token = malloc(strlen(json_str) + 20);

  if (!token) {
    fprintf(stderr, "generate_auth_token: Memory allocation failed\n");
    free(json_str);
    return NULL;
  }

  sprintf(token, "simple_jwt:%s", json_str);
  free(json_str);
  // LOG_DEBUG kaldırıldı
  return token;
}

bool validate_auth_token(const char *token_str) {
  if (!token_str) {
    return false;
  }

  if (strncmp(token_str, "simple_jwt:", 11) != 0) {
    return false;
  }

  const char *json_str = token_str + 11;
  cJSON *json = cJSON_Parse(json_str);

  if (!json) {
    return false;
  }

  cJSON *exp_item = cJSON_GetObjectItem(json, "exp");

  if (!cJSON_IsNumber(exp_item)) {
    cJSON_Delete(json);
    return false;
  }

  time_t now = time(NULL);
  bool valid = (exp_item->valuedouble > now);
  cJSON_Delete(json);
  return valid;
}

int get_user_id_from_token(const char *token_str) {
  if (!token_str || strncmp(token_str, "simple_jwt:", 11) != 0) {
    return -1;
  }

  const char *json_str = token_str + 11;
  cJSON *json = cJSON_Parse(json_str);

  if (!json) return -1;

  cJSON *user_id_item = cJSON_GetObjectItem(json, "user_id");
  int user_id = -1;

  if (cJSON_IsNumber(user_id_item)) {
    user_id = user_id_item->valueint;
  }

  cJSON_Delete(json);
  return user_id;
}

char *get_role_from_token(const char *token_str) {
  if (!token_str || strncmp(token_str, "simple_jwt:", 11) != 0) {
    return NULL;
  }

  const char *json_str = token_str + 11;
  cJSON *json = cJSON_Parse(json_str);

  if (!json) return NULL;

  cJSON *role_item = cJSON_GetObjectItem(json, "role");
  char *role = NULL;

  if (cJSON_IsString(role_item)) {
    role = strdup(role_item->valuestring);
  }

  cJSON_Delete(json);
  return role;
}

int verify_user_credentials(const char *username, const char *password, char *out_role) {
  if (!username || !password) {
    fprintf(stderr, "verify_user_credentials: NULL parameter\n");
    return -1;
  }

  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) {
    fprintf(stderr, "verify_user_credentials: Failed to acquire DB connection\n");
    return -1;
  }

  char query[512];
  snprintf(query, sizeof(query),
           "SELECT id, role FROM users WHERE username = '%s' AND password_hash = '%s';",
           username, password);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "verify_user_credentials: Query failed for user: %s\n", username);
    PQclear(res);
    db_pool_release(conn_wrapper);
    return -1;
  }

  if (PQntuples(res) == 0) {
    fprintf(stderr, "verify_user_credentials: Login failed for user: %s\n", username);
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
  printf("verify_user_credentials: User %s (ID: %d) successfully logged in\n", username, uid);
  return uid;
}
