#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "vector.h"

#define LEXER_FILE_MODE "r"

#define NEED_MORE_STRING_FOR_TOKEN 0
#define CREATING_TOKEN 1

struct Lexer
{
    const char *input_text;
    char current_possible_token[MAX_TOKEN_SIZE];
    struct Vector *tokens;
    char current_char, next_char;
};

struct LexerLoader
{
    FILE *file;
    const char *file_path;
    char *buffer;
};

struct TokenPair
{
    char in_code_name[MAX_TOKEN_SIZE];
    enum TokenType type;
};

extern bool AssertLexer(struct Lexer *lexer);

extern struct Lexer * CreateLexer(const char *text_input);

static int MakeToken(struct Lexer *lexer, struct TokenInfo token_pos);

static bool PushNewToken(struct Lexer *lexer, size_t i, struct TokenInfo token_pos, const struct TokenPair token_pairs[]);

extern void RunTokenizer(struct Lexer *lexer);

extern void LogTokenData(struct Lexer *lexer);

extern void DestroyLexer(struct Lexer *lexer);

extern void CreateBufferForLexer(struct LexerLoader *loader);

extern void ClearLexerData(struct Lexer* lexer);

static const struct TokenPair TOKEN_OPERATORS[] = {
    {"-", SUBTRACT},
    {"+", ADD},
    {"*", MULTIPLE},
    {"/", DIVIDE},
};

static const struct TokenPair TOKEN_PAIRS[] = {
    {"{", LCURLEY_BRACKET},
    {"}", RCURLEY_BRACKET},
    {"(", LPAR},
    {")", RPAR},
    {";", END_EXPRESSION},
    {")", RPAR},
    {"[", LBRACKET},
    {"]", RBRACKET},
    {"<", LESS_THAN},
    {"<=", LESS_THAN_EQUAL},
    {">", GREATER_THAN},
    {">=", GREATER_THAN_EQUAL},
    {"^", TO_THE_POWER_OF},
    {"print", PRINT}
};

#endif // LEXER_H