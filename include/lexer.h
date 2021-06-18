#ifndef LEXER_H
#define LEXER_H

#include "../include/token.h"

#define LEXER_FILE_MODE "r"

#define NEED_MORE_STRING_FOR_TOKEN 0
#define CREATING_TOKEN 1

struct Lexer {
    uint8_t *input_text;
    char current_possible_token[MAX_TOKEN_SIZE];
    uint32_t size;
    uint32_t allocated_size;
    uint32_t file_size;
    struct Token *tokens;
};

struct LexLoader {
    uint8_t* text;
    uint32_t size;
};

struct TokenPair {
    char in_code_name[MAX_TOKEN_SIZE];
    enum TokenType type;
};

static const struct TokenPair TOKEN_OPERATORS[] = {
 
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
    {">", GREATER_THAN},
    {"^", TO_THE_POWER_OF},
    {":", COLON },
    {"=", EQUAL},
    {"!", EXCLEMATION},
    {"print", PRINT},
    {"int", INT},
    {"if", IF},
    {"and", AND},
    {"or", OR},
    {"-", SUBTRACT},
    {"+", ADD},
    {"*", MULTIPLE},
    {"/", DIVIDE}
};

static const struct TokenPair TOKEN_KEY_WORDS[] = {

};

extern struct LexLoader create_buffer_for_lexer(const char *filepath);

extern struct Lexer *create_lexer(struct LexLoader *loader);

extern void destroy_lexer(struct Lexer *lexer);

extern void run_tokenizer(struct Lexer *lexer);

extern void log_token_data(struct Lexer *lexer);

extern void clear_lexer_data(struct Lexer* lexer);

#endif // LEXER_H