#ifndef TOKEN_GENERATOR_H
#define TOKEN_GENERATOR_H


char *generate_auth_token_simple(int user_id, const char *role);
int validate_token(const char *token);
void free_token(char *token);

#endif
