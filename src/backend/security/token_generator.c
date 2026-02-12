#include "token_generator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

char *generate_auth_token_simple(int user_id, const char *role) {
  // Basit mock token
  char *token = malloc(128);

  if (!token) return NULL;

  time_t now = time(NULL);
  sprintf(token, "token_%d_%s_%ld", user_id, role, now);
  return token;
}

int validate_token(const char *token) {
  if (!token) return 0;

  return (strstr(token, "token_") == token) ? 1 : 0;
}
