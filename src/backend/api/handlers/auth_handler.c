#include "auth_handler.h"
#include "../../database/db_connection.h"
#include "../../security/jwt.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void handle_auth_request(HttpRequest *req, HttpResponse *res) {
  char username[50] = {0}, password[50] = {0}, role[20] = {0};
  // Support both query params and simple body formats
  char *target = (strlen(req->query_params) > 0) ? req->query_params : req->body;
  char *u_ptr = strstr(target, "u=");
  char *p_ptr = strstr(target, "p=");

  if (u_ptr && p_ptr) {
    sscanf(u_ptr, "u=%[^& \n\r]", username);
    sscanf(p_ptr, "p=%[^& \n\r]", password);
    // Mock verify user (using logic from legacy)
    // In real app, we'd call the user_service
    int uid = -1;

    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
      uid = 1;
      strcpy(role, "admin");
    } else if (strcmp(username, "teknik1") == 0 && strcmp(password, "tech123") == 0) {
      uid = 2;
      strcpy(role, "teknisyen");
    }

    if (uid != -1) {
      char *token = generate_auth_token(uid, username, role);
      res->status_code = 200;
      sprintf(res->body, "{\"success\": true, \"token\": \"%s\", \"role\": \"%s\", \"username\": \"%s\"}",
              token, role, username);
      free(token);
    } else {
      res->status_code = 401;
      strcpy(res->body, "{\"success\": false, \"message\": \"Invalid credentials\"}");
    }
  } else {
    res->status_code = 400;
    strcpy(res->body, "{\"error\": \"Missing credentials\"}");
  }
}
