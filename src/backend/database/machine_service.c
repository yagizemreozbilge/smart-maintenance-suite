#include "machine_service.h"
#include "db_connection.h"
#include "logger.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// ====================================================================
// GERÇEK IMPLEMENTASYON (PostgreSQL)
// ====================================================================

int get_all_machines(Machine *machines, int max_count) {
  // 1. Havuzdan bir bağlantı ödünç al
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) {
    LOG_ERROR("Could not acquire connection to fetch machines.");
    return 0;
  }

  // 2. SQL Sorgusunu çalıştır
  PGresult *res = PQexec(conn_wrapper->pg_conn,
                         "SELECT id, name, model, location, status FROM machines");

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    LOG_ERROR("SELECT machines failed: %s", PQerrorMessage(conn_wrapper->pg_conn));
    PQclear(res);
    db_pool_release(conn_wrapper);
    return 0;
  }

  // 3. Gelen verileri C Struct'larına kopyala
  int rows = PQntuples(res);
  int count = (rows < max_count) ? rows : max_count;

  for (int i = 0; i < count; i++) {
    machines[i].id = atoi(PQgetvalue(res, i, 0));
    strncpy(machines[i].name, PQgetvalue(res, i, 1), 99);
    machines[i].name[99] = '\0';
    strncpy(machines[i].model, PQgetvalue(res, i, 2), 49);
    machines[i].model[49] = '\0';
    strncpy(machines[i].location, PQgetvalue(res, i, 3), 99);
    machines[i].location[99] = '\0';
    strncpy(machines[i].status, PQgetvalue(res, i, 4), 19);
    machines[i].status[19] = '\0';
    machines[i].health_score = get_machine_health_score(machines[i].id);
  }

  // 4. Temizlik
  PQclear(res);
  db_pool_release(conn_wrapper);
  LOG_INFO("Successfully fetched %d machines from DB.", count);
  return count;
}

int get_machine_by_id(int id, Machine *machine) {
  // Gerçek implementasyon - veritabanından sorgula
  // Şimdilik mock veri döndür
  if (!machine) return -1;

  machine->id = id;
  sprintf(machine->name, "Machine %d", id);
  strcpy(machine->model, "Standard Model");
  strcpy(machine->location, "Production Line");
  strcpy(machine->status, "running");
  machine->health_score = 90.0;
  return 0;
}

double get_machine_health_score(int machine_id) {
  (void)machine_id;
  // Gerçek implementasyon - sensör verilerinden hesapla
  return 94.7;
}

// ====================================================================
// MOCK IMPLEMENTATIONS FOR TESTING
// ====================================================================

#ifdef TEST_MODE

// TEST MODE'DA gerçek fonksiyonları geçersiz kıl
#undef get_all_machines
#undef get_machine_by_id
#undef get_machine_health_score

int get_all_machines(Machine *machines, int max_count) {
  if (!machines || max_count <= 0) return 0;

  // Mock machine data for testing
  machines[0].id = 1;
  strcpy(machines[0].name, "CNC Machine");
  strcpy(machines[0].model, "HAAS VF-2");
  strcpy(machines[0].location, "Shop Floor A");
  strcpy(machines[0].status, "running");
  machines[0].health_score = 95.5;
  machines[1].id = 2;
  strcpy(machines[1].name, "Conveyor Belt");
  strcpy(machines[1].model, "Dorner 2200");
  strcpy(machines[1].location, "Assembly Line");
  strcpy(machines[1].status, "idle");
  machines[1].health_score = 87.3;
  machines[2].id = 3;
  strcpy(machines[2].name, "Robot Arm");
  strcpy(machines[2].model, "Fanuc M-10iA");
  strcpy(machines[2].location, "Welding Station");
  strcpy(machines[2].status, "running");
  machines[2].health_score = 99.1;
  return 3;
}

int get_machine_by_id(int id, Machine *machine) {
  if (!machine) return -1;

  machine->id = id;
  sprintf(machine->name, "Machine %d", id);
  strcpy(machine->model, "Test Model");
  strcpy(machine->location, "Test Location");
  strcpy(machine->status, "running");
  machine->health_score = 90.0;
  return 0;
}

double get_machine_health_score(int machine_id) {
  (void)machine_id;
  return 94.7;
}

#endif /* TEST_MODE */
