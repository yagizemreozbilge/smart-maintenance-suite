#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../../api/handlers/machine_handler.h"

/* ==========================================================
   MOCK STATE
   ========================================================== */

static int mock_duplicate_mode = 0;
static int mock_add_machine_success = 1;

/* ==========================================================
   MOCK STRUCTS
   ========================================================== */

typedef struct {
  int id;
  char name[100];
  char model[100];
  char location[100];
  char status[50];
} Machine;

/* ==========================================================
   MOCK FUNCTIONS
   ========================================================== */

int get_all_machines(Machine *list, int max) {
  if (!mock_duplicate_mode)
    return 0;

  strcpy(list[0].name, "CNC Machine");
  return 1;
}

bool add_machine(const char *name,
                 const char *model,
                 const char *location,
                 const char *status) {
  (void)name;
  (void)model;
  (void)location;
  (void)status;
  return mock_add_machine_success;
}

/* ==========================================================
   MOCK SERIALIZER (for GET)
   ========================================================== */

char *serialize_machines_to_json(void) {
  char *json = malloc(128);
  strcpy(json, "{\"machines\":[{\"id\":1,\"name\":\"CNC Machine\"}]}");
  return json;
}

/* ==========================================================
   TESTS
   ========================================================== */

void test_get_request() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "GET");
  handle_machine_request(&req, &res);
  assert(res.status_code == 200);
  assert(strcmp(res.content_type, "application/json") == 0);
  assert(strstr(res.body, "CNC Machine") != NULL);
  printf("✔ test_get_request\n");
}

void test_post_success() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "POST");
  strcpy(req.body,
         "{\"name\":\"Drill\",\"model\":\"X1\",\"location\":\"Hall A\",\"status\":\"running\"}");
  mock_duplicate_mode = 0;
  mock_add_machine_success = 1;
  handle_machine_request(&req, &res);
  assert(res.status_code == 201);
  assert(strstr(res.body, "success") != NULL);
  printf("✔ test_post_success\n");
}

void test_invalid_json() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "POST");
  strcpy(req.body, "INVALID_JSON");
  handle_machine_request(&req, &res);
  assert(res.status_code == 400);
  assert(strstr(res.body, "Invalid JSON") != NULL);
  printf("✔ test_invalid_json\n");
}

void test_missing_fields() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "POST");
  strcpy(req.body, "{\"name\":\"OnlyName\"}");
  handle_machine_request(&req, &res);
  assert(res.status_code == 400);
  assert(strstr(res.body, "Missing required fields") != NULL);
  printf("✔ test_missing_fields\n");
}

void test_duplicate_machine() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "POST");
  strcpy(req.body,
         "{\"name\":\"CNC Machine\",\"status\":\"running\"}");
  mock_duplicate_mode = 1;
  mock_add_machine_success = 1;
  handle_machine_request(&req, &res);
  assert(res.status_code == 409);
  assert(strstr(res.body, "already exists") != NULL);
  printf("✔ test_duplicate_machine\n");
}

void test_add_machine_failure() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "POST");
  strcpy(req.body,
         "{\"name\":\"NewMachine\",\"status\":\"running\"}");
  mock_duplicate_mode = 0;
  mock_add_machine_success = 0;
  handle_machine_request(&req, &res);
  assert(res.status_code == 500);
  assert(strstr(res.body, "Database error") != NULL);
  printf("✔ test_add_machine_failure\n");
}

/* ==========================================================
   MAIN
   ========================================================== */

int main() {
  test_get_request();
  test_post_success();
  test_invalid_json();
  test_missing_fields();
  test_duplicate_machine();
  test_add_machine_failure();
  printf("\nALL machine_handler tests passed.\n");
  return 0;
}
