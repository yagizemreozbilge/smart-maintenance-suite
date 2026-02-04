#include "api_handlers.h"
#include "machine_service.h"
#include "inventory_service.h"
#include "alert_service.h"
#include "sensor_service.h"
#include "cJSON.h"
#include <stdlib.h>
#include <stdio.h>

char *serialize_machines_to_json() {
  Machine list[20];
  int count = get_all_machines(list, 20);
  // 1. Yeni bir JSON Dizisi (Array) oluştur: [ ]
  cJSON *root = cJSON_CreateArray();

  for (int i = 0; i < count; i++) {
    // 2. Her makine için bir JSON Objesi oluştur: { }
    cJSON *machine = cJSON_CreateObject();
    // 3. Bilgileri objeye ekle
    cJSON_AddNumberToObject(machine, "id", list[i].id);
    cJSON_AddStringToObject(machine, "name", list[i].name);
    cJSON_AddStringToObject(machine, "status", list[i].status);
    cJSON_AddStringToObject(machine, "location", list[i].location);
    // 4. Objeyi ana dizinin içine at
    cJSON_AddItemToArray(root, machine);
  }

  // 5. JSON yapısını string (metin) formatına çevir
  char *json_output = cJSON_Print(root);
  // 6. Belleği temizle (JSON objesini sil, string kalır)
  cJSON_Delete(root);
  return json_output;
}

char *serialize_inventory_to_json() {
  InventoryItem items[50];
  int count = get_all_inventory(items, 50);
  cJSON *root = cJSON_CreateArray();

  for (int i = 0; i < count; i++) {
    cJSON *item = cJSON_CreateObject();
    cJSON_AddNumberToObject(item, "id", items[i].id);
    cJSON_AddStringToObject(item, "part_name", items[i].part_name);
    cJSON_AddStringToObject(item, "sku", items[i].sku);
    cJSON_AddNumberToObject(item, "quantity", items[i].quantity);
    cJSON_AddNumberToObject(item, "min_stock", items[i].min_stock_level);
    cJSON_AddNumberToObject(item, "cost", items[i].unit_cost);
    cJSON_AddItemToArray(root, item);
  }

  char *json_output = cJSON_Print(root);
  cJSON_Delete(root);
  return json_output;
}

char *serialize_alerts_to_json() {
  AlertInfo alerts[50];
  int count = get_recent_alerts(alerts, 50);
  cJSON *root = cJSON_CreateArray();

  for (int i = 0; i < count; i++) {
    cJSON *alert = cJSON_CreateObject();
    cJSON_AddNumberToObject(alert, "id", alerts[i].id);
    cJSON_AddNumberToObject(alert, "sensor_id", alerts[i].sensor_id);
    cJSON_AddStringToObject(alert, "severity", alerts[i].severity);
    cJSON_AddStringToObject(alert, "message", alerts[i].message);
    cJSON_AddStringToObject(alert, "created_at", alerts[i].created_at);
    cJSON_AddItemToArray(root, alert);
  }

  char *json_output = cJSON_Print(root);
  cJSON_Delete(root);
  return json_output;
}

// Helper: We might need a function to get all sensors status, currently we only have per machine.
// Let's assume for now we provide health for machine ID 1 as a placeholder or we can add a global fetcher.
char *serialize_sensors_to_json() {
  SensorStatus stats[50];
  // For now, let's fetch for machine 1 just to see it working, or we can improve the service.
  int count = get_machine_health(1, stats, 50);
  cJSON *root = cJSON_CreateArray();

  for (int i = 0; i < count; i++) {
    cJSON *s = cJSON_CreateObject();
    cJSON_AddNumberToObject(s, "sensor_id", stats[i].sensor_id);
    cJSON_AddStringToObject(s, "type", stats[i].sensor_type);
    cJSON_AddNumberToObject(s, "value", stats[i].last_value);
    cJSON_AddStringToObject(s, "unit", stats[i].unit);
    cJSON_AddStringToObject(s, "timestamp", stats[i].recorded_at);
    cJSON_AddItemToArray(root, s);
  }

  char *json_output = cJSON_Print(root);
  cJSON_Delete(root);
  return json_output;
}
