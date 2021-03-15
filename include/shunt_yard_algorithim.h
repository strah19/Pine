#ifndef SHUNT_YARD_ALGORITHIM_H
#define SHUNT_YARD_ALGORITHIM_H

#include "lexer.h"

enum Associativity {
    LEFT, RIGHT
};

struct TokenNode {
    enum TokenType op;
    int precedence;
    enum Associativity associativity;
    int value;
};

extern void RunShuntYardAlgorithim(struct Lexer* lexer);

#endif