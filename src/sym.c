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

static struct Symbol* sym_table[TABLE_SIZE];
static uint32_t sym_index = 0;

unsigned hash(char* s);
struct Symbol* lookup(char* s);
struct Symbol* install(char* symbol_name);

unsigned hash(char* s) {
    unsigned hashval;

    for (hashval = 0; *s != '\0'; s++) 
        hashval = *s + 31 * hashval;
    return hashval % TABLE_SIZE;
}

struct Symbol* lookup(char* s) {
    struct Symbol* sym;

    for (sym = sym_table[hash(s)]; sym != NULL; sym = sym->next)
        if (strcmp(s, sym->name) == 0)
            return sym;
     return NULL;
}

struct Symbol* install(char* symbol_name) {
    struct Symbol* sym;
    unsigned hashval;
    sym = lookup(symbol_name);
    if (sym == NULL) {
        sym = (struct Symbol*) malloc(sizeof(*sym));
        if (sym == NULL)
            return NULL;
        hashval = hash(symbol_name);
        sym->next = sym_table[hashval];
        sym->id = sym_index++;
        sym->name = symbol_name;
        sym_table[hashval] = sym;
    }

    return sym;
}

void destroy_symbols() {
    for (int i = 0; i < TABLE_SIZE; i++) 
        free(sym_table[i]);
}

uint32_t search_type_symbol(char* name, enum SymType type) {
    struct Symbol* sym = lookup(name);
    if (sym == NULL)
        return -1;
    return sym->id;
}

struct Symbol* add_symbol(char* name, enum SymType type) {
    if(search_type_symbol(name, type) == -1) {
        struct Symbol* sym = install(name);
        sym->type = type;
        return sym;
    }
    else  {
        fprintf(stderr, "Error: Symbol '%s' is already defined.\n", name);
        exit(EXIT_FAILURE);
    }

    return NULL;
}

struct Symbol* get_symbol(char* name, enum SymType type) {
    uint32_t index = search_type_symbol(name, type);
    return (index != -1) ? sym_table[index] : NULL;
}

void log_symbols() {
    for (int i = 0; i < TABLE_SIZE; i++)
        if(sym_table[i] != NULL)
            printf("Symbol: '%s', Index: %d\n", sym_table[i]->name, sym_table[i]->id);
}

struct Symbol* get_symbols() {
    return *sym_table;
}

uint32_t get_sym_index() {
    return sym_index;
}

void update_sym_index(uint32_t index) {
    sym_index = index;
}