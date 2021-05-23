/**
 * @file token.c
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
 * This file contains the token object that represents a single
 * possible token for the lexer to use.
 */

#include "../include/token.h"
#include "../include/err.h"

static int code_token_id_counter = -1;

extern int move_token_counter(int value) {
    code_token_id_counter += value;
    return code_token_id_counter;
}

extern void reset_token_counter() {
    code_token_id_counter = 0;
}

struct Token *create_token(enum TokenType type, const char* in_source_token, struct TokenInfo token_info) {
    struct Token *token;
    token = malloc(sizeof(struct Token));
    if (token == NULL) 
        fatal_error("could not allocate 'Token'");

    memset(token, 0, sizeof(struct Token));

    strcpy(token->token_string, in_source_token);
    remove_whitespaces(token->token_string);
    token->token_info = token_info;

    token->code_id = move_token_counter(1);
    token->type = type;

    return token;
}

void log_token(struct Token* token) {
    printf("Token Id[%d], Type: %d, Str: '%s', Line: %d, Pos: %d\n", token->code_id, token->type, token->token_string, token->token_info.token_line, token->token_info.token_pos);
}