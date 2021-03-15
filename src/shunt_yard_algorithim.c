#include "../include/shunt_yard_algorithim.h"
#include "../include/stack.h"

const struct TokenNode shunt_yard_operators[] = {
    {SUBTRACT, 2, LEFT, 0},
    {ADD, 2, LEFT, 0},
    {MULTIPLE, 3, LEFT, 0},
    {DIVIDE, 3, LEFT, 0}};

void RunShuntYardAlgorithim(struct Lexer *lexer)
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
            node->op = INTEGER;
            PushStack(output_queue, node);
        }    
        else
        {
            for (size_t i = 0; i < sizeof(shunt_yard_operators) / sizeof(shunt_yard_operators[0]); i++)
            {
                if (token->type == shunt_yard_operators[i].op)
                {
                    
                    struct TokenNode* top = Peek(op_stack);
                    if(top != NULL){
                        while((!IsEmpty(op_stack)) && ((shunt_yard_operators[i].associativity == LEFT && shunt_yard_operators[i].precedence <= top->precedence) || 
                                                    (shunt_yard_operators[i].associativity == RIGHT && shunt_yard_operators[i].precedence < top->precedence))) {
                            PushStack(output_queue, top);           
                            PopStack(op_stack);
                            top = Peek(op_stack);
                        }
                    }
                    

                    PushStack(op_stack, (void*) &shunt_yard_operators[i]);           
                }
            }
        }
    }
    
    struct TokenNode* top = Peek(op_stack);
    while (!IsEmpty(op_stack))
    {
        PushStack(output_queue, top);           
        PopStack(op_stack);
        top = Peek(op_stack);
    }

    for(size_t i = 0; i < output_queue->top + 1; i++) {
        struct TokenNode *data = output_queue->array[i];
        printf("Output Queue Element %d, Value: %d, Operator: %d\n", i, data->value, data->op);
    }

    DestroyStack(output_queue);
    DestroyStack(op_stack);
}