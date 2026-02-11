#include "api_handlers.h"
#include "alert_service.h"
#include "cJSON.h"
#include "machine_service.h"
#include "inventory_service.h"
#include "maintenance_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ====================================================================
// SERIALIZATION FUNCTIONS - Bunlar handler'lar tarafından çağrılıyor
// ====================================================================

char *serialize_machines_to_json(void) {
  MachineInfo machines[100];
  int count = get_all_machines(machines, 100);
  cJSON *root = cJSON_CreateObject();
  cJSON *items = cJSON_CreateArray();

  for (int i = 0; i < count; i++) {
    cJSON *item = cJSON_CreateObject();
    cJSON_AddNumberToObject(item, "id", machines[i].id);
    cJSON_AddStringToObject(item, "name", machines[i].name);
    cJSON_AddStringToObject(item, "status",
                            machines[i].status == 0 ? "running" : "idle");
    cJSON_AddItemToArray(items, item);
  }

  cJSON_AddItemToObject(root, "machines", items);
  char *json = cJSON_Print(root);
  cJSON_Delete(root);
  return json;
}

char *serialize_inventory_to_json(void) {
  InventoryItem items[100];
  int count = get_all_inventory(items, 100);
  cJSON *root = cJSON_CreateObject();
  cJSON *item_array = cJSON_CreateArray();

  for (int i = 0; i < count; i++) {
    cJSON *item = cJSON_CreateObject();
    cJSON_AddNumberToObject(item, "id", items[i].id);
    cJSON_AddStringToObject(item, "name", items[i].name);
    cJSON_AddNumberToObject(item, "quantity", items[i].quantity);
    cJSON_AddItemToArray(item_array, item);
  }

  cJSON_AddItemToObject(root, "inventory", item_array);
  char *json = cJSON_Print(root);
  cJSON_Delete(root);
  return json;
}

char *serialize_maintenance_to_json(void) {
  MaintenanceLog logs[100];
  int count = get_all_maintenance_logs(logs, 100);
  cJSON *root = cJSON_CreateObject();
  cJSON *log_array = cJSON_CreateArray();

  for (int i = 0; i < count; i++) {
    cJSON *log = cJSON_CreateObject();
    cJSON_AddNumberToObject(log, "id", logs[i].id);
    cJSON_AddNumberToObject(log, "machine_id", logs[i].machine_id);
    cJSON_AddStringToObject(log, "description", logs[i].description);
    cJSON_AddStringToObject(log, "status", logs[i].status);
    cJSON_AddItemToArray(log_array, log);
  }

  cJSON_AddItemToObject(root, "maintenance_logs", log_array);
  char *json = cJSON_Print(root);
  cJSON_Delete(root);
  return json;
}

// ====================================================================
// AUTH FUNCTIONS
// ====================================================================

char *generate_auth_token(void) {
  return strdup("mock-jwt-token-12345");
}

// ====================================================================
// REPORT FUNCTIONS
// ====================================================================

char *generate_maintenance_xml_report(void) {
  return strdup("<?xml version=\"1.0\"?><report><maintenance>mock</maintenance></report>");
}

char *generate_inventory_xml_report(void) {
  return strdup("<?xml version=\"1.0\"?><report><inventory>mock</inventory></report>");
}
