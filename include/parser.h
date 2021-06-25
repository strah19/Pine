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

static bool run_statements(struct Parser* parser);

static void expression_assignment(struct Parser* parser, struct ASTNode** root);

static void print_statement(struct Parser* parser);

static void assignment_statement(struct Parser* parser);

static void if_statement(struct Parser* parser);

static void else_statement(struct Parser* parser);

static void elif_statement(struct Parser* parser);

static int equal_statement(struct Parser* parser, int end_token, struct Token* var_token, struct ASTNode** root);

static void while_statement(struct Parser* parser);

static void break_statement(struct Parser* parser);

#endif //!PARSER_H