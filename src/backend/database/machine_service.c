#include "machine_service.h"
#include "db_connection.h"
#include "logger.h"
#include <string.h>
#include <stdlib.h>

int get_all_machines(Machine *machines, int max_count) {
  // 1. Havuzdan bir bağlantı ödünç al
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) {
    LOG_ERROR("Could not acquire connection to fetch machines.");
    return 0;
  }

  // 2. SQL Sorgusunu çalıştır
  PGresult *res = PQexec(conn_wrapper->pg_conn, "SELECT id, name, model, location, status FROM machines");

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
  }

  // 4. Temizlik: Sonucu temizle ve bağlantıyı havuza geri ver
  PQclear(res);
  db_pool_release(conn_wrapper);
  LOG_INFO("Successfully fetched %d machines from DB.", count);
  return count;
}
