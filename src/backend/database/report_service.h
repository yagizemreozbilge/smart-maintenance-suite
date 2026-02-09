#ifndef REPORT_SERVICE_H
#define REPORT_SERVICE_H

#include "maintenance_service.h"
#include "inventory_service.h"

// Raporları CSV formatında string olarak üretir
char *generate_maintenance_csv_report();
char *generate_inventory_csv_report();

#endif
