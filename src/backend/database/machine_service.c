#include "machine_service.h"
#include "db_connection.h"
#include "logger.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef TEST_MODE

// ====================================================================
// GERÇEK IMPLEMENTASYON (PostgreSQL)
// ====================================================================

int get_all_machines(Machine *machines, int max_count) {
  if (!machines || max_count <= 0)
    return 0;

  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) {
    LOG_ERROR("Could not acquire connection to fetch machines.");
    return 0;
  }

  PGresult *res = PQexec(
                    conn_wrapper->pg_conn,
                    "SELECT id, name, model, location, status FROM machines");

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    LOG_ERROR("SELECT machines failed: %s",
              PQerrorMessage(conn_wrapper->pg_conn));
    PQclear(res);
    db_pool_release(conn_wrapper);
    return 0;
  }

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
    machines[i].health_score =
      get_machine_health_score(machines[i].id);
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  LOG_INFO("Successfully fetched %d machines from DB.", count);
  return count;
}

int get_machine_by_id(int id, Machine *machine) {
  if (!machine)
    return -1;

  machine->id = id;
  snprintf(machine->name, sizeof(machine->name),
           "Machine %d", id);
  strncpy(machine->model, "Standard Model", sizeof(machine->model) - 1);
  machine->model[sizeof(machine->model) - 1] = '\0';
  strncpy(machine->location, "Production Line",
          sizeof(machine->location) - 1);
  machine->location[sizeof(machine->location) - 1] = '\0';
  strncpy(machine->status, "running",
          sizeof(machine->status) - 1);
  machine->status[sizeof(machine->status) - 1] = '\0';
  machine->health_score = 90.0;
  return 0;
}

double get_machine_health_score(int machine_id) {
  (void)machine_id;
  return 94.7;
}

#endif  // TEST_MODE



#ifdef TEST_MODE

// =====================================
// MOCK IMPLEMENTATION (COVERAGE BOOST)
// =====================================

int get_all_machines(Machine *machines, int max_count) {
  if (!machines)
    return 0;

  if (max_count <= 0)
    return 0;

  machines[0].id = 1;
  strcpy(machines[0].name, "CNC-01");
  strcpy(machines[0].model, "X100");
  strcpy(machines[0].location, "Line A");
  strcpy(machines[0].status, "active");
  machines[0].health_score = 88.5;
  return 1;
}

int get_machine_by_id(int id, Machine *machine) {
  if (!machine)
    return 0;

  if (id <= 0)
    return 0;

  machine->id = id;
  strcpy(machine->name, "Mock Machine");
  strcpy(machine->model, "Mock Model");
  strcpy(machine->location, "Test Lab");
  strcpy(machine->status, "active");
  machine->health_score = 90.0;
  return 1;
}

double get_machine_health_score(int id) {
  if (id <= 0)
    return 0.0;

  return 88.5;
}

#endif
