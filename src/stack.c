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

struct Stack *create_stack(size_t size_of_element, int max_size) {
    struct Stack *stack = (struct Stack *)malloc(sizeof(struct Stack));
    
    if (stack == NULL)
        fatal_error("could not allocate 'Stack'");

    stack->top = 0;
    stack->size_of_element = size_of_element;
    stack->array = malloc(size_of_element * max_size);
    stack->max_size = max_size;
    return stack;
}

void push_stack(struct Stack *stack, void *data) {
    if(stack->top == stack->max_size)
        fatal_error("stack is full");

    int start = stack->top * stack->size_of_element;
    for(int i = 0; i < stack->size_of_element; i++) 
        *(stack->array + start + i) = *((unsigned char *)(data + i));
    stack->top = stack->top + 1;
}

void *pop_stack(struct Stack *stack) {
    if (is_stack_empty(stack))
        return NULL;

    void *elem = peek_stack(stack);
    stack->top = stack->top - 1;
    return elem;
}

void *peek_stack(struct Stack *stack) {
    if (is_stack_empty(stack))
        return NULL;

    int start = (stack->top - 1) * stack->size_of_element;
    unsigned char *elem;
    elem = (unsigned char *)malloc(stack->size_of_element);
    for(int i = 0; i < stack->size_of_element; i++) 
        *(elem + i) = *(stack->array + start + i);
    return (void *)elem;
}

extern void *get_stack(struct Stack* stack, size_t index) {
    if (index < 0 || index > stack->max_size - 1)
        fatal_error("stack index is out of range");

    int start = index * stack->size_of_element;
    unsigned char *elem;
    elem = (unsigned char *)malloc(stack->size_of_element);
    for(int i = 0; i < stack->size_of_element; i++) 
        *(elem + i) = *(stack->array + start + i);
    return (void *)elem;  
}

void destroy_stack(struct Stack *stack) {
    free(stack->array);
    free(stack);
}

bool is_stack_empty(struct Stack *stack) {
    return (stack->top == 0);
}