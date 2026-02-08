#ifndef JWT_H
#define JWT_H

#include <stdbool.h>

typedef struct {
  int user_id;
  char username[50];
  char role[20];
  char token_string[128];
  long expires_at;
} AuthToken;

/**
 * Creates a new session token for a user.
 */
char *generate_auth_token(int user_id, const char *username, const char *role);

/**
 * Validates a given token string.
 * @return true if valid and not expired.
 */
bool validate_auth_token(const char *token_str);

/**
 * Verifies user credentials against the database.
 * @return user_id if success, -1 if failed.
 */
int verify_user_credentials(const char *username, const char *password, char *out_role);

#endif
