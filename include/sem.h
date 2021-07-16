#ifndef SEM_H
#define SEM_H

#include "../include/expression.h"

extern void validate_ast(struct ASTNode* root);

extern bool check_if_conversion_possible(enum TokenType current, enum TokenType new_type);

extern void begin_sem();

#endif // !SEM_H
