#ifndef RBAC_H
#define RBAC_H

typedef enum {
  ROLE_ADMIN,
  ROLE_TEKNISYEN,
  ROLE_OPERATOR,
  ROLE_UNKNOWN
} UserRole;

// Rol metnini enum tipine çevirir
UserRole string_to_role(const char *role_str);

// Yetki kontrolleri
int can_delete_assets(UserRole role);     // Sadece Admin
int can_onboard_assets(UserRole role);    // Admin ve Teknisyen
int can_view_system(UserRole role);       // Hepsi

#endif
