#include "../include/shunt_yard_algorithim.h"
#include "../include/stack.h"
#include <math.h>

const struct TokenNode shunt_yard_operators[] = {
    {SUBTRACT, 2, LEFT, 0},
    {ADD, 2, LEFT, 0},
    {MULTIPLE, 3, LEFT, 0},
    {TO_THE_POWER_OF, 4, LEFT, 0},
    {DIVIDE, 3, LEFT, 0}};

struct Stack *RunShuntYardAlgorithim(struct Lexer *lexer)
{
    struct Stack *output_queue;
    struct Stack *op_stack;

    output_queue = CreateStack(sizeof(struct TokenNode));
    op_stack = CreateStack(sizeof(struct TokenNode));

    for (size_t i = 0; i < lexer->tokens->size; i++)
    {
        struct Token *token = lexer->tokens->array[i];

        if (token->type == INTEGER)
        {
            struct TokenNode *node;
            node = malloc(sizeof(struct TokenNode));
            node->value = atoi(token->token_string);
            node->op = FLOAT;
            PushStack(output_queue, node);
        }
        if(token->type == FLOAT) {
            char* pend;
            struct TokenNode *node;
            node = malloc(sizeof(struct TokenNode));
            node->value = strtof(token->token_string, &pend);
            node->op = token->type;
            PushStack(output_queue, node);
        }
        else if (token->type == LPAR)
        {
            struct TokenNode *node;
            node = malloc(sizeof(struct TokenNode));
            node->value = 0;
            node->op = LPAR;
            PushStack(op_stack, node);
        }
        else if (token->type == RPAR)
        {
            struct TokenNode *top = Peek(op_stack);
            while (top->op != LPAR)
            {
                PushStack(output_queue, top);
                PopStack(op_stack);
                top = Peek(op_stack);
            }
            if (top->op == LPAR)
            {
                PopStack(op_stack);
            }
        }
        for (size_t i = 0; i < sizeof(shunt_yard_operators) / sizeof(shunt_yard_operators[0]); i++)
        {
            if (token->type == shunt_yard_operators[i].op)
            {
                struct TokenNode *top = Peek(op_stack);
                if (top != NULL)
                {
                    while ((!IsEmpty(op_stack)) && ((shunt_yard_operators[i].associativity == LEFT && shunt_yard_operators[i].precedence <= top->precedence) || (shunt_yard_operators[i].associativity == RIGHT && shunt_yard_operators[i].precedence < top->precedence)) && top->op != LPAR)
                    {
                        PushStack(output_queue, top);
                        PopStack(op_stack);
                        top = Peek(op_stack);
                    }
                }

                PushStack(op_stack, (void *)&shunt_yard_operators[i]);
            }
        }
    }

    struct TokenNode *top = Peek(op_stack);
    while (!IsEmpty(op_stack))
    {
        PushStack(output_queue, top);
        PopStack(op_stack);
        top = Peek(op_stack);
    }

    for (size_t i = 0; i < output_queue->top + 1; i++)
    {
        struct TokenNode *data = output_queue->array[i];
        printf("Output Queue Element %d, Value: %f, Operator: %d\n", i, data->value, data->op);
    }

    DestroyStack(op_stack);
    return output_queue;
}

float RunPostfixNotation(struct Stack *shunt_yard_output)
{
    struct Stack *out = CreateStack(sizeof(struct TokenNode));
    struct TokenNode *current;
    float result;
    
    for (size_t i = 0; i < shunt_yard_output->top + 1; i++)
    {
        current = shunt_yard_output->array[i];
        if (current->op == INTEGER || current->op == FLOAT)
        {
            PushStack(out, current);
        }
        if (out->top >= 1 && current->op != FLOAT)
        {
            struct TokenNode *operand1 = Peek(out);
            PopStack(out);
            struct TokenNode *operand2 = Peek(out);
            PopStack(out);
            struct TokenNode *node;
            node = malloc(sizeof(struct TokenNode));
            switch (current->op)
            {
            case ADD:
            {
                node->value = operand2->value + operand1->value;
                break;
            }
            case SUBTRACT:
            {
                node->value = operand2->value - operand1->value;
                break;
            }
            case MULTIPLE:
            {
                node->value = operand2->value * operand1->value;
                break;
            }
            case DIVIDE:
            {
                node->value = operand2->value / operand1->value;
                break;
            }
            case TO_THE_POWER_OF:
            {
                node->value = powf(operand2->value, operand1->value);
                break;
            }
            default:
                break;
            }
            result = node->value;
            PushStack(out, node);
        }
    }
    DestroyStack(out);

    return result;
}