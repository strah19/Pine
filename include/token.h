#ifndef TOKEN_H
#define TOKEN_H

#include "pine.h"

#define MAX_TOKEN_SIZE 256

enum TokenType
{
    INTEGER, ID, IF, END_EXPRESSION, LPAR, RPAR, EQUAL, DOUBLE_EQUAL, ADD, SUBTRACT, MULTIPLE, DIVIDE, MAIN, LBRACKET, RBRACKET, LCURLEY_BRACKET,
    RCURLEY_BRACKET, ELIF, ELSE, RETURN, WHILE, FOR, CHAR, LESS_THAN, GREATER_THAN, LESS_THAN_EQUAL, GREATER_THAN_EQUAL, TO_THE_POWER_OF, FLOAT, PRINT,
    NONE, T_EOF
};

struct TokenInfo
{
    unsigned int token_line;
    unsigned int token_pos;
};

struct Token
{
    enum TokenType type;
    char token_string[MAX_TOKEN_SIZE];
    struct TokenInfo token_info;
    int code_id;
};

extern int MoveTokenCounter(int value);

extern void ResetTokenCounter();

extern struct Token *AddToken(enum TokenType type, const char *in_source_token, struct TokenInfo token_info);

#endif // TOKEN_H