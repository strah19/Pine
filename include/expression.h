#ifndef SHUNT_YARD_ALGORITHIM_H
#define SHUNT_YARD_ALGORITHIM_H

#include "../include/lexer.h"

#include <stdint.h>

enum Associativity {
    LEFT,
    RIGHT,
    NO_ASSOCIATIVITY
};

struct SYNode {
    enum TokenType op;
    int precedence;
    enum Associativity associativity;
    float value;
};

static const struct SYNode SHUNT_YARD_OPERATORS[] = {
    {SUBTRACT, 2, LEFT, 0},
    {ADD, 2, LEFT, 0},
    {MULTIPLE, 3, LEFT, 0},
    {TO_THE_POWER_OF, 4, LEFT, 0},
    {DIVIDE, 3, LEFT, 0}
};

struct ASTNode {
    enum TokenType op;                             
    struct ASTNode *left;                
    struct ASTNode *right;
    
    float value;    
};

struct Expression {
    struct Stack *output_queue;
    struct Stack *op_stack;
};

extern int run_expression(struct Expression* expression, struct Lexer *lexer, uint32_t token_offset);

extern float calculate_expression(struct Expression* expression);

extern struct ASTNode* create_ast_node(enum TokenType op, struct ASTNode* left, struct ASTNode* right, float value);

extern struct ASTNode create_ast_node_from_expression(struct Expression* expression);

extern void destroy_ast_node(struct ASTNode* root);

extern void destroy_expression(struct Expression* expression);

extern struct ASTNode* create_leaf_node(enum TokenType op, float value);

extern struct ASTNode* create_unary(enum TokenType op, float value, struct ASTNode* left);

#endif