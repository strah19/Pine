/**
 * @file lexer.c
 * @author strah19
 * @date May 23 2021
 * @version 1.2
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * @section DESCRIPTION
 *
 * This file contains lexer and tokenizer code. Main input is a text buffer
 * that tokenize's it into a vector according to specific business rules
 * that will later be used by the parser.
 */

#include "../include/lexer.h"
#include "../include/err.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#define REALLOC_TOKEN_SIZE 1000

struct Lexer *create_lexer(struct LexLoader *loader) {
    struct Lexer *lexer;
    lexer = malloc(sizeof(struct Lexer));
    if (lexer == NULL)
        fatal_error("could not allocate 'Lexer'");

    memset(lexer, 0, sizeof(struct Lexer));
    if (loader == NULL)
        fatal_error("Lexer input was incomplete");

    lexer->input_text = loader->text;
    lexer->size = 0;
    lexer->allocated_size = REALLOC_TOKEN_SIZE;
    lexer->file_size = loader->size;
    lexer->tokens = malloc(sizeof(struct Token) * REALLOC_TOKEN_SIZE);

    return lexer;
}

void push_token(struct Lexer* lexer, enum TokenType type, const char* val, uint32_t line, uint32_t pos) {
    if (lexer->size + 1 > lexer->allocated_size) {
        uint32_t new_size = (sizeof(lexer->tokens) * 2);
        lexer->tokens = realloc(lexer->tokens, (sizeof(struct Token) * lexer->size) * 2);
    if (!lexer->tokens) 
        fatal_error("Failed to resize lexical tokens");
        lexer->allocated_size = lexer->size * 2;
    }

    lexer->tokens[get_token_counter() + 1] = create_token(type, val, line, pos);
    lexer->size++;
}

void reset_lexer(uint32_t* pos, uint32_t* token_len, char* token_str) {
    *pos += *token_len;
    *token_len = 0;
    memset(token_str, 0, MAX_TOKEN_SIZE);   
}

void run_tokenizer(struct Lexer *lexer) {
    uint8_t* bp = lexer->input_text;
    uint8_t* end_byte = lexer->input_text + lexer->file_size;

    char current_token_str[MAX_TOKEN_SIZE];
    memset(current_token_str, 0, sizeof(current_token_str));
    uint32_t current_token_len = 0;

    uint32_t line = 1;
    uint32_t pos = 1;

    uint8_t comment_counter = 0;
    uint8_t single_line = 0;

    bool possible_variable = false;

    while (*bp != '`' && !single_line && !comment_counter) {
        if (*bp == '\n') {
            if (single_line)
                single_line = 0;
            line++;
            pos = 1;
        }

        if (*bp == '/' && *(bp + 1) == '*') {
            bp++;
            comment_counter++;
        }
        else if (*bp == '*' && *(bp + 1) == '/') {
            bp++;
            comment_counter--;
        }
        else if (*bp == '/' && *(bp + 1) == '/') {
            bp++;
            single_line = 1;
        }
        else if(*bp != ' ' && *bp != '\n' && !comment_counter && !single_line) {
            current_token_str[current_token_len] = *bp;
            current_token_len++;

            if (is_char_digit((char) *bp) && !is_char_digit((char) *(bp + 1))) {
                push_token(lexer, INTEGER, current_token_str, line, pos);
                reset_lexer(&pos, &current_token_len, current_token_str);
            }
            else {
                if (*bp == '=' && *(bp + 1) == '=') {
                    push_token(lexer, DOUBLE_EQUAL, "==", line, pos);
                    reset_lexer(&pos, &current_token_len, current_token_str);
                    bp++;
                }
                else if (*bp == '<' && *(bp + 1) == '=') {
                    push_token(lexer, DOUBLE_EQUAL, "<=", line, pos);
                    reset_lexer(&pos, &current_token_len, current_token_str);
                    bp++;
                }
                else if (*bp == '>' && *(bp + 1) == '=') {
                    push_token(lexer, DOUBLE_EQUAL, ">=", line, pos);
                    reset_lexer(&pos, &current_token_len, current_token_str);
                    bp++;
                }

                else {
                    bool check_for_var = true;
                    for (int i = 0; i < sizeof(TOKEN_PAIRS) / sizeof(TOKEN_PAIRS[0]); i++) {
                        if (current_token_str[0] != TOKEN_PAIRS[i].in_code_name[0])
                            continue;
                        bool cont = (!TOKEN_PAIRS[i].check_next) ? true : !(is_char_good_for_variable_name((char)(*(bp + 1))));
                        if (strcmp(current_token_str, TOKEN_PAIRS[i].in_code_name) == 0 && cont) {
                            push_token(lexer, TOKEN_PAIRS[i].type, TOKEN_PAIRS[i].in_code_name, line, pos);
                            reset_lexer(&pos, &current_token_len, current_token_str);
                            check_for_var = false;
                            break;
                        }    
                    }

                    if (check_for_var) {
                        possible_variable = is_char_good_for_variable_name((char)*bp);
                        if (possible_variable && !is_char_good_for_variable_name((char)(*(bp + 1)))) {
                            push_token(lexer, ID, current_token_str, line, pos);
                            reset_lexer(&pos, &current_token_len, current_token_str);  
                            possible_variable = false;
                        }
                    }
                }
            }
        }
        
        bp++;
    }

    push_token(lexer, T_EOF, "-1", line, pos);

    if (comment_counter != 0)
        fatal_error("Mismatched multi-line comment");
}

struct LexLoader create_buffer_for_lexer(const char *filepath) {
    struct LexLoader loader;
    FILE* file;
    int size;

    if(file = fopen(filepath, LEXER_FILE_MODE)) {
        struct stat st;

        if (fstat(fileno(file), &st) != -1) {
            loader.text = (uint8_t *) malloc(st.st_size);
            loader.size = st.st_size;
            fread((void*) loader.text, 1, st.st_size, file);
        }
        else 
            fatal_error("Failed to load stats of input file");
    }
    else 
        fatal_error("Failed to open input file for compilation");

    return loader;
}   

void log_token_data(struct Lexer *lexer) {
    for (size_t i = 0; i < lexer->size; i++) {
        struct Token *token = &lexer->tokens[i];
        printf("Token[%d], Type: %d, { %s } ln: %d, pos: %d\n", token->code_id, token->type, token->token_string, token->token_info.token_line, token->token_info.token_pos);
    }
}

void destroy_lexer(struct Lexer *lexer) {
    free(lexer->tokens);
    free(lexer);
}

void clear_lexer_data(struct Lexer *lexer) {
    lexer->input_text = "";
    lexer->current_possible_token[0] = '\0';
    reset_token_counter();
}