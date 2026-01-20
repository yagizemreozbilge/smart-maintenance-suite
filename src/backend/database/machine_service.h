#ifndef MACHINE_SERVICE_H
#define MACHINE_SERVICE_H

#include <stddef.h>

/** Machine status representation */
typedef struct {
  int id;
  char name[100];
  char model[50];
  char location[100];
  char status[20];
} Machine;

/**
 * Fetches all machines from the database.
 * @param machines Pointer to an array of Machine structs (allocated by caller).
 * @param max_count Maximum number of machines to fetch.
 * @return Number of machines successfully fetched.
 */
int get_all_machines(Machine *machines, int max_count);

#endif
