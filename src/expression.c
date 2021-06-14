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
#include "../include/parser.h"

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
            node.order = token->token_info.token_pos * token->token_info.token_line;
            push_stack(expression->output_queue, (void*) &node);  
        }
        else if (token->type == FLOAT) {
            char *pend;
            struct SYNode node;
            node.value = strtof(token->token_string, &pend);
            node.op = token->type;
            node.order = token->token_info.token_pos * token->token_info.token_line;
            push_stack(expression->output_queue, (void*) &node);  
        }
        else if(token->type == ID) {
            struct SYNode node;
            struct Symbol* sym = get_global_symbol(token->token_string);

            if (sym == NULL)
                fatal_token_error("Undefined variable", token);
            
            node.value = sym->value;
            node.op = FLOAT;
            node.order = token->token_info.token_pos * token->token_info.token_line;
            push_stack(expression->output_queue, (void*) &node);  
        }
        else if (token->type == LPAR) {
            struct SYNode node;
            node.value = 0;
            node.op = LPAR;
            node.order = token->token_info.token_pos * token->token_info.token_line;
            push_stack(expression->output_queue, (void*) &node);  
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
                struct SYNode op = SHUNT_YARD_OPERATORS[i];
                op.order = token->token_info.token_pos * token->token_info.token_line;
                if (top != NULL) {
                    while ((!is_stack_empty(expression->op_stack)) && ((SHUNT_YARD_OPERATORS[i].associativity == LEFT && SHUNT_YARD_OPERATORS[i].precedence <= top->precedence) || (SHUNT_YARD_OPERATORS[i].associativity == RIGHT && SHUNT_YARD_OPERATORS[i].precedence < top->precedence)) && top->op != LPAR) {
                        push_stack(expression->output_queue, top);
                        pop_stack(expression->op_stack);
                        top = peek_stack(expression->op_stack);
                    }
                }

                push_stack(expression->op_stack, (void*)&op);
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
    printf("Expression output~------------------------------------------------~\n");
    for (size_t i = 0; i < expression->output_queue->top; i++) {
        struct SYNode *data = get_stack(expression->output_queue, i);
        printf("Output queue element %d, value: %f, operator: %d, rel pos: %d\n", i, data->value, data->op, data->order);
    }
    */

    parser->token_index += token_stoppage;
}

float calculate_expression(struct Expression* expression) {
    struct Stack *out = create_stack(sizeof(struct SYNode), STACK_SIZE);
    float result;

    if(expression->output_queue->top == 1) {
        struct SYNode *current = get_stack(expression->output_queue, 0);
        result = current->value;
    }

    for (size_t i = 0; i < expression->output_queue->top; i++) {
        struct SYNode *current = get_stack(expression->output_queue, i);
        if (current->op == INTEGER || current->op == FLOAT)
            push_stack(out, current);
        else {
            struct SYNode *operand1 = peek_stack(out);
            pop_stack(out);
            struct SYNode *operand2 = peek_stack(out);
            pop_stack(out);
            struct SYNode node;

            switch (current->op) {
            case ADD: {
                node.value = operand2->value + operand1->value;
                break;
            }
            case SUBTRACT: {
                node.value = operand2->value - operand1->value;
                break;
            }
            case MULTIPLE: {
                node.value = operand2->value * operand1->value;
                break;
            }
            case DIVIDE: {
                node.value = operand2->value / operand1->value;
                break;
            }
            case TO_THE_POWER_OF: {
                node.value = powf(operand2->value, operand1->value);
                break;
            }
            case LESS_THAN: {
                if(operand1->order < operand2->order) 
                    node.value = (operand1->value < operand2->value) ? 1 : 0;
                else
                    node.value = (operand2->value < operand1->value) ? 1 : 0;
                break;
            }
            case GREATER_THAN: {
                if(operand1->order < operand2->order) 
                    node.value = (operand1->value > operand2->value) ? 1 : 0;
                else
                    node.value = (operand2->value > operand1->value) ? 1 : 0;
                break;
            }
            case LESS_THAN_EQUAL: {
                if(operand1->order < operand2->order) 
                    node.value = (operand1->value <= operand2->value) ? 1 : 0;
                else
                    node.value = (operand2->value <= operand1->value) ? 1 : 0;
                break;
            }
            case GREATER_THAN_EQUAL: {
                 if(operand1->order < operand2->order) 
                    node.value = (operand1->value >= operand2->value) ? 1 : 0;
                else
                    node.value = (operand2->value >= operand1->value) ? 1 : 0;
                break;
            }
            case DOUBLE_EQUAL: {
                node.value = (operand2->value == operand1->value) ? 1 : 0;
                break;
            }
            case AND: {
                node.value = (operand1->value != 0 && operand2->value != 0) ? 1 : 0;
                break;
            }
            case OR: {
                node.value = (operand1->value || operand2->value) ? 1 : 0;
                break;
            }
            default:
                break;
            }
            result = node.value;
            node.order = operand1->order;
            push_stack(out, (void*) &node);
        }
    }
    destroy_stack(out);

    return result;
}

struct ASTNode* create_ast_node(enum TokenType op, struct ASTNode* left, struct ASTNode* right) {
    struct ASTNode* node = NULL;

    node = malloc(sizeof(struct ASTNode));

    if(node == NULL)
        fatal_error("could not allocate 'ASTNode'");

    node->left = left;
    node->right = right;
    node->parent = NULL;

    node->op = op;
    node->precedence = 0;
    node->value.var_id = 0;
    node->value.int_val = 0;

    return node;
}

