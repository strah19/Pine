#include "../include/err.h"

void fatal_token_error(const char* error, struct Token* token) {
    fprintf(stderr, "Error: %s: '%s' on line %d.\n", error, token->token_string, token->token_info.token_line);
    exit(EXIT_FAILURE);
}

void fatal_compiler_error(const char* error, const char* s1, int line) {
    fprintf(stderr, "Error: %s: '%s' on line %d.\n", error, s1, line);
    exit(EXIT_FAILURE);
}

void fatal_error(const char* error) {
    fprintf(stderr, "Error: %s.\n", error);
    exit(EXIT_FAILURE);  
}

void token_warning(const char* warning, struct Token* token) {
    fprintf(stderr, "Warning: %s: %s on line %d.\n", warning, token->token_string, token->token_info.token_line);
}