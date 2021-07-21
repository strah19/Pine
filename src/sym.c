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
static uint32_t var_id_counter = 0;
static struct FuncSym* function = NULL;

struct VariableType var_types[] = {
    {"int", INT, 4, INTEGER},
    {"char", CHAR, 1, CHAR}
};

void set_current_function(struct FuncSym* func) {
    function = func;
}

struct FuncSym* get_function() {
    return function;
}

uint32_t search_type_symbol(char* name, enum SymType type) {
    if (function) 
        for (int j = 0; j < function->arg_nums; j++) 
            if (function->arg_info[j].name[0] == name[0]) 
                if (strcmp(function->arg_info[j].name, name) == 0) 
                    return function->arg_info[j].var_info.id;

    for (int i = 0; i < sym_index; i++) 
        if (sym_table[i].name[0] == name[0] && sym_table[i].type == type)
            if (strcmp(sym_table[i].name, name) == 0)
                return i;
    return UNKNOWN_ID;
}

struct Symbol* add_symbol(char* name, enum SymType type) {
    if (search_type_symbol(name, type) == UNKNOWN_ID) {
        sym_table[sym_index].name = name;
        sym_table[sym_index].type = type;
        sym_table[sym_index].index = sym_index;

        if (type == VAR) {
            sym_table[sym_index].var.id = var_id_counter++;
        }
        else if (type == FUNC) {
            sym_table[sym_index].function.arg_nums = 0;
        }

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
    return (index != UNKNOWN_ID) ? &sym_table[index] : NULL;
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

void fill_func_info(struct ArgSym* arg, struct VariableType* var_type, int* id, bool is_const, const char* token_str) {
    arg->var_info.is_const = is_const;
    arg->var_info.type = var_type->type;
    arg->var_info.value_type = var_type->value_type;
    arg->var_info.size = var_type->size;
    arg->var_info.id = (*id)--;
    arg->name = (char*) token_str;
}

void fill_return_info(struct FuncSym* func, enum TokenType type, uint32_t size, enum TokenType value, bool is_const, int id) {
    func->return_info.id = id;
    func->return_info.type = type;
    func->return_info.size = size;
    func->return_info.value_type = value;
    func->return_info.is_const = is_const;
}