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

static struct Symbol global_syms[NSYMBOL];
static int global_sym_index = 0;

int find_global_symbol(char* name) {
    for (int i = 0; i < global_sym_index; i++)  
        if(strcmp(global_syms[i].name, name) == 0)
            return i;
    return -1;
}

int add_symbol(char* name, float value) {
    if(find_global_symbol(name) == -1) {
        global_syms[global_sym_index].name = name;
        global_syms[global_sym_index].value = value;
        global_sym_index++;
    }
    else  {
        fprintf(stderr, "Error: Symbol '%s' is already defined.\n", name);
        exit(EXIT_FAILURE);
    }

    return global_sym_index - 1;
}

struct Symbol* get_global_symbol(char* name) {
    int index = find_global_symbol(name);
    return (index != -1) ? &global_syms[index] : NULL;
}

void log_symbols() {
    for (int i = 0; i < global_sym_index; i++)  
       printf("Symbol: '%s', Index: %d\n", global_syms[i].name, i);   
}

struct Symbol* get_symbols() {
    return global_syms;
}

int get_sym_index() {
    return global_sym_index;
}

void update_sym_index(int index) {
    global_sym_index = index;
}