#ifndef SHUNT_YARD_ALGORITHIM_H
#define SHUNT_YARD_ALGORITHIM_H

#include "lexer.h"
#include <stdint.h>

enum Associativity
{
    LEFT,
    RIGHT,
    NO_ASSOCIATIVITY
};

struct SYNode
{
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

struct ASTNode
{
    enum TokenType op;                             
    struct ASTNode *left;                
    struct ASTNode *right;
    
    float value;    
};

struct Expression 
{
    struct Stack *output_queue;
    struct Stack *op_stack;
};

extern void RunExpression(struct Expression* expression, struct Lexer *lexer, enum TokenType stopping_token, uint32_t token_offset);

extern float CalculateExpression(struct Expression* expression);

extern struct ASTNode* CreateASTNode(enum TokenType op, struct ASTNode* left, struct ASTNode* right, float value);

extern struct ASTNode CreateASTreeFromExpression(struct Expression* expression);

extern void DestroyASTNode(struct ASTNode* root);

extern void DestroyExpression(struct Expression* expression);

#endif