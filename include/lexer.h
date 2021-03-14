#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "vector.h"

#define LEXER_FILE_MODE "r"

#define NEED_MORE_STRING_FOR_TOKEN 0
#define CREATING_TOKEN 1

struct Lexer {
    FILE* source_file;
    char current_possible_token[MAX_TOKEN_SIZE];
    struct Vector* tokens;
    char* copy_string_token;
    char current_char, next_char;
};

extern bool
AssertLexer ( struct Lexer* lexer );

extern struct Lexer*
CreateLexer( const char* source_file_path );

static int
MakeToken ( struct Lexer* lexer, struct TokenInfo token_pos );

extern void
RunTokenizer ( struct Lexer* lexer );

extern void
LogTokenData ( struct Lexer* lexer );

extern void
DestroyLexer ( struct Lexer* lexer );

struct TokenPair {
    const char* in_code_name;
    enum TokenType type;
};

static struct TokenPair token_pairs[] = {
    {"{", LCURLEY_BRACKET},
    {"}", RCURLEY_BRACKET},
    {"(", LPAR},
    {";", END_EXPRESSION},
    {")", RPAR},
    {"[", LBRACKET},
    {"]", RBRACKET},
    {"<", LESS_THAN},
    {"<=", LESS_THAN_EQUAL},
    {">", GREATER_THAN},
    {">=", GREATER_THAN_EQUAL},
    {"-", SUBTRACT},
    {"+", ADD},
    {"*", MULTIPLE},
    {"/", DIVIDE}
};

#endif // LEXER_H