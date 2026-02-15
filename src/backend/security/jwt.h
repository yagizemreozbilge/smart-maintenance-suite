#ifndef JWT_H
#define JWT_H

#include <stdbool.h>

char *generate_auth_token(int user_id, const char *username, const char *role);
bool validate_auth_token(const char *token_str);
int verify_user_credentials(const char *username, const char *password, char *out_role);
int get_user_id_from_token(const char *token_str);
char *get_role_from_token(const char *token_str);

#endif
