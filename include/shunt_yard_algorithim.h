#ifndef SHUNT_YARD_ALGORITHIM_H
#define SHUNT_YARD_ALGORITHIM_H

#include "lexer.h"

enum Associativity
{
    LEFT,
    RIGHT
};

struct TokenNode
{
    enum TokenType op;
    int precedence;
    enum Associativity associativity;
    float value;
};

extern struct Stack *RunShuntYardAlgorithim(struct Lexer *lexer);

extern float RunPostfixNotation(struct Stack *shunt_yard_output);

#endif