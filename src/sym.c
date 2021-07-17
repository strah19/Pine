/**
 * @file sym.c
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
 * This file contains code for variables and functions defined in 
 * the language. It stores and completes operations to determine
 * the semantics of the parsing code.
 */

#include "../include/sym.h"
#include "../include/err.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 1024

static struct Symbol sym_table[TABLE_SIZE];
static uint32_t sym_index = 0;

struct VariableType var_types[] = {
    {"int", INT, 4, INTEGER},
    {"char", CHAR, 1, CHAR}
};

uint32_t search_type_symbol(char* name, enum SymType type) {
    for (int i = 0; i < sym_index; i++)
        if (sym_table[i].name[0] == name[0] && sym_table[i].type == type)
            if (strcmp(sym_table[i].name, name) == 0)
                return i;
    return -1;
}

struct Symbol* add_symbol(char* name, enum SymType type) {
    if (search_type_symbol(name, type) == -1) {
        sym_table[sym_index].name = name;
        sym_table[sym_index].type = type;
        sym_table[sym_index].id = sym_index;

        sym_index++;
    }
    else {
        fprintf(stderr, "Error: Symbol '%s' is already defined.\n", name);
        exit(EXIT_FAILURE);
    }

    return  &sym_table[sym_index - 1];
}

struct Symbol* get_symbol(char* name, enum SymType type) {
    uint32_t index = search_type_symbol(name, type);
    return (index != -1) ? &sym_table[index] : NULL;
}

void log_symbols() {
    for (int i = 0; i < sym_index; i++)
        printf("Symbol: '%s', Index: %d, Type: %d\n", sym_table[i].name, i, sym_table[i].var.type);
}

struct Symbol* get_symbols() {
    return sym_table;
}

uint32_t get_sym_index() {
    return sym_index;
}

void update_sym_index(uint32_t index) {
    sym_index = index;
}

struct VariableType* get_variable_types(enum TokenType type) {
    for (int i = 0; i < sizeof(var_types) / sizeof(var_types[0]); i++) {
        if (var_types[i].type == type)
            return &var_types[i];
    }
    return NULL;
}