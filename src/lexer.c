/**
 * @file lexer.c
 * @author strah19
 * @date May 23 2021
 * @version 1.0
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

struct Lexer *create_lexer(const char *text_input) {
    struct Lexer *lexer;
    lexer = malloc(sizeof(struct Lexer));
    if (lexer == NULL)
        fatal_error("could not allocate 'Lexer'");

    memset(lexer, 0, sizeof(struct Lexer));
    if (text_input == NULL)
        fatal_error("Lexer input was incomplete");

    lexer->input_text = text_input;
    lexer->tokens = create_vector(sizeof(struct Token));

    return lexer;
}

int make_token(struct Lexer *lexer, struct TokenInfo token_pos) {
    for (size_t i = 0; i < sizeof(TOKEN_PAIRS) / sizeof(TOKEN_PAIRS[0]); i++)
        if (push_new_token(lexer, i, token_pos, TOKEN_PAIRS))
            return CREATING_TOKEN;
    for (size_t i = 0; i < sizeof(TOKEN_OPERATORS) / sizeof(TOKEN_OPERATORS[0]); i++)
        if (push_new_token(lexer, i, token_pos, TOKEN_OPERATORS))
            return CREATING_TOKEN;
    for (size_t i = 0; i < sizeof(TOKEN_KEY_WORDS) / sizeof(TOKEN_KEY_WORDS[0]); i++)
        if (push_new_token(lexer, i, token_pos, TOKEN_KEY_WORDS))
            return CREATING_TOKEN;

    if (lexer->current_possible_token[0] == '=') {
        push_back_vector(lexer->tokens, create_token(EQUAL, lexer->current_possible_token, token_pos));
        struct Token *token_1 = lexer->tokens->array[lexer->tokens->size - 1];
        struct Token *token_2 = lexer->tokens->array[lexer->tokens->size - 2];

        if (token_1->type == EQUAL && token_2->type == EQUAL) {
            if (token_1->token_info.token_pos - token_2->token_info.token_pos == 1) {
                pop_vector(lexer->tokens, lexer->tokens->size - 1);
                pop_vector(lexer->tokens, lexer->tokens->size - 2);
                move_token_counter(-2);
                push_back_vector(lexer->tokens, create_token(DOUBLE_EQUAL, "==", token_pos));
            }
        }
        if(token_1->type == EQUAL && token_2->type == LESS_THAN) {
            pop_vector(lexer->tokens, lexer->tokens->size - 1);
            pop_vector(lexer->tokens, lexer->tokens->size - 2);
            move_token_counter(-2);
            push_back_vector(lexer->tokens, create_token(LESS_THAN_EQUAL, "<=", token_pos));
        }
        if(token_1->type == EQUAL && token_2->type == GREATER_THAN) {
            pop_vector(lexer->tokens, lexer->tokens->size - 1);
            pop_vector(lexer->tokens, lexer->tokens->size - 2);
            move_token_counter(-2);
            push_back_vector(lexer->tokens, create_token(GREATER_THAN, ">=", token_pos));
        }
        return CREATING_TOKEN;
    }
    else if (is_char_digit(lexer->current_possible_token[0]) || lexer->current_char == '.') {
        if (!is_char_digit((char)lexer->next_char) && lexer->next_char != '.') {
            int is_float = strchr(lexer->current_possible_token, '.') != NULL;
            if (is_float)
                push_back_vector(lexer->tokens, create_token(FLOAT, lexer->current_possible_token, token_pos));
            else
                push_back_vector(lexer->tokens, create_token(INTEGER, lexer->current_possible_token, token_pos));
            return CREATING_TOKEN;
        }
    }
    else if (is_char_good_for_variable_name(lexer->current_possible_token[0])) {
        for (size_t i = 0; i < sizeof(TOKEN_KEY_WORDS) / sizeof(TOKEN_KEY_WORDS[0]); i++) {
            if (!is_char_good_for_variable_name((char)lexer->next_char) && strcmp(lexer->current_possible_token, TOKEN_KEY_WORDS[i].in_code_name) != 0) {
                push_back_vector(lexer->tokens, create_token(ID, lexer->current_possible_token, token_pos));
                return CREATING_TOKEN;
            }
        }
    }

    return NEED_MORE_STRING_FOR_TOKEN;
}

bool push_new_token(struct Lexer *lexer, size_t i, struct TokenInfo token_pos, const struct TokenPair token_pairs[]) {
    if(*lexer->current_possible_token != token_pairs[i].in_code_name[0])
        return false;
    if (adv_string_compare(lexer->current_possible_token, token_pairs[i].in_code_name, strlen(token_pairs[i].in_code_name)) == 0) {
        push_back_vector(lexer->tokens, create_token(token_pairs[i].type, token_pairs[i].in_code_name, token_pos));
        return true;
    }
    return false;
}

void run_tokenizer(struct Lexer *lexer) {
    int get_char = 0;
    int current_character = 0;
    int next_character = 0;
    int token_size = 0;
    bool wait_on_loop = false;
    struct TokenInfo token_position = {1, 0};
    int current_lexer_state = NEED_MORE_STRING_FOR_TOKEN;
    int counter = 0;

    while (counter <= strlen(lexer->input_text)) {
        get_char = lexer->input_text[counter];
        counter++;

        if (!wait_on_loop)
            current_character = get_char;

        next_character = get_char;

        lexer->current_char = current_character;
        lexer->next_char = next_character;
        if (wait_on_loop) {
            switch (current_lexer_state) {
            case NEED_MORE_STRING_FOR_TOKEN: {
                if (current_character != ' ' && current_character != '\n' && token_size < MAX_TOKEN_SIZE) {
                    lexer->current_possible_token[token_size] = (char)current_character;
                    token_size++;
                }
            }
            break;
            case CREATING_TOKEN: {
                for (int i = 0; i < token_size; i++) {
                    lexer->current_possible_token[i] = ' ';
                }
                lexer->current_possible_token[1] = '\0';

                token_size = 0;
                if (current_character != ' ' && current_character != '\n' && token_size < MAX_TOKEN_SIZE) {
                    lexer->current_possible_token[token_size] = (char)current_character;
                    token_size++;
                }
            }
            break;
            }
        }

        current_lexer_state = make_token(lexer, token_position);

        current_character = next_character;
        if (current_character == '\n') {
            token_position.token_line++;
            token_position.token_pos = 0;
        }
        token_position.token_pos++;

        wait_on_loop = true;
    }

    push_back_vector(lexer->tokens, create_token(T_EOF, "EOF", token_position));
}

extern void create_buffer_for_lexer(struct LexerLoader *loader)
{
    loader->file = fopen(loader->file_path, LEXER_FILE_MODE);
    if (loader->file != NULL) {
        if (fseek(loader->file, 0L, SEEK_END) == 0) {
            long buf_size = ftell(loader->file);
            if (buf_size == -1)
            { 
            }

            loader->buffer = malloc(sizeof(char) * (buf_size + 1));

            if (fseek(loader->file, 0L, SEEK_SET) != 0)
            { 
            }

            size_t new_len = fread(loader->buffer, sizeof(char), buf_size, loader->file);
            if (ferror(loader->file) != 0) {
                fputs("Error reading file", stderr);
            }
            else {
                loader->buffer[new_len++] = '\0'; 
            }
        }
        fclose(loader->file);
    }
}

void log_token_data(struct Lexer *lexer) {
    for (size_t i = 0; i < lexer->tokens->size; i++) {
        struct Token *token = lexer->tokens->array[i];
        printf("Token[%d], Type: %d, { %s } ln: %d, pos: %d\n", token->code_id, token->type, token->token_string, token->token_info.token_line, token->token_info.token_pos);
    }
}

void destroy_lexer(struct Lexer *lexer) {
    free_vector(lexer->tokens);
    free(lexer);
}

void clear_lexer_data(struct Lexer *lexer) {
    lexer->input_text = "";
    lexer->current_possible_token[0] = '\0';
    clear_vector(lexer->tokens);
    lexer->next_char = ' ';
    lexer->current_char = ' ';
    reset_token_counter();
}