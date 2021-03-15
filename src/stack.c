#include "../include/stack.h"

struct Stack *CreateStack(size_t size_of_element)
{
    struct Stack *stack = (struct Stack *)malloc(sizeof(struct Stack));
    stack->top = -1;
    stack->size_of_element = size_of_element;
    stack->array = calloc(0, size_of_element);
    return stack;
}

void PushStack(struct Stack *stack, void *data)
{
    stack->top++;
    if(stack->top * sizeof(data) + sizeof(data) > sizeof(stack->array))
        stack->array = realloc(stack->array, (stack->top + 1) * sizeof(data));
    stack->array[stack->top] = data;
}

void *PopStack(struct Stack *stack)
{
    if (IsEmpty(stack))
        return NULL;
    stack->top--;
    return stack->array[stack->top];
}

void *Peek(struct Stack *stack)
{
    if (IsEmpty(stack))
        return NULL;
    return stack->array[stack->top];
}

void DestroyStack(struct Stack *stack)
{
    free(stack->array);
    free(stack);
}

bool IsEmpty(struct Stack *stack)
{
    return (stack->top == -1);
}