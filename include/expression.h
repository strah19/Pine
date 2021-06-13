#ifndef SHUNT_YARD_ALGORITHIM_H
#define SHUNT_YARD_ALGORITHIM_H

#include "../include/parser.h"

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
    int order;
};

static const struct SYNode SHUNT_YARD_OPERATORS[] = {
    {SUBTRACT, 2, LEFT, 0},
    {ADD, 2, LEFT, 0},
    {MULTIPLE, 3, LEFT, 0},
    {TO_THE_POWER_OF, 4, LEFT, 0},
    {DIVIDE, 3, LEFT, 0},
    {GREATER_THAN, 1, LEFT, 0},
    {LESS_THAN, 1, LEFT, 0},
    {GREATER_THAN_EQUAL, 1, LEFT, 0},
    {LESS_THAN_EQUAL, 1, LEFT, 0},
    {DOUBLE_EQUAL, 1, LEFT, 0},
    {AND, 0, LEFT, 0},
    {OR, 0, LEFT, 0}
};

struct ASTNode {
    enum TokenType op;                             
    struct ASTNode *left;                
    struct ASTNode *right;
    
    union 
    {
       int int_val;
    } value;
};

struct Expression {
    struct Stack *output_queue;
    struct Stack *op_stack;
};

extern void run_expression(struct Expression* expression, struct Parser* parser);

extern float calculate_expression(struct Expression* expression);

extern struct ASTNode* create_ast_node(enum TokenType op, struct ASTNode* left, struct ASTNode* right);

extern struct ASTNode create_ast_node_from_expression(struct Expression* expression);

extern void destroy_ast_node(struct ASTNode* root);

extern void destroy_expression(struct Expression* expression);

extern struct ASTNode* create_leaf_node(enum TokenType op);

extern struct ASTNode* create_unary(enum TokenType op, struct ASTNode* left);

#endif