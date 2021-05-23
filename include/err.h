#ifndef ERR_H
#define ERR_H

#include "../include/token.h"

extern void fatal_token_error(const char* error, struct Token* token);

extern void fatal_error(const char* error);

extern void fatal_compiler_error(const char* error, const char* s1, int line);

extern void token_warning(const char* warning, struct Token* token);

#endif //!ERR_H