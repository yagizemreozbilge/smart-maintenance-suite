#include "api_handlers.h"
#include "machine_service.h"
#include "inventory_service.h"
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