struct ASTNode create_ast_node_from_expression(struct Expression* expression) {
    struct Stack *out = create_stack(sizeof(struct ASTNode), STACK_SIZE);
    struct SYNode *current;

    for (size_t i = 0; i < expression->output_queue->top; i++) {
        current = get_stack(expression->output_queue, i);
        if (current->op == INTEGER || current->op == FLOAT) {
            struct ASTNode* node = create_ast_node(FLOAT, NULL, NULL);
            node->value.int_val = (int) current->value;
            push_stack(out, node);
        }

        if (out->top >= 1 && current->op != FLOAT) {
            struct ASTNode *operand1 = peek_stack(out);
            pop_stack(out);
            struct ASTNode *operand2 = peek_stack(out);
            pop_stack(out);
            struct ASTNode *node;

            node = create_ast_node(current->op, operand1, operand2);
            node->value.int_val = 0;

            push_stack(out, node);
        }
    }
    struct ASTNode* r = peek_stack(out);
    struct ASTNode root = *r;

    destroy_stack(out);

    return root;
}

void destroy_ast_node(struct ASTNode* root) {
    free(root);
}

void destroy_expression(struct Expression* expression) {
    destroy_stack(expression->op_stack);
    destroy_stack(expression->output_queue);
}

struct ASTNode* create_leaf_node(enum TokenType op) {
    return create_ast_node(op, NULL, NULL);
}

struct ASTNode* create_unary(enum TokenType op, struct ASTNode* left) {
    return create_ast_node(op, left, NULL);
}

void log_tree(struct ASTNode* root) {
    if (root == NULL)
        return;

    printf("Node: %d, Op: %d\n", root->value, root->op);

    log_tree(root->left);
    log_tree(root->right);
}

int fig_precedence_from_op(enum TokenType op) {
    switch(op) {
    case ADD:
        return 2;
    case SUBTRACT:
        return 2;
    case MULTIPLE:
        return 4;
    case DIVIDE:
        return 4;
    case LPAR: 
        return 1;
    case RPAR:
        return 1;
    }

    return -1;
}

void fill_ast_node(struct Token* token, struct ASTNode** node) {
    if (token->type == INTEGER) {
        (*node)->value.int_val = atoi(token->token_string);
        (*node)->precedence = 10;
    }
    else if (token->type == FLOAT) {
        char *pend;
        (*node)->value.int_val = (int) strtof(token->token_string, &pend);
        (*node)->precedence = 10;
    }
    else if(token->type == ID) {
        int var_id = find_global_symbol(token->token_string);
        if (var_id == -1)
            fatal_token_error("Undefined variable", token);
        (*node)->value.var_id = var_id;
        (*node)->precedence = 10;
    }
    else {
        (*node)->value.int_val = 0;
        (*node)->precedence = fig_precedence_from_op(token->type);
    }
}

void make_ast_from_expr(struct ASTNode** root, struct Parser* parser) {
    int token_stoppage = 0;
    *root = create_ast_node(LPAR, NULL, NULL);

    struct ASTNode* current = *root;
    struct ASTNode* new_item = NULL;

    for (size_t i = parser->token_index; i < parser->lexer->tokens->size; i++) {
        struct Token *token = parser->lexer->tokens->array[i];

        if(token->type == END_EXPRESSION) 
            break;

        new_item = create_ast_node(token->type, NULL, NULL);
        fill_ast_node(token, &new_item);

        if (new_item->op != LPAR) 
            if (new_item->op == RPAR) 
                while(current->precedence > new_item->precedence && current->parent != NULL)
				    current = current->parent;
            else 
                while (current->precedence >= new_item->precedence && current->parent != NULL) 
                    current = current->parent;

        if (new_item->op == RPAR) {
            new_item = current->parent;
            new_item->right = current->right;
            if(current->right) current->right->parent = new_item;

            memset(current, 0, sizeof(struct ASTNode));
            free(current);

            current = new_item;
        }
        else {
            new_item->left = current->right;
            if (current->right) 
            current->right->parent = new_item;
            current->right = new_item;
            new_item->parent = current;

            current = new_item;
        }
        
        token_stoppage++;
    }
    *root = (*root)->right; //The AST new value will always fill into the right first
    log_tree((*root));

    parser->token_index += token_stoppage;
}

bool is_node_childern_operands(struct ASTNode* node) {
    return ((node->left->op == INTEGER || node->left->op == ID) && (node->right->op == INTEGER || node->right->op == ID));
}

int run_bin_exp(struct ASTNode* node) {
    int left_val = 0, right_val = 0;

    if (node->left->op == ID) 
        left_val = (int) get_symbols()[node->left->value.var_id].value;
    else
        left_val = node->left->value.int_val;
   if (node->right->op == ID) 
        right_val = (int) get_symbols()[node->right->value.var_id].value;
    else
        right_val = node->right->value.int_val;

    switch(node->op) {
    case ADD:
        return left_val + right_val;
    case SUBTRACT:
        return left_val - right_val;
    case MULTIPLE:
        return left_val * right_val;
    case DIVIDE:
        return left_val / right_val;
    }

    return 0;
}

struct ASTNode* run_ast_tree(struct ASTNode* root) {
    if (root == NULL || root->left == NULL || root->right == NULL) {
        if (root->op == ID)
            root->value.int_val = (int) get_symbols()[root->value.var_id].value;
        return root;
    }
    
    if(is_node_childern_operands(root)) {
        root->value.int_val = run_bin_exp(root);
        return root;
    }
    else {
        root->left = run_ast_tree(root->left);
        root->right = run_ast_tree(root->right); 
    }
    root->value.int_val = run_bin_exp(root);
    return root;
}