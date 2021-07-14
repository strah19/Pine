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
 * This file contains code for AST nodes and functions to create
 * an AST tree out of an expression.
 */

#include "../include/expression.h"
#include "../include/sym.h"
#include "../include/stack.h"
#include "../include/err.h"
#include "../include/parser.h"

#include <math.h>

#define NUMERIC_PRECEDENCE 10

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
    node->var_id = 0;
    node->int_val = 0;
    node->order = 0;

    return node;
}

void destroy_ast_node(struct ASTNode* root) {
	if(!root) return;
	destroy_ast_node (root->left);
	destroy_ast_node (root->right);
	memset(root, 0, sizeof(struct ASTNode));
	free(root);
}

struct ASTNode* create_leaf_node(enum TokenType op) {
    return create_ast_node(op, NULL, NULL);
}

void log_tree(struct ASTNode* root, uint32_t tree_branch) {
    if (root == NULL)
        return;

    printf("Node: %d, Op: %d, Branch: %s\n", root->int_val, root->op, (tree_branch == LEFT_NODE) ? "LEFT" : "RIGHT");

    log_tree(root->left, LEFT_NODE);
    log_tree(root->right, RIGHT_NODE);
}

void log_ast(struct ASTNode* root) {
    printf("AST:\n");
    log_tree(root, LEFT_NODE);
}

int fig_precedence_from_op(enum TokenType op) {
    switch(op) {
    case ADD:
        return 6;
    case SUBTRACT:
        return 6;
    case MULTIPLE:
        return 7;
    case DIVIDE:
        return 7;
    case LPAR: 
        return 1;
    case RPAR:
        return 1;
    case DOUBLE_EQUAL:
        return 5;
    case OR:
        return 2;
    case AND:
        return 3;
    case LESS_THAN:
        return 5;
    case GREATER_THAN:
        return 5;
    case LESS_THAN_EQUAL:
        return 5;
    case GREATER_THAN_EQUAL:
        return 5;
    case NOT:
        return 4;
    }

    return -1;
}

void fill_ast_node(struct Token* token, struct ASTNode** node) {
    if (token->type == INTEGER) {
        (*node)->int_val = atoi(token->token_string);
        (*node)->precedence = NUMERIC_PRECEDENCE;
    }
    else if (token->type == FLOAT) {
        char *pend;
        (*node)->int_val = (int) strtof(token->token_string, &pend);
        (*node)->precedence = NUMERIC_PRECEDENCE;
    }
    else if (token->type == STR) {
        if (token->token_string[1] == '\0') {
            (*node)->char_val = token->token_string[0];
            (*node)->precedence = NUMERIC_PRECEDENCE;
        }
    }
    else if(token->type == ID) {
        int var_id = search_type_symbol(token->token_string, VAR);
        if (var_id == -1)
            fatal_token_error("Undefined variable", token);
        (*node)->var_id = var_id;
        (*node)->precedence = NUMERIC_PRECEDENCE;
    }
    else {
        (*node)->int_val = 0;
        (*node)->precedence = fig_precedence_from_op(token->type);
    }
}

void make_ast_from_expr(struct ASTNode** root, struct Parser* parser) {
    int token_stoppage = 0;
    *root = create_ast_node(LPAR, NULL, NULL);

    struct ASTNode* current = *root;
    struct ASTNode* new_item = NULL;

    for (size_t i = parser->token_index; i < parser->lexer->size; i++) {
        struct Token *token = &parser->lexer->tokens[i];

        if(token->type == END_EXPRESSION || token->type == LCURLEY_BRACKET) 
            break;

        new_item = create_ast_node(token->type, NULL, NULL);
        new_item->order = token->token_info.token_pos * token->token_info.token_line;
        fill_ast_node(token, &new_item);

        if (new_item->op == EQUAL)
            fatal_error("Equal operator can not be evaluated in an expression");

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
    
    parser->token_index += token_stoppage;
}

/*
* @TODO: Update 'run_bin_exp' for new type system. This is where we would check for any conversion info in the AST node and handle it, 
* the parser would handle the actual syntax while what needs to be converted will be held in the nodes for further use down the pipeline.
* 
*/

int run_bin_exp(struct ASTNode* node) {
    int left_val = 0, right_val = 0;

    left_val = node->left->int_val;
    right_val = node->right->int_val;

    switch(node->op) {
    case ADD:
        return left_val + right_val;
    case SUBTRACT:
        return left_val - right_val;
    case MULTIPLE:
        return left_val * right_val;
    case DIVIDE:
        return left_val / right_val;
    case DOUBLE_EQUAL:
        return (left_val == right_val);
    case OR:
        return (left_val || right_val);
    case AND:
        return (left_val && right_val);
    case LESS_THAN:
        return (left_val < right_val);
    case GREATER_THAN:
        return (left_val > right_val);
    case LESS_THAN_EQUAL:
        return (left_val <= right_val);
    case GREATER_THAN_EQUAL:
        return (left_val >= right_val);
    case NOT:
        return (left_val != right_val);
    }

    return 0;
}