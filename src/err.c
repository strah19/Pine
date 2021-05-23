/**
 * @file err.c
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
 * This file contains basic error handlers and logging functions to allow 
 * developers to specify different levels of errors and handle unexpected behavior 
 * that could arise.
 */

#include "../include/err.h"

void fatal_token_error(const char* error, struct Token* token) {
    fprintf(stderr, "Error: %s: '%s' on line %d.\n", error, token->token_string, token->token_info.token_line);
    exit(EXIT_FAILURE);
}

void fatal_compiler_error(const char* error, const char* s1, int line) {
    fprintf(stderr, "Error: %s: '%s' on line %d.\n", error, s1, line);
    exit(EXIT_FAILURE);
}

void fatal_error(const char* error) {
    fprintf(stderr, "Error: %s.\n", error);
    exit(EXIT_FAILURE);  
}

void token_warning(const char* warning, struct Token* token) {
    fprintf(stderr, "Warning: %s: %s on line %d.\n", warning, token->token_string, token->token_info.token_line);
}