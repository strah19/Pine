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

#define NSYMBOL 1024

static struct Symbol syms[NSYMBOL];
static uint32_t sym_index = 0;

uint32_t search_all_symbol(char* name) {
    for (int i = 0; i < sym_index; i++)  
        if (syms[i].name[0] == name[0])
            if (strcmp(syms[i].name, name) == 0)
                return i;
    return -1;
}

uint32_t search_type_symbol(char* name, enum SymType type) {
    for (int i = 0; i < sym_index; i++)  
        if (syms[i].name[0] == name[0] && syms[i].type == type) 
            if (strcmp(syms[i].name, name) == 0) 
                return i;
    return -1;
}

uint32_t add_symbol(char* name, enum SymType type) {
    if(search_type_symbol(name, type) == -1) {
        syms[sym_index].name = name;
        syms[sym_index].type = type;

        sym_index++;
    }
    else  {
        fprintf(stderr, "Error: Symbol '%s' is already defined.\n", name);
        exit(EXIT_FAILURE);
    }

    return sym_index - 1;
}

struct Symbol* get_symbol(char* name, enum SymType type) {
    uint32_t index = search_type_symbol(name, type);
    return (index != -1) ? &syms[index] : NULL;
}

void log_symbols() {
    for (int i = 0; i < sym_index; i++)  
       printf("Symbol: '%s', Index: %d\n", syms[i].name, i);   
}

struct Symbol* get_symbols() {
    return syms;
}

uint32_t get_sym_index() {
    return sym_index;
}

void update_sym_index(uint32_t index) {
    sym_index = index;
}