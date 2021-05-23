/**
 * @file vector.c
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
 * This file contains a generic vector object that 
 * represents a common dynamic array.
 */

#include "../include/vector.h"
#include "../include/err.h"

#include <stdio.h>

struct Vector *create_vector(size_t item_size) {
    struct Vector *vector = NULL;
    vector = malloc(sizeof(struct Vector));

    if (vector == NULL) 
        fatal_error("could not allocate 'Vector'");
        
    vector->size = 0;
    vector->element_size = item_size;
    vector->array = calloc(0, item_size);
}

void free_vector(struct Vector *vector) {
    for (size_t i = 0; i < vector->size; i++)
        free(vector->array[i]);

    free(vector->array);
    free(vector);
}

void push_back_vector(struct Vector *vector, void *element) {
    vector->size++;
    vector->array = realloc(vector->array, vector->size * vector->element_size);
    vector->array[vector->size - 1] = element;
}

void pop_vector(struct Vector *vector, size_t element) {
    if (vector->size > 0) {
        vector->size--;
        vector->array = realloc(vector->array, vector->size * vector->element_size);
    }
}

void clear_vector(struct Vector* vector) {
    vector->size = 0;
    vector->array = realloc(vector->array, vector->size * vector->element_size);
}