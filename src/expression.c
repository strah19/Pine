/**
 * @file expression.c
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
 * This file contains code for constructing postfix notation for expressions 
 * and calculating them. It also contains AST nodes and functions to create
 * an AST tree out of an expression.
 */

#include "../include/expression.h"
#include "../include/sym.h"
#include "../include/stack.h"
#include "../include/err.h"

#include <math.h>

void run_expression(struct Expression* expression, struct Parser* parser) {
    int token_stoppage = 0;

    expression->output_queue = create_stack(sizeof(struct SYNode), STACK_SIZE);
    expression->op_stack = create_stack(sizeof(struct SYNode), STACK_SIZE);

    for (size_t i = parser->token_index; i < parser->lexer->tokens->size; i++) {
        bool stop = false;
        struct Token *token = parser->lexer->tokens->array[i];
        
        if (token->type == INTEGER) {
            struct SYNode node;
            node.value = atoi(token->token_string);
            node.op = FLOAT;
            push_stack(expression->output_queue, (void*) &node);  
        }
        else if (token->type == FLOAT) {
            char *pend;
            struct SYNode *node;
            node = malloc(sizeof(struct SYNode));
            node->value = strtof(token->token_string, &pend);
            node->op = token->type;
            push_stack(expression->output_queue, node);
        }
        else if(token->type == ID) {
            struct SYNode *node;
            node = malloc(sizeof(struct SYNode));
            node->value = get_global_symbol(token->token_string)->value;
            node->op = FLOAT;
            push_stack(expression->output_queue, node);  
        }
        else if (token->type == LPAR) {
            struct SYNode *node;
            node = malloc(sizeof(struct SYNode));
            node->value = 0;
            node->op = LPAR;
            push_stack(expression->op_stack, node);
        }
        else if (token->type == RPAR) {
            struct SYNode *top = peek_stack(expression->op_stack);
            while (top->op != LPAR) {
                push_stack(expression->output_queue, top);
                pop_stack(expression->op_stack);
                top = peek_stack(expression->op_stack);
            }
            if (top->op == LPAR)
                pop_stack(expression->op_stack);
        }
        else
            stop = true;
        for (size_t i = 0; i < sizeof(SHUNT_YARD_OPERATORS) / sizeof(SHUNT_YARD_OPERATORS[0]); i++) {
            if (token->type == SHUNT_YARD_OPERATORS[i].op) {
                struct SYNode *top = peek_stack(expression->op_stack);
                if (top != NULL) {
                    while ((!is_stack_empty(expression->op_stack)) && ((SHUNT_YARD_OPERATORS[i].associativity == LEFT && SHUNT_YARD_OPERATORS[i].precedence <= top->precedence) || (SHUNT_YARD_OPERATORS[i].associativity == RIGHT && SHUNT_YARD_OPERATORS[i].precedence < top->precedence)) && top->op != LPAR) {
                        push_stack(expression->output_queue, top);
                        pop_stack(expression->op_stack);
                        top = peek_stack(expression->op_stack);
                    }
                }

                push_stack(expression->op_stack, (void *)&SHUNT_YARD_OPERATORS[i]);
                stop = false;
                break;
            }
        }

        if(stop == true)
            break;

        token_stoppage++;
    }

    struct SYNode *top = peek_stack(expression->op_stack);
    while (!is_stack_empty(expression->op_stack)) {
        push_stack(expression->output_queue, top);
        pop_stack(expression->op_stack);
        top = peek_stack(expression->op_stack);
    }

    /*
    for (size_t i = 0; i < expression->output_queue->top; i++)
    {
        struct SYNode *data = get_stack(expression->output_queue, i);
        printf("Output Queue Element %d, Value: %f, Operator: %d\n", i, data->value, data->op);
    }
    */
    

    parser->token_index += token_stoppage;
}

