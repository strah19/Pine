#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

struct Stack
{
    int top;
    void** array;
    size_t size_of_element;
};

extern struct Stack *CreateStack(size_t size_of_element);

extern void PushStack(struct Stack *stack, void *data);

extern void* PopStack(struct Stack *stack);

extern void *Peek(struct Stack *stack);

extern void DestroyStack(struct Stack* stack);

extern bool IsEmpty(struct Stack* stack);

#endif
