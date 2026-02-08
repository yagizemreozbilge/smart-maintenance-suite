#ifndef API_HANDLERS_H
#define API_HANDLERS_H

/**
 * Converts the machine list to a JSON string.
 * This string must be freed by the caller using free().
 */
char *serialize_machines_to_json();
char *serialize_inventory_to_json();
char *serialize_alerts_to_json();
char *serialize_sensors_to_json(int machine_id);
char *serialize_maintenance_to_json();

#endif
