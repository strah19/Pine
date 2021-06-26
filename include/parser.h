#ifndef PARSER_H
#define PARSER_H

#include "../include/lexer.h"
#include "../include/expression.h"
#include "../include/bytecode_builder.h"

#include <stdint.h>

#define MAX_TOKEN_IN_EXPRESSION 64

struct Parser  {
    struct Lexer* lexer;
    struct ByteCodeBuilder* bc_builder;
    uint32_t token_index;
};

extern struct Parser* create_parser(struct Lexer* lexer, struct ByteCodeBuilder* bc_builder);

extern void destroy_parser(struct Parser* parser);

extern void run_parser(struct Parser* parser);

extern struct Token* peek_next_token(struct Parser* parser);  //Only see current token.

extern struct Token* retrieve_next_token(struct Parser* parser);  //Get current token and then increment to next token.

extern void match_token(struct Parser* parser, enum TokenType type, const char* what);

#endif //!PARSER_H