#include "rbac.h"
#include <string.h>

UserRole string_to_role(const char *role_str) {
  if (role_str == NULL) return ROLE_UNKNOWN;

  if (strcmp(role_str, "admin") == 0) return ROLE_ADMIN;

  if (strcmp(role_str, "teknisyen") == 0) return ROLE_TEKNISYEN;

  if (strcmp(role_str, "operator") == 0) return ROLE_OPERATOR;

  return ROLE_UNKNOWN;
}

int can_delete_assets(UserRole role) {
  return (role == ROLE_ADMIN);
}

int can_onboard_assets(UserRole role) {
  return (role == ROLE_ADMIN || role == ROLE_TEKNISYEN);
}

int can_view_system(UserRole role) {
  return (role != ROLE_UNKNOWN);
}
