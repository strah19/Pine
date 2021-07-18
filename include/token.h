#ifndef TOKEN_H
#define TOKEN_H

#include "../include/pine.h"
#include <stdint.h>

#define MAX_TOKEN_SIZE 256

enum TokenType {
    INTEGER, ID, IF, END_EXPRESSION, LPAR, RPAR, EQUAL, DOUBLE_EQUAL, ADD, SUBTRACT, MULTIPLE, DIVIDE, MAIN, LBRACKET, RBRACKET, LCURLEY_BRACKET,
    INT, RCURLEY_BRACKET, ELIF, ELSE, RETURN, WHILE, FOR, CHAR, LESS_THAN, GREATER_THAN, LESS_THAN_EQUAL, GREATER_THAN_EQUAL, TO_THE_POWER_OF, FLOAT, PRINT, AND, OR, COLON,
    EXCLEMATION, NOT, BREAK, VOID, STR, CONST, T_FUNC,
    NONE, T_EOF, 
};

struct TokenInfo {
    unsigned int token_line;
    unsigned int token_pos;
};

struct Token {
    enum TokenType type;
    char token_string[MAX_TOKEN_SIZE];
    struct TokenInfo token_info;
    int code_id;
};

extern int move_token_counter(int value);

extern void reset_token_counter();

extern int get_token_counter();

extern struct Token create_token(enum TokenType type, const char *in_source_token, uint32_t line, uint32_t pos);

extern void log_token(struct Token* token);

#endif // TOKEN_H