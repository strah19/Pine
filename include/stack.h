#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define STACK_SIZE 100

struct Stack {
    int top;
    unsigned char* array;
    size_t size_of_element;
    int max_size;
};

extern struct Stack *create_stack(size_t size_of_element, int max_size);

extern void push_stack(struct Stack *stack, void *data);

extern void* pop_stack(struct Stack *stack);

extern void *peek_stack(struct Stack *stack);

extern void *get_stack(struct Stack* stack, size_t index);

extern void destroy_stack(struct Stack* stack);

extern bool is_stack_empty(struct Stack* stack);

#endif
