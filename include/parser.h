#ifndef PARSER_H
#define PARSER_H

#include "../include/lexer.h"

#include <stdint.h>

#define MAX_TOKEN_IN_EXPRESSION 64

struct Parser  {
    struct Lexer* lexer;
    uint32_t token_index;
};

extern struct Parser* create_parser(struct Lexer* lexer);

extern void destroy_parser(struct Parser* parser);

extern void run_parser(struct Parser* parser);

extern struct Token* peek_next_token(struct Parser* parser);  //Only see current token.

extern struct Token* retrieve_next_token(struct Parser* parser);  //Get current token and then increment to next token.

extern void match_token(struct Parser* parser, enum TokenType type, const char* what);

static void print_statement(struct Parser* parser);

static void assignment_statement(struct Parser* parser);

static void variable_decleration_statement(struct Parser* parser);

static int equal_statement(struct Parser* parser, int end_token);

#endif //!PARSER_H