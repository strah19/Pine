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
    bool check_next;
};

static const struct TokenPair TOKEN_PAIRS[] = {
    {"{", LCURLEY_BRACKET, false},
    {"}", RCURLEY_BRACKET, false},
    {"(", LPAR, false},
    {")", RPAR, false},
    {";", END_EXPRESSION, false},
    {"[", LBRACKET, false},
    {"]", RBRACKET, false},
    {"<", LESS_THAN, false},
    {">", GREATER_THAN, false},
    {"^", TO_THE_POWER_OF, false},
    {":", COLON , false},
    {"=", EQUAL, false},
    {"!=", NOT, false},
    {"-", SUBTRACT, false},
    {"+", ADD, false},
    {"*", MULTIPLE, false},
    {"/", DIVIDE, false},
    {"print", PRINT, true},
    {"int", INT, true},
    {"if", IF, true},
    {"else", ELSE, true},
    {"elif", ELIF, true},
    {"and", AND, true},
    {"or", OR, true},
    {"while", WHILE, true},
    {"break", BREAK, true},
    {"void", VOID, true},
    {"char", CHAR, true},
    {"const", CONST, true}
};

extern struct LexLoader create_buffer_for_lexer(const char *filepath);

extern struct Lexer *create_lexer(struct LexLoader *loader);

extern void destroy_lexer(struct Lexer *lexer);

extern void run_tokenizer(struct Lexer *lexer);

extern void log_token_data(struct Lexer *lexer);

extern void clear_lexer_data(struct Lexer* lexer);

#endif // LEXER_H