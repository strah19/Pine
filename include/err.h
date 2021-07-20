#ifndef ERR_H
#define ERR_H

#include "../include/token.h"

extern void fatal_token_error(const char* error, struct Token* token);

extern void fatal_error(const char* error);

extern void fatal_compiler_error(const char* error, const char* s1, int line);

extern void token_warning(const char* warning, struct Token* token);

#define LVALUE_CONST_ERROR "Value cannot be modified as it is a const"
#define UNDEFINED_ID_ERROR "Undefined variable"
#define MUST_BE_LVALUE_ERROR "Value needs to be modifiable lvalue"
#define CONST_MUST_BE_DEFINED_ERROR "Need to give const a value"
#define VAR_REDEFINITION_ERROR "Redefinition of variable"
#define FUNC_REDEFINITION_ERROR "Redefinition of function"
#define ELSE_WITHOUT_IF_ERROR "Else without an if"
#define ELIF_WITHOUT_IF_ERROR "Elif without an if"
#define UNDEFINED_TOKEN_ERROR "Undefined token"
#define MISMATCHED_ARG_NUMS_ERROR "Wrong number of arguments to function"
#define MISMATCHED_LCURLY_ERROR "Mismatched { token"

#endif //!ERR_H