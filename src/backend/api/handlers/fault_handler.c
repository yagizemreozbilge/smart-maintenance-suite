/* This file is a template for fault_handler.c. Content will be filled by yagiz on 2025-12-29. */
#include "fault_handler.h"
#include "../../database/cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Fault/Arıza verileri için JSON serialize fonksiyonu
char *serialize_faults_to_json(void) {
  cJSON *root = cJSON_CreateObject();
  cJSON *faults_array = cJSON_CreateArray();
  // Mock fault/arıza verileri
  cJSON *fault1 = cJSON_CreateObject();
  cJSON_AddNumberToObject(fault1, "id", 1);
  cJSON_AddNumberToObject(fault1, "machine_id", 1);
  cJSON_AddStringToObject(fault1, "machine_name", "CNC Machine");
  cJSON_AddStringToObject(fault1, "description", "Overheating detected");
  cJSON_AddStringToObject(fault1, "severity", "CRITICAL");
  cJSON_AddStringToObject(fault1, "status", "active");
  cJSON_AddStringToObject(fault1, "detected_at", "2026-02-11 10:30:00");
  cJSON_AddItemToArray(faults_array, fault1);
  cJSON *fault2 = cJSON_CreateObject();
  cJSON_AddNumberToObject(fault2, "id", 2);
  cJSON_AddNumberToObject(fault2, "machine_id", 2);
  cJSON_AddStringToObject(fault2, "machine_name", "Conveyor Belt");
  cJSON_AddStringToObject(fault2, "description", "Belt misalignment");
  cJSON_AddStringToObject(fault2, "severity", "WARNING");
  cJSON_AddStringToObject(fault2, "status", "acknowledged");
  cJSON_AddStringToObject(fault2, "detected_at", "2026-02-11 09:15:00");
  cJSON_AddItemToArray(faults_array, fault2);
  cJSON_AddItemToObject(root, "faults", faults_array);
  char *json = cJSON_Print(root);
  cJSON_Delete(root);
  return json;
}

// Tek bir fault detayı için JSON serialize
char *serialize_fault_detail_to_json(int fault_id) {
  cJSON *root = cJSON_CreateObject();
  cJSON_AddNumberToObject(root, "id", fault_id);
  cJSON_AddNumberToObject(root, "machine_id", 1);
  cJSON_AddStringToObject(root, "machine_name", "CNC Machine");
  cJSON_AddStringToObject(root, "description", "Overheating detected - Temperature exceeded 90°C");
  cJSON_AddStringToObject(root, "severity", "CRITICAL");
  cJSON_AddStringToObject(root, "status", "active");
  cJSON_AddStringToObject(root, "detected_at", "2026-02-11 10:30:00");
  cJSON_AddStringToObject(root, "resolved_at", "");
  cJSON_AddStringToObject(root, "root_cause", "Coolant pump failure");
  cJSON_AddStringToObject(root, "recommended_action", "Check and replace coolant pump");
  char *json = cJSON_Print(root);
  cJSON_Delete(root);
  return json;
}

void handle_fault_request(HttpRequest *req, HttpResponse *res) {
  // GET /api/faults - Tüm arızaları listele
  // GET /api/faults/1 - Tek bir arıza detayı
  // POST /api/faults - Yeni arıza bildir
  // PUT /api/faults/1/acknowledge - Arızayı onayla
  // PUT /api/faults/1/resolve - Arızayı çöz
  if (strcmp(req->method, "GET") == 0) {
    // Check if it's a single fault request
    char *fault_id_str = strstr(req->path, "/api/faults/");

    if (fault_id_str && strlen(fault_id_str) > 12) {
      // GET /api/faults/{id}
      int fault_id = atoi(fault_id_str + 12);
      char *json = serialize_fault_detail_to_json(fault_id);
      res->status_code = 200;
      strcpy(res->content_type, "application/json");
      strncpy(res->body, json, 8191);
      res->body[8191] = '\0';
      free(json);
    } else {
      // GET /api/faults
      char *json = serialize_faults_to_json();
      res->status_code = 200;
      strcpy(res->content_type, "application/json");
      strncpy(res->body, json, 8191);
      res->body[8191] = '\0';
      free(json);
    }
  } else if (strcmp(req->method, "POST") == 0) {
    // POST /api/faults - Yeni arıza bildir
    res->status_code = 201;
    strcpy(res->content_type, "application/json");
    strcpy(res->body, "{\"success\": true, \"message\": \"Fault reported successfully\", \"fault_id\": 3}");
  } else if (strcmp(req->method, "PUT") == 0) {
    // PUT /api/faults/1/acknowledge veya /api/faults/1/resolve
    if (strstr(req->path, "/acknowledge")) {
      res->status_code = 200;
      strcpy(res->content_type, "application/json");
      strcpy(res->body, "{\"success\": true, \"message\": \"Fault acknowledged\"}");
    } else if (strstr(req->path, "/resolve")) {
      res->status_code = 200;
      strcpy(res->content_type, "application/json");
      strcpy(res->body, "{\"success\": true, \"message\": \"Fault resolved\"}");
    } else {
      res->status_code = 400;
      strcpy(res->content_type, "application/json");
      strcpy(res->body, "{\"error\": \"Invalid fault operation\"}");
    }
  } else {
    res->status_code = 405;
    strcpy(res->content_type, "application/json");
    strcpy(res->body, "{\"error\": \"Method not supported for faults\"}");
  }
}
