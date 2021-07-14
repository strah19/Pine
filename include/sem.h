#ifndef SEM_H
#define SEM_H

#include "../include/expression.h"

extern bool check_if_conversion_possible(enum TokenType current, enum TokenType new_type);

extern void convert_node(struct ASTNode* node, enum TokenType new_type);

#endif // !SEM_H
