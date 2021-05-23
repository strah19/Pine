#include "../include/stack.h"
#include "../include/err.h"

/**
 * @file stack.c
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
 * This file contains a generic stack object that can represent
 * a stack container.
 */

#include <stdio.h>

struct Stack *create_stack(size_t size_of_element) {
    struct Stack *stack = (struct Stack *)malloc(sizeof(struct Stack));
    
    if (stack == NULL)
        fatal_error("could not allocate 'Stack'");

    stack->top = -1;
    stack->size_of_element = size_of_element;
    stack->full_size = 0;
    stack->array = calloc(0, size_of_element);
    return stack;
}

void push_stack(struct Stack *stack, void *data) {
    stack->top++;
    if (stack->top * sizeof(data) + sizeof(data) > sizeof(stack->array)) {
        stack->array = realloc(stack->array, (stack->top + 1) * sizeof(data));
        stack->full_size++;
    }
    stack->array[stack->top] = data;
}

void *pop_stack(struct Stack *stack) {
    if (is_stack_empty(stack))
        return NULL;
    stack->top--;
    return stack->array[stack->top];
}

void *peek_stack(struct Stack *stack) {
    if (is_stack_empty(stack))
        return NULL;
    return stack->array[stack->top];
}

void destroy_stack(struct Stack *stack) {
    for (int i = 0; i < stack->full_size; i++)
        free(stack->array[i]);
    free(stack);
}

bool is_stack_empty(struct Stack *stack) {
    return (stack->top == -1);
}