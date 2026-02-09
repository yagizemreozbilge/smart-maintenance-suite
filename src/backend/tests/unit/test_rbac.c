#include "../../security/rbac.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_string_to_role_admin() {
  UserRole role = string_to_role("admin");
  assert(role == ROLE_ADMIN);
  printf("[PASS] string_to_role: admin\n");
}

void test_string_to_role_teknisyen() {
  UserRole role = string_to_role("teknisyen");
  assert(role == ROLE_TEKNISYEN);
  printf("[PASS] string_to_role: teknisyen\n");
}

void test_string_to_role_operator() {
  UserRole role = string_to_role("operator");
  assert(role == ROLE_OPERATOR);
  printf("[PASS] string_to_role: operator\n");
}

void test_string_to_role_unknown() {
  UserRole role = string_to_role("invalid_role");
  assert(role == ROLE_UNKNOWN);
  printf("[PASS] string_to_role: unknown role\n");
}

void test_string_to_role_null() {
  UserRole role = string_to_role(NULL);
  assert(role == ROLE_UNKNOWN);
  printf("[PASS] string_to_role: NULL input\n");
}

void test_can_delete_assets_admin() {
  assert(can_delete_assets(ROLE_ADMIN) == 1);
  printf("[PASS] can_delete_assets: admin allowed\n");
}

void test_can_delete_assets_teknisyen() {
  assert(can_delete_assets(ROLE_TEKNISYEN) == 0);
  printf("[PASS] can_delete_assets: teknisyen denied\n");
}

void test_can_delete_assets_operator() {
  assert(can_delete_assets(ROLE_OPERATOR) == 0);
  printf("[PASS] can_delete_assets: operator denied\n");
}

void test_can_onboard_assets_admin() {
  assert(can_onboard_assets(ROLE_ADMIN) == 1);
  printf("[PASS] can_onboard_assets: admin allowed\n");
}

void test_can_onboard_assets_teknisyen() {
  assert(can_onboard_assets(ROLE_TEKNISYEN) == 1);
  printf("[PASS] can_onboard_assets: teknisyen allowed\n");
}

void test_can_onboard_assets_operator() {
  assert(can_onboard_assets(ROLE_OPERATOR) == 0);
  printf("[PASS] can_onboard_assets: operator denied\n");
}

void test_can_view_system_all_roles() {
  assert(can_view_system(ROLE_ADMIN) == 1);
  assert(can_view_system(ROLE_TEKNISYEN) == 1);
  assert(can_view_system(ROLE_OPERATOR) == 1);
  printf("[PASS] can_view_system: all roles allowed\n");
}

void test_can_view_system_unknown() {
  assert(can_view_system(ROLE_UNKNOWN) == 0);
  printf("[PASS] can_view_system: unknown role denied\n");
}

int main() {
  printf("=== RBAC Unit Tests ===\n");
  test_string_to_role_admin();
  test_string_to_role_teknisyen();
  test_string_to_role_operator();
  test_string_to_role_unknown();
  test_string_to_role_null();
  test_can_delete_assets_admin();
  test_can_delete_assets_teknisyen();
  test_can_delete_assets_operator();
  test_can_onboard_assets_admin();
  test_can_onboard_assets_teknisyen();
  test_can_onboard_assets_operator();
  test_can_view_system_all_roles();
  test_can_view_system_unknown();
  printf("\n✅ All RBAC tests passed!\n");
  return 0;
}
