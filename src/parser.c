/**
 * @file parser.c
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
 * This file contains the main code for parsing our language. It 
 * specifies the grammer and handles the generation of AST trees.
 */

#include "../include/parser.h"
#include "../include/stack.h"
#include "../include/sym.h"
#include "../include/err.h"

#define PARSER_LOOP 1

struct Parser* create_parser(struct Lexer* lexer) {
    struct Parser* parser;
    parser = malloc(sizeof(struct Parser));

    if (parser == NULL)
        fatal_error("could not allocate 'Parser'");

    memset(parser, 0, sizeof(struct Parser));
    parser->lexer = lexer;
    parser->token_index = 0;

    return parser;
}

void destroy_parser(struct Parser* parser) {
    free(parser);
}

void run_parser(struct Parser* parser) {
    while (PARSER_LOOP) {
        if(!run_statements(parser))
            break;
    }
}

bool run_statements(struct Parser* parser) {
    struct Token* token = peek_next_token(parser);
    
    if (token->type == T_EOF)
        return false;
    switch (token->type) {
    case PRINT: //temporary, till Pine supports functions.
        print_statement(parser);
        break;
    case ID:
        assignment_statement(parser);
        break;
    default:
        fatal_token_error("Undefined token", token);
    }

    return true;
}

struct Token* peek_next_token(struct Parser* parser) {
    return (parser->token_index < parser->lexer->tokens->size) ? parser->lexer->tokens->array[parser->token_index] : NULL;
}

struct Token* retrieve_next_token(struct Parser* parser) {
    return (parser->token_index < parser->lexer->tokens->size) ? parser->lexer->tokens->array[parser->token_index++] : NULL;
}

void match_token(struct Parser* parser, enum TokenType type, const char* what) {
    struct Token* token = peek_next_token(parser);
    if (token) 
        if (token->type == type)
            retrieve_next_token(parser);
        else 
            fatal_compiler_error("Expected", what, token->token_info.token_line);
}

void expression_assignment(struct Parser* parser, struct Token* var, struct ASTNode** root) {
    struct ASTNode* ast_tree;
    make_ast_from_expr(&ast_tree, parser);

    struct Symbol* variable = get_global_symbol(var->token_string);
    if (variable == NULL) {
        ast_tree = run_ast_tree(ast_tree);
        add_symbol(var->token_string, ast_tree->value.int_val);
    }
    else {
        ast_tree = run_ast_tree(ast_tree);
        variable->value = ast_tree->value.int_val;

        *root = ast_tree;
    }
    destroy_ast_node(ast_tree);
}

void print_statement(struct Parser* parser) {
    match_token(parser, PRINT, "print");

    struct Token* token = peek_next_token(parser);
    
    struct ASTNode* ast_tree;
    make_ast_from_expr(&ast_tree, parser);
    printf("%d\n", run_ast_tree(ast_tree)->value);
    destroy_ast_node(ast_tree);

    match_token(parser, END_EXPRESSION, ";");
}

void assignment_statement(struct Parser* parser) {
    struct ASTNode* root_ast = NULL;

    struct Token* token = peek_next_token(parser);
    match_token(parser, ID, "identifier");
    int var_id = 0;

    if (peek_next_token(parser)->type == COLON) {
        retrieve_next_token(parser);
        match_token(parser, INT, "int");
        if (find_global_symbol(token->token_string) == -1) 
            var_id = add_symbol(token->token_string, 0.0);
        else
            fatal_token_error("Redefination of variable", token);
        if(peek_next_token(parser)->type == END_EXPRESSION) {
            match_token(parser, END_EXPRESSION, ";");
            root_ast = create_ast_node(EQUAL, NULL, NULL);
            root_ast->left = create_ast_node(ID, NULL, NULL);
            root_ast->left->value.var_id = var_id;
            root_ast->right = create_ast_node(INTEGER, NULL, NULL);
            root_ast->right->value.int_val = 0;
            destroy_ast_node(root_ast);
            return;
        }
    }
    else {
        if (find_global_symbol(token->token_string) == -1) 
            var_id = add_symbol(token->token_string, 0.0);
        else 
            var_id = find_global_symbol(token->token_string);
    }

    struct Symbol* var = get_global_symbol(token->token_string);  
    parser->token_index = equal_statement(parser, parser->token_index, token, &root_ast);
    
    match_token(parser, END_EXPRESSION, ";");
    destroy_ast_node(root_ast);
}

int equal_statement(struct Parser* parser, int end_token, struct Token* var_token, struct ASTNode** root) {
    *root = create_ast_node(EQUAL, NULL, NULL);

    (*root)->left = create_ast_node(ID, NULL, NULL);
    (*root)->left->value.var_id = find_global_symbol(var_token->token_string);

    if (var_token->type != ID)
        fatal_token_error("Value needs to be modifiable lvalue", var_token);
    else if ((*root)->left->value.var_id == -1)
        fatal_token_error("Undefined variable", var_token);

    match_token(parser, EQUAL, "=");

    struct Token* token = peek_next_token(parser);

    int start_of_expression = parser->token_index;

    while (1) {
        if (token->type == EQUAL || token->type == END_EXPRESSION)
            break;
        token = retrieve_next_token(parser);
    }
    parser->token_index--;

    if (token->type == EQUAL) {
        struct Token* next_var = parser->lexer->tokens->array[parser->token_index - 1];
        struct Token* lval_check = parser->lexer->tokens->array[parser->token_index - 2];
            if (next_var->type != ID && lval_check->type != EQUAL) 
                fatal_token_error("Value needs to be a modifiable lvalue", next_var);

        end_token = equal_statement(parser, end_token, next_var, &(*root)->right);
    }
    if (token->type == END_EXPRESSION)
    {
        parser->token_index = start_of_expression;
        expression_assignment(parser, var_token, &(*root)->right);
   
        end_token = parser->token_index;
        return end_token;
    }
    parser->token_index = start_of_expression;

    expression_assignment(parser, var_token, NULL);

    return end_token;
}

void if_statement(struct Parser* parser) {
    match_token(parser, IF, "if");

    struct ASTNode* ast_tree;
    make_ast_from_expr(&ast_tree, parser);

    ast_tree = run_ast_tree(ast_tree);
    int val = ast_tree->value.int_val;
    destroy_ast_node(ast_tree);

    match_token(parser, LCURLEY_BRACKET, "{");

    if(val) {
        while(peek_next_token(parser)->type != RCURLEY_BRACKET)
            run_statements(parser);
    }
    else {
        while(peek_next_token(parser)->type != RCURLEY_BRACKET)
            retrieve_next_token(parser);
    }
    match_token(parser, RCURLEY_BRACKET, "}");
}

void else_statement(struct Parser* parser) {
    
}

void elif_statement(struct Parser* parser) {

}