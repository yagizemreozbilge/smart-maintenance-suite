#include "maintenance_service.h"
#include "db_connection.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TEST_MODE

bool add_maintenance_log(int machine_id, const char *technician, const char *description, double cost) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) {
    LOG_ERROR("Could not acquire connection to add maintenance log.");
    return false;
  }

  char query[1024];
  snprintf(query, sizeof(query),
           "INSERT INTO maintenance_logs (machine_id, performer, description, cost) "
           "VALUES (%d, '%s', '%s', %.2f);",
           machine_id, technician, description, cost);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    LOG_ERROR("Failed to insert maintenance log: %s", PQerrorMessage(conn_wrapper->pg_conn));
    PQclear(res);
    db_pool_release(conn_wrapper);
    return false;
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  LOG_INFO("Maintenance log added for Machine %d.", machine_id);
  return true;
}

int get_maintenance_history(int machine_id, MaintenanceLog *out_logs, int max_logs) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) {
    LOG_ERROR("Could not acquire connection to fetch maintenance history.");
    return 0;
  }

  char query[256];
  snprintf(query, sizeof(query),
           "SELECT id, machine_id, performer, to_char(ts, 'YYYY-MM-DD'), description, cost "
           "FROM maintenance_logs WHERE machine_id = %d ORDER BY ts DESC;",
           machine_id);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    LOG_ERROR("Failed to fetch maintenance history: %s", PQerrorMessage(conn_wrapper->pg_conn));
    PQclear(res);
    db_pool_release(conn_wrapper);
    return 0;
  }

  int rows = PQntuples(res);
  int count = (rows < max_logs) ? rows : max_logs;

  for (int i = 0; i < count; i++) {
    out_logs[i].id = atoi(PQgetvalue(res, i, 0));
    out_logs[i].machine_id = atoi(PQgetvalue(res, i, 1));
    strncpy(out_logs[i].technician_name, PQgetvalue(res, i, 2), 99);
    out_logs[i].technician_name[99] = '\0';
    strncpy(out_logs[i].log_date, PQgetvalue(res, i, 3), 31);
    out_logs[i].log_date[31] = '\0';
    strncpy(out_logs[i].description, PQgetvalue(res, i, 4), 511);
    out_logs[i].description[511] = '\0';
    out_logs[i].cost = atof(PQgetvalue(res, i, 5));
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  LOG_INFO("Fetched %d maintenance logs for Machine %d.", count, machine_id);
  return count;
}

int get_all_maintenance_logs(MaintenanceLog *out_logs, int max_logs) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return 0;

  const char *query = "SELECT id, machine_id, performer, to_char(ts, 'YYYY-MM-DD'), description, cost "
                      "FROM maintenance_logs ORDER BY ts DESC;";
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    PQclear(res);
    db_pool_release(conn_wrapper);
    return 0;
  }

  int rows = PQntuples(res);
  int count = (rows < max_logs) ? rows : max_logs;

  for (int i = 0; i < count; i++) {
    out_logs[i].id = atoi(PQgetvalue(res, i, 0));
    out_logs[i].machine_id = atoi(PQgetvalue(res, i, 1));
    strncpy(out_logs[i].technician_name, PQgetvalue(res, i, 2), 99);
    out_logs[i].technician_name[99] = '\0';
    strncpy(out_logs[i].log_date, PQgetvalue(res, i, 3), 31);
    out_logs[i].log_date[31] = '\0';
    strncpy(out_logs[i].description, PQgetvalue(res, i, 4), 511);
    out_logs[i].description[511] = '\0';
    out_logs[i].cost = atof(PQgetvalue(res, i, 5));
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return count;
}

bool update_maintenance_log(int log_id, const char *description, double cost) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return false;

  char query[1024];
  snprintf(query, sizeof(query),
           "UPDATE maintenance_logs SET description = '%s', cost = %.2f WHERE id = %d;",
           description, cost, log_id);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);
  bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);

  if (!success) {
    LOG_ERROR("Failed to update log: %s", PQerrorMessage(conn_wrapper->pg_conn));
  } else {
    LOG_INFO("Maintenance log %d updated successfully.", log_id);
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return success;
}

bool delete_maintenance_log(int log_id) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return false;

  char query[128];
  snprintf(query, sizeof(query), "DELETE FROM maintenance_logs WHERE id = %d;", log_id);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);
  bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);

  if (!success) {
    LOG_ERROR("Failed to delete log: %s", PQerrorMessage(conn_wrapper->pg_conn));
  } else {
    LOG_INFO("Maintenance log %d deleted.", log_id);
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return success;
}

#endif  // TEST_MODE

#ifdef TEST_MODE

// =====================================
// MOCK IMPLEMENTATION
// =====================================

bool add_maintenance_log(int machine_id,
                         const char *technician,
                         const char *description,
                         double cost) {
  if (machine_id <= 0 || !technician || !description)
    return false;

  return true;
}

int get_maintenance_history(int machine_id,
                            MaintenanceLog *logs,
                            int max_count) {
  if (!logs || max_count <= 0)
    return 0;

  logs[0].id = 1;
  logs[0].machine_id = machine_id;
  strcpy(logs[0].technician_name, "TestTech");
  strcpy(logs[0].log_date, "2025-01-01");
  strcpy(logs[0].description, "Mock history");
  logs[0].cost = 100.0;
  return 1;
}

int get_all_maintenance_logs(MaintenanceLog *logs,
                             int max_count) {
  if (!logs || max_count <= 0)
    return 0;

  logs[0].id = 1;
  logs[0].machine_id = 1;
  strcpy(logs[0].technician_name, "Ali");
  strcpy(logs[0].log_date, "2025-01-01");
  strcpy(logs[0].description, "Routine oil change");
  logs[0].cost = 150.5;
  return 1;
}

bool update_maintenance_log(int log_id,
                            const char *description,
                            double cost) {
  if (log_id <= 0 || !description)
    return false;

  return true;
}

bool delete_maintenance_log(int log_id) {
  if (log_id <= 0)
    return false;

  return true;
}

#endif  // TEST_MODE
