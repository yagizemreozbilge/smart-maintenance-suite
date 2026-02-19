#include "auth_handler.h"
#include "../../database/cJSON.h"
#include "../../security/jwt.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void handle_auth_request(HttpRequest *req, HttpResponse *res) {
  // Mock kullanıcı bilgileri
  int user_id = 1;
  const char *username = "admin";
  const char *role = "admin";
  // jwt.h'daki orijinal generate_auth_token fonksiyonunu çağır
  char *token = generate_auth_token(user_id, username, role);
  cJSON *root = cJSON_CreateObject();
  cJSON_AddBoolToObject(root, "success", true);
  cJSON_AddStringToObject(root, "status", "authenticated");
  cJSON_AddStringToObject(root, "token", token);
  cJSON_AddStringToObject(root, "role", role);
  cJSON_AddStringToObject(root, "username", username);
  char *json = cJSON_Print(root);
  cJSON_Delete(root);
  free(token);
  res->status_code = 200;
  strcpy(res->content_type, "application/json");
  strncpy(res->body, json, 8191);
  res->body[8191] = '\0';
  free(json);
}
