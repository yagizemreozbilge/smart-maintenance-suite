#ifndef API_HANDLERS_H
#define API_HANDLERS_H

char *serialize_machines_to_json(void);
char *serialize_inventory_to_json(void);
char *serialize_maintenance_to_json(void);
char *generate_auth_token(void);
char *generate_maintenance_xml_report(void);
char *generate_inventory_xml_report(void);

#endif
