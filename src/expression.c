#include "../include/expression.h"
#include "../include/stack.h"
#include <math.h>

void RunExpression(struct Expression* expression, struct Lexer *lexer, enum TokenType stopping_token, uint32_t token_offset)
{
    expression->output_queue = CreateStack(sizeof(struct SYNode));
    expression->op_stack = CreateStack(sizeof(struct SYNode));

    for (size_t i = token_offset; i < lexer->tokens->size; i++)
    {
        struct Token *token = lexer->tokens->array[i];
        if(token->type == stopping_token)
            break;

        if (token->type == INTEGER)
        {
            struct SYNode *node;
            node = malloc(sizeof(struct SYNode));
            node->value = atoi(token->token_string);
            node->op = FLOAT;
            PushStack(expression->output_queue, node);  
        }
        if (token->type == FLOAT)
        {
            char *pend;
            struct SYNode *node;
            node = malloc(sizeof(struct SYNode));
            node->value = strtof(token->token_string, &pend);
            node->op = token->type;
            PushStack(expression->output_queue, node);
        }
        else if (token->type == LPAR)
        {
            struct SYNode *node;
            node = malloc(sizeof(struct SYNode));
            node->value = 0;
            node->op = LPAR;
            PushStack(expression->op_stack, node);
        }
        else if (token->type == RPAR)
        {
            struct SYNode *top = Peek(expression->op_stack);
            while (top->op != LPAR)
            {
                PushStack(expression->output_queue, top);
                PopStack(expression->op_stack);
                top = Peek(expression->op_stack);
            }
            if (top->op == LPAR)
            {
                PopStack(expression->op_stack);
            }
        }
        for (size_t i = 0; i < sizeof(SHUNT_YARD_OPERATORS) / sizeof(SHUNT_YARD_OPERATORS[0]); i++)
        {
            if (token->type == SHUNT_YARD_OPERATORS[i].op)
            {
                struct SYNode *top = Peek(expression->op_stack);
                if (top != NULL)
                {
                    while ((!IsEmpty(expression->op_stack)) && ((SHUNT_YARD_OPERATORS[i].associativity == LEFT && SHUNT_YARD_OPERATORS[i].precedence <= top->precedence) || (SHUNT_YARD_OPERATORS[i].associativity == RIGHT && SHUNT_YARD_OPERATORS[i].precedence < top->precedence)) && top->op != LPAR)
                    {
                        PushStack(expression->output_queue, top);
                        PopStack(expression->op_stack);
                        top = Peek(expression->op_stack);
                    }
                }

                PushStack(expression->op_stack, (void *)&SHUNT_YARD_OPERATORS[i]);
            }
        }
    }

    struct SYNode *top = Peek(expression->op_stack);
    while (!IsEmpty(expression->op_stack))
    {
        PushStack(expression->output_queue, top);
        PopStack(expression->op_stack);
        top = Peek(expression->op_stack);
    }

    /*
    for (size_t i = 0; i < output_queue->top + 1; i++)
    {
        struct SYNode *data = output_queue->array[i];
        printf("Output Queue Element %d, Value: %f, Operator: %d\n", i, data->value, data->op);
    }
    */
}

float CalculateExpression(struct Expression* expression)
{
    struct Stack *out = CreateStack(sizeof(struct SYNode));
    float result;

    for (size_t i = 0; i < expression->output_queue->top + 1; i++)
    {
        struct SYNode * current = expression->output_queue->array[i];
        if (current->op == INTEGER || current->op == FLOAT)
        {
            PushStack(out, current);
        }
        if (out->top >= 1 && current->op != FLOAT)
        {
            struct SYNode *operand1 = Peek(out);
            PopStack(out);
            struct SYNode *operand2 = Peek(out);
            PopStack(out);
            struct SYNode *node;
            node = malloc(sizeof(struct SYNode));

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

struct ASTNode* CreateASTNode(enum TokenType op, struct ASTNode* left, struct ASTNode* right, float value)
{
    struct ASTNode* node = NULL;

    node = malloc(sizeof(struct ASTNode));

    if(node == NULL)
    {
        fprintf(stderr, "Unable to malloc in CreateASTNode()\n");
        exit(EXIT_FAILURE);
    }

    memset(node, 0, sizeof(struct ASTNode));

    node->left = left;
    node->right = right;
    node->op = op;
    node->value = value;

    return node;
}

void TransverseTree(struct ASTNode* root, int space)
{
    if (root == NULL)
        return;
    space += 10;
    TransverseTree(root->right, space);
  
    printf("\n");
    for (int i = 10; i < space; i++)
        printf(" ");
    printf("Node:%d   %f\n", root->op, root->value);
  
    TransverseTree(root->left, space);
}

struct ASTNode CreateASTreeFromExpression(struct Expression* expression)
{
    struct Stack *out = CreateStack(sizeof(struct ASTNode));
    struct SYNode *current;

    for (size_t i = 0; i < expression->output_queue->top + 1; i++)
    {
        current = expression->output_queue->array[i];
        if (current->op == INTEGER || current->op == FLOAT)
        {
            PushStack(out, CreateASTNode(FLOAT, NULL, NULL, current->value));
        }
        if (out->top >= 1 && current->op != FLOAT)
        {
            struct ASTNode *operand1 = Peek(out);
            PopStack(out);
            struct ASTNode *operand2 = Peek(out);
            PopStack(out);
            struct ASTNode *node;

            node = CreateASTNode(current->op, operand1, operand2, 0.0);

            PushStack(out, node);
        }
    }
    struct ASTNode* r = Peek(out);
    struct ASTNode root = *r;

    TransverseTree(&root, 0);
    DestroyStack(out);

    return root;
}

void DestroyASTNode(struct ASTNode* root)
{
    if(root == NULL)
        return;

    free(root);
    DestroyASTNode(root->left);
    DestroyASTNode(root->right);
}

void DestroyExpression(struct Expression* expression)
{
    DestroyStack(expression->op_stack);
    DestroyStack(expression->output_queue);
}