float calculate_expression(struct Expression* expression) {
    struct Stack *out = create_stack(sizeof(struct SYNode), STACK_SIZE);
    float result;

    if(expression->output_queue->top == 1) {
        struct SYNode * current = get_stack(expression->output_queue, 0);
        result = current->value;
    }

    for (size_t i = 0; i < expression->output_queue->top; i++) {
        struct SYNode * current = get_stack(expression->output_queue, i);
        if (current->op == INTEGER || current->op == FLOAT)
            push_stack(out, current);
        if (out->top >= 1 && current->op != FLOAT) {
            struct SYNode *operand1 = peek_stack(out);
            pop_stack(out);
            struct SYNode *operand2 = peek_stack(out);
            pop_stack(out);
            struct SYNode *node;
            node = malloc(sizeof(struct SYNode));

            switch (current->op) {
            case ADD: {
                node->value = operand2->value + operand1->value;
                break;
            }
            case SUBTRACT: {
                node->value = operand2->value - operand1->value;
                break;
            }
            case MULTIPLE: {
                node->value = operand2->value * operand1->value;
                break;
            }
            case DIVIDE: {
                node->value = operand2->value / operand1->value;
                break;
            }
            case TO_THE_POWER_OF: {
                node->value = powf(operand2->value, operand1->value);
                break;
            }
            case LESS_THAN: {
                node->value = (operand2->value < operand1->value) ? 1 : 0;
                break;
            }
            case GREATER_THAN: {
                node->value = (operand2->value > operand1->value) ? 1 : 0;
                break;
            }
            case LESS_THAN_EQUAL: {
                node->value = (operand2->value <= operand1->value) ? 1 : 0;
                break;
            }
            case GREATER_THAN_EQUAL: {
                node->value = (operand2->value >= operand1->value) ? 1 : 0;
                break;
            }
            default:
                break;
            }
            result = node->value;
            push_stack(out, node);
        }
    }
    destroy_stack(out);

    return result;
}

struct ASTNode* create_ast_node(enum TokenType op, struct ASTNode* left, struct ASTNode* right, float value) {
    struct ASTNode* node = NULL;

    node = malloc(sizeof(struct ASTNode));

    if(node == NULL)
        fatal_error("could not allocate 'ASTNode'");

    memset(node, 0, sizeof(struct ASTNode));

    node->left = left;
    node->right = right;
    node->op = op;
    node->value = value;

    return node;
}

struct ASTNode create_ast_node_from_expression(struct Expression* expression) {
    struct Stack *out = create_stack(sizeof(struct ASTNode), STACK_SIZE);
    struct SYNode *current;

    for (size_t i = 0; i < expression->output_queue->top; i++) {
        current = get_stack(expression->output_queue, i);
        if (current->op == INTEGER || current->op == FLOAT)
            push_stack(out, create_ast_node(FLOAT, NULL, NULL, current->value));
        if (out->top >= 1 && current->op != FLOAT) {
            struct ASTNode *operand1 = peek_stack(out);
            pop_stack(out);
            struct ASTNode *operand2 = peek_stack(out);
            pop_stack(out);
            struct ASTNode *node;

            node = create_ast_node(current->op, operand1, operand2, 0.0);

            push_stack(out, node);
        }
    }
    struct ASTNode* r = peek_stack(out);
    struct ASTNode root = *r;

    destroy_stack(out);

    return root;
}

void destroy_ast_node(struct ASTNode* root) {
    if(root == NULL)
        return;

    free(root);
    destroy_ast_node(root->left);
    destroy_ast_node(root->right);
}

void destroy_expression(struct Expression* expression) {
    destroy_stack(expression->op_stack);
    destroy_stack(expression->output_queue);
}

struct ASTNode* create_leaf_node(enum TokenType op, float value) {
    return create_ast_node(op, NULL, NULL, value);
}

struct ASTNode* create_unary(enum TokenType op, float value, struct ASTNode* left) {
    return create_ast_node(op, left, NULL, value);
}