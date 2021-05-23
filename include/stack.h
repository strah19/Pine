#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

struct Stack {
    int top;
    void** array;
    int full_size;
    size_t size_of_element;
};

extern struct Stack *create_stack(size_t size_of_element);

extern void push_stack(struct Stack *stack, void *data);

extern void* pop_stack(struct Stack *stack);

extern void *peek_stack(struct Stack *stack);

extern void destroy_stack(struct Stack* stack);

extern bool is_stack_empty(struct Stack* stack);

#endif
