#ifndef LEXER_H
#define LEXER_H

#include "../include/token.h"
#include "../include/vector.h"

#define LEXER_FILE_MODE "r"

#define NEED_MORE_STRING_FOR_TOKEN 0
#define CREATING_TOKEN 1

struct Lexer {
    const char *input_text;
    char current_possible_token[MAX_TOKEN_SIZE];
    struct Vector *tokens;
    char current_char, next_char;
};

struct LexerLoader {
    FILE *file;
    const char *file_path;
    char *buffer;
};

struct TokenPair {
    char in_code_name[MAX_TOKEN_SIZE];
    enum TokenType type;
};

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
};

static const struct TokenPair TOKEN_KEY_WORDS[] = {
    {"print", PRINT},
    {"int", INT},
    {"if", IF},
    {"and", AND},
    {"or", OR}
};

extern struct Lexer *create_lexer(const char *text_input);

static int make_token(struct Lexer *lexer, struct TokenInfo token_pos);

static bool push_new_token(struct Lexer *lexer, size_t i, struct TokenInfo token_pos, const struct TokenPair token_pairs[]);

extern void run_tokenizer(struct Lexer *lexer);

extern void log_token_data(struct Lexer *lexer);

extern void destroy_lexer(struct Lexer *lexer);

extern void create_buffer_for_lexer(struct LexerLoader *loader);

extern void clear_lexer_data(struct Lexer* lexer);

#endif // LEXER_H