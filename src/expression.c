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

void log_tree(struct ASTNode* root, uint32_t tree_branch);

struct ASTNode* create_ast_node(enum TokenType op, struct ASTNode* left, struct ASTNode* right) {
    return create_ast_node_fill(op, left, right, NULL, T_NONE);
}

struct ASTNode* create_ast_node_fill(enum TokenType op, struct ASTNode* left, struct ASTNode* right, struct ASTNode* parent, enum TokenType type) {
    struct ASTNode* node = NULL;

    node = malloc(sizeof(struct ASTNode));

    if (node == NULL)
        fatal_error("could not allocate 'ASTNode'");

    node->left = left;
    node->right = right;
    node->parent = parent;

    node->op = op;
    node->precedence = 0;
    node->var_id = 0;
    node->int_val = 0;
    node->order = 0;
    node->type = type;

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

    printf("\tNode: %d, Op: %d, Type: %d, Branch: %s, Parent: %d\n", root->int_val, root->op, root->type, (tree_branch == LEFT_NODE) ? "LEFT" : "RIGHT", (root->parent) ? true : false);

    log_tree(root->left, LEFT_NODE);
    log_tree(root->right, RIGHT_NODE);
}

void log_ast(struct ASTNode* root) {
    printf("AST:\n");
    log_tree(root, LEFT_NODE);
}

int fig_precedence_from_op(enum TokenType op) {
    switch(op) {
        case T_PLUS: return 6;
        case T_MINUS: return 6;
        case T_STAR: return 7;
        case T_SLASH: return 7;
        case T_LPAR: return 1;
        case T_RPAR: return 1;
        case T_DOUBLE_EQUAL: return 5;
        case T_OR: return 2;
        case T_AND: return 3;
        case T_LESS_THAN: return 5;
        case T_GREATER_THAN: return 5;
        case T_LESS_THAN_EQUAL: return 5;
        case T_GREATER_THAN_EQUAL: return 5;
        case T_NOT: return 4;
    }

    return -1;
}

void fill_ast_node(struct Token* token, struct ASTNode** node) {
    if (token->type == T_INTEGER) {
        (*node)->int_val = atoi(token->token_string);
        (*node)->precedence = NUMERIC_PRECEDENCE;
        (*node)->type = T_INTEGER;
    }
    else if (token->type == T_FLOAT) {
        char *pend;
        (*node)->int_val = (int) strtof(token->token_string, &pend);
        (*node)->precedence = NUMERIC_PRECEDENCE;
        (*node)->type = T_FLOAT;
    }
    else if (token->type == T_STR) {
        if (token->token_string[1] == '\0') {
            (*node)->char_val = token->token_string[0];
            (*node)->precedence = NUMERIC_PRECEDENCE;
            (*node)->type = T_CHAR;
            (*node)->op = T_CHAR;
        }
        else 
            (*node)->type = T_STR;
    }
    else if(token->type == T_ID) {
        int var_id = search_type_symbol(token->token_string, VAR);

        if (check_id_arg(var_id)) {
            (*node)->var_id = get_function()->arg_info[get_arg_id(var_id)].var_info.id;
            (*node)->type = get_function()->arg_info[get_arg_id(var_id)].var_info.value_type;
        }
        else {
            (*node)->var_id = get_symbols()[var_id].var.id;
            (*node)->type = get_symbols()[var_id].var.value_type;
        }
        (*node)->precedence = NUMERIC_PRECEDENCE;
    }
    else {
        (*node)->precedence = fig_precedence_from_op(token->type);
        if ((*node)->precedence == -1) {
            fatal_token_error("Unknown value in expression", token);
        }
    }
}

void make_ast_from_expr(struct ASTNode** root, struct Parser* parser) {
    int token_stoppage = 0;
    *root = create_ast_node(T_LPAR, NULL, NULL);

    struct ASTNode* current = *root;
    struct ASTNode* new_item = NULL;

    for (size_t i = parser->token_index; i < parser->lexer->size; i++) {
        struct Token *token = &parser->lexer->tokens[i];

        //This is BAD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        if(token->type == T_SEMI_COLON || token->type == T_LCURLEY_BRACKET || token->type == T_RPAR || token->type == T_COMMA)
            break;

        new_item = create_ast_node(token->type, NULL, NULL);
        new_item->order = token->token_info.token_pos * token->token_info.token_line;
        fill_ast_node(token, &new_item);

        if (new_item->op == T_EQUAL)
            fatal_error("Equal operator can not be evaluated in an expression");

        if (new_item->op != T_LPAR)
            if (new_item->op == T_RPAR)
                while(current->precedence > new_item->precedence && current->parent != NULL)
				    current = current->parent;
            else 
                while (current->precedence >= new_item->precedence && current->parent != NULL) 
                    current = current->parent;

        if (new_item->op == T_RPAR) {
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
    *root = (*root)->right;
    
    parser->token_index += token_stoppage;
}