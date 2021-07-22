#ifndef TOKEN_H
#define TOKEN_H

#include "../include/pine.h"
#include <stdint.h>

#define MAX_TOKEN_SIZE 256

enum TokenType {
    T_INTEGER, T_ID, T_IF, T_SEMI_COLON, T_LPAR, T_RPAR, T_EQUAL, T_DOUBLE_EQUAL, T_PLUS, T_MINUS, 
    T_STAR, T_SLASH, T_LBRACKET, T_RBRACKET, T_LCURLEY_BRACKET, T_INT, T_RCURLEY_BRACKET, T_ELIF, 
    T_ELSE, T_RETURN, T_WHILE, T_FOR, T_CHAR, T_LESS_THAN, T_GREATER_THAN, T_LESS_THAN_EQUAL, 
    T_GREATER_THAN_EQUAL, T_FLOAT, T_PRINT, T_AND, T_OR, T_COLON,T_NOT, T_BREAK, T_VOID, T_STR, 
    T_CONST, T_COMMA, T_ARROW_PTR, T_NONE, T_EOF,
};

struct TokenInfo {
    uint32_t token_line;
    uint32_t token_pos;
};

struct Token {
    enum TokenType type;
    char token_string[MAX_TOKEN_SIZE];
    struct TokenInfo token_info;
    uint32_t index;
};

extern int move_token_counter(int offset);

extern void reset_token_counter();

extern int get_token_counter();

extern struct Token create_token(enum TokenType type, const char *in_source_token, uint32_t line, uint32_t pos);

extern void log_token(struct Token* token);

#endif // TOKEN_H