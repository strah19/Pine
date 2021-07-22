#ifndef LEXER_H
#define LEXER_H

#include "../include/token.h"

#define LEXER_FILE_MODE "r"

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
    {"{", T_LCURLEY_BRACKET, false},
    {"}", T_RCURLEY_BRACKET, false},
    {"(", T_LPAR, false},
    {")", T_RPAR, false},
    {";", T_SEMI_COLON, false},
    {"[", T_LBRACKET, false},
    {"]", T_RBRACKET, false},
    {"<", T_LESS_THAN, false},
    {">", T_GREATER_THAN, false},
    {":", T_COLON , false},
    {"=", T_EQUAL, false},
    {"!=", T_NOT, false},
    {"-", T_MINUS, false},
    {"+", T_PLUS, false},
    {"*", T_STAR, false},
    {"/", T_SLASH, false},
    {",", T_COMMA, false},
    {"->", T_ARROW_PTR, false},
    {"print", T_PRINT, true},
    {"int", T_INT, true},
    {"if", T_IF, true},
    {"else", T_ELSE, true},
    {"elif", T_ELIF, true},
    {"and", T_AND, true},
    {"or", T_OR, true},
    {"while", T_WHILE, true},
    {"break", T_BREAK, true},
    {"void", T_VOID, true},
    {"char", T_CHAR, true},
    {"const", T_CONST, true},
    {"return", T_RETURN, true}
};

extern struct LexLoader create_buffer_for_lexer(const char *filepath);

extern struct Lexer *create_lexer(struct LexLoader *loader);

extern void destroy_lexer(struct Lexer *lexer);

extern void run_tokenizer(struct Lexer *lexer);

extern void log_lexer(struct Lexer *lexer);

extern void clear_lexer_data(struct Lexer* lexer);

extern const char* convert_type_to_str(enum TokenType type);

#endif // LEXER_H