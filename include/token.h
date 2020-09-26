#ifndef TOKEN_H
#define TOKEN_H

#include "pine.h"

#define MAX_TOKEN_SIZE 256

enum TokenType {
    INTEGER, ID, IF, END_EXPRESSION, LPAR, RPAR, EQUAL, 
    DOUBLE_EQUAL, ADD, SUBTRACT, MULTIPLE, DIVIDE
};

struct TokenInfo {
    unsigned int token_line;
    unsigned int token_pos;
};

struct Token {
    enum TokenType type;
    char* token_string;
    struct TokenInfo token_info;
    int code_id;
};

extern int 
MoveTokenCounter ( int value );

extern struct Token* 
AddToken ( enum TokenType type, const char* in_source_token, struct TokenInfo token_info, char* copy_string );

#endif // TOKEN_H