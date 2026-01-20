#include "sensor_service.h"
#include "db_connection.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_machine_health(int machine_id, SensorStatus *out_stats, int max_sensors) {
  // 1. Acquire a connection from the pool
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) {
    LOG_ERROR("Could not acquire connection for machine health check (ID: %d).", machine_id);
    return 0;
  }

  // 2. Prepare the SQL query
  // We use "DISTINCT ON (s.id)" to get exactly one row (the newest one) per sensor.
  char query[512];
  snprintf(query, sizeof(query),
           "SELECT DISTINCT ON (s.id) "
           "s.id, s.type, sd.value, s.unit, "
           "to_char(sd.recorded_at, 'YYYY-MM-DD HH24:MI:SS') "
           "FROM sensors s "
           "JOIN sensor_data sd ON s.id = sd.sensor_id "
           "WHERE s.machine_id = %d "
           "ORDER BY s.id, sd.recorded_at DESC;",
           machine_id);
  // 3. Execute the query
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    LOG_ERROR("Health check query failed for machine %d: %s", machine_id, PQerrorMessage(conn_wrapper->pg_conn));
    PQclear(res);
    db_pool_release(conn_wrapper);
    return 0;
  }

  // 4. Map DB results to our C struct array
  int rows = PQntuples(res);
  int count = (rows < max_sensors) ? rows : max_sensors;

  for (int i = 0; i < count; i++) {
    out_stats[i].sensor_id = atoi(PQgetvalue(res, i, 0));
    strncpy(out_stats[i].type, PQgetvalue(res, i, 1), 31);
    out_stats[i].type[31] = '\0';
    out_stats[i].last_value = atof(PQgetvalue(res, i, 2));
    strncpy(out_stats[i].unit, PQgetvalue(res, i, 3), 9);
    out_stats[i].unit[9] = '\0';
    strncpy(out_stats[i].recorded_at, PQgetvalue(res, i, 4), 31);
    out_stats[i].recorded_at[31] = '\0';
  }

  // 5. Cleanup
  PQclear(res);
  db_pool_release(conn_wrapper);
  LOG_INFO("Fetched health data for machine %d (%d sensors found).", machine_id, count);
  return count;
}
