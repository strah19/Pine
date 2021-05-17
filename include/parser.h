#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdint.h>

#define MAX_TOKEN_IN_EXPRESSION 64

struct Parser 
{
    struct Lexer* lexer;
    uint32_t token_index;
};

extern struct Parser* CreateParser(struct Lexer* lexer);

extern void DestroyParser(struct Parser* parser);

extern void RunParser(struct Parser* parser);

extern struct Token* PeekNextToken(struct Parser* parser);  //Only see current token.

extern struct Token* RetreaveNextToken(struct Parser* parser);  //Get current token and then increment to next token.

extern void MatchToken(struct Parser* parser, enum TokenType type, const char* what);

#endif //!PARSER_H