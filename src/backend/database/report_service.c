#include "report_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *generate_maintenance_csv_report();
char *generate_inventory_csv_report();
char *generate_maintenance_xml_report();
char *generate_inventory_xml_report();

char *generate_maintenance_csv_report() {
  MaintenanceLog logs[100];
  int count = get_all_maintenance_logs(logs, 100);
  // Tahmini buffer boyutu: her satır ~200 karakter + başlık
  char *report = (char *)malloc(count * 200 + 500);

  if (!report) return NULL;

  strcpy(report, "Log_ID,Machine_ID,Technician,Date,Description\n");

  for (int i = 0; i < count; i++) {
    char line[256];
    // CSV formatında virgüllerle ayırıyoruz
    sprintf(line, "%d,%d,%s,%s,%s\n",
            logs[i].id, logs[i].machine_id, logs[i].technician_name,
            logs[i].log_date, logs[i].description);
    strcat(report, line);
  }

  return report;
}

char *generate_inventory_csv_report() {
  InventoryItem items[100];
  int count = get_all_inventory(items, 100);
  char *report = (char *)malloc(count * 200 + 500);

  if (!report) return NULL;

  strcpy(report, "Part_ID,Part_Name,SKU,Current_Quantity,Min_Stock\n");

  for (int i = 0; i < count; i++) {
    char line[256];
    sprintf(line, "%d,%s,%s,%d,%d\n",
            items[i].id, items[i].part_name, items[i].sku,
            items[i].quantity, items[i].min_stock_level);
    strcat(report, line);
  }

  return report;
}
