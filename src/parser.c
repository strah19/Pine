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
#include "../include/opcodes.h"

#define PARSER_LOOP 1

struct Parser* create_parser(struct Lexer* lexer, struct ByteCodeBuilder* bc_builder) {
    struct Parser* parser;
    parser = malloc(sizeof(struct Parser));

    if (parser == NULL)
        fatal_error("could not allocate 'Parser'");

    memset(parser, 0, sizeof(struct Parser));
    parser->lexer = lexer;
    parser->bc_builder = bc_builder;
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
    case IF:
        if_statement(parser);
        break;
    case ELSE:
        fatal_token_error("Else without an if", token);
        break;
    case ELIF:
        fatal_token_error("Elif without an if", token);
        break;
    case WHILE:
        while_statement(parser);
        break;
    case VOID:  //This is currently not robust enough.
        func(parser);   
        break;
    default:
        fatal_token_error("Undefined token", token);
    }

    return true;
}

struct Token* peek_next_token(struct Parser* parser) {
    return (parser->token_index <= parser->lexer->size) ? &parser->lexer->tokens[parser->token_index] : NULL;
}

struct Token* retrieve_next_token(struct Parser* parser) {
    return (parser->token_index <= parser->lexer->size) ? &parser->lexer->tokens[parser->token_index++] : NULL;
}

void match_token(struct Parser* parser, enum TokenType type, const char* what) {
    struct Token* token = peek_next_token(parser);
    if (token) 
        if (token->type == type)
            retrieve_next_token(parser);
        else 
            fatal_compiler_error("Expected", what, token->token_info.token_line);
}

void expression_assignment(struct Parser* parser, struct ASTNode** root) {
    struct ASTNode* ast_tree;
    make_ast_from_expr(&ast_tree, parser);
    *root = ast_tree;
}

void print_statement(struct Parser* parser) {
    match_token(parser, PRINT, "print");

    struct Token* token = peek_next_token(parser);
    
    struct ASTNode* ast_tree;
    make_ast_from_expr(&ast_tree, parser);

    destroy_ast_node(ast_tree);

    match_token(parser, END_EXPRESSION, ";");
}

int check_for_var_redefination(struct Token* token) {
    if (search_type_symbol(token->token_string, VAR) == -1) 
        return add_symbol(token->token_string, VAR);
    else
        fatal_token_error("Redefination of variable", token);
    return -1;
}

void assignment_statement(struct Parser* parser) {
    struct ASTNode* assignment_ast = NULL;

    struct Token* token = peek_next_token(parser);
    match_token(parser, ID, "identifier");
    int var_id = 0;

    if (peek_next_token(parser)->type == COLON) {
        retrieve_next_token(parser); 
        var_id = check_for_var_redefination(token);
        if (peek_next_token(parser)->type == INT) {
            match_token(parser, INT, "int");
            if(peek_next_token(parser)->type == END_EXPRESSION) {
                assignment_ast = create_ast_node(EQUAL, NULL, NULL);
                assignment_ast->left = create_ast_node(ID, NULL, NULL);
                assignment_ast->left->var_id = var_id;
                assignment_ast->right = create_ast_node(INTEGER, NULL, NULL);
                assignment_ast->right->int_val = 0;

                assignment_ast->left->parent = assignment_ast;
                assignment_ast->right->parent = assignment_ast;

                if (get_sym_index() > parser->bc_builder->data_size)
                    parser->bc_builder->data_size++;
                build_decleration(parser->bc_builder, assignment_ast);

                match_token(parser, END_EXPRESSION, ";");
                destroy_ast_node(assignment_ast);

                return;
            }
        }
        if (get_sym_index() > parser->bc_builder->data_size)
            parser->bc_builder->data_size++;
    }

    if (search_type_symbol(token->token_string, VAR) == -1) 
            fatal_token_error("Undefined variable", token);

    struct Symbol* var = get_symbol(token->token_string, VAR);  
    parser->token_index = equal_statement(parser, parser->token_index, token, &assignment_ast);

    build_decleration(parser->bc_builder, assignment_ast);

    match_token(parser, END_EXPRESSION, ";");
    destroy_ast_node(assignment_ast);
}

int equal_statement(struct Parser* parser, int end_token, struct Token* var_token, struct ASTNode** root) {
    *root = create_ast_node(EQUAL, NULL, NULL);

    (*root)->left = create_ast_node(ID, NULL, NULL);
    (*root)->left->var_id = search_type_symbol(var_token->token_string, VAR);
    (*root)->left->parent = *root;

    if (var_token->type != ID)
        fatal_token_error("Value needs to be modifiable lvalue", var_token);
    else if ((*root)->left->var_id == -1)
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
        struct Token* next_var = &parser->lexer->tokens[parser->token_index - 1];
        struct Token* lval_check = &parser->lexer->tokens[parser->token_index - 2];
            if (next_var->type != ID && lval_check->type != EQUAL) 
                fatal_token_error("Value needs to be a modifiable lvalue", next_var);

        end_token = equal_statement(parser, end_token, next_var, &(*root)->right);
    }
    if (token->type == END_EXPRESSION) {
        parser->token_index = start_of_expression;
        expression_assignment(parser, &(*root)->right);
        (*root)->right->parent = *root;
   
        end_token = parser->token_index;
        return end_token;
    }

    return end_token;
}

void generic_condition(struct Parser* parser, uint32_t condition_type, const char* name) {
    match_token(parser, condition_type, name);

    struct ASTNode* ast_tree;
    make_ast_from_expr(&ast_tree, parser);

    build_expression(parser->bc_builder, ast_tree);
    match_token(parser, LCURLEY_BRACKET, "{");

    uint32_t jmp_reference = get_jmp_reference(parser->bc_builder, ast_tree);
    destroy_ast_node(ast_tree);
    int local_variable_frame = get_sym_index();

    uint32_t left_bracket_match = parser->token_index;
    while(peek_next_token(parser)->type != RCURLEY_BRACKET && peek_next_token(parser)->type != T_EOF) 
        run_statements(parser);
    if (peek_next_token(parser)->type != RCURLEY_BRACKET) 
        fatal_token_error("Mismatched { token", &parser->lexer->tokens[left_bracket_match]);

    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = JMP;
    uint32_t main_ref = parser->bc_builder->current_builder_location;
    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = -1;

    match_token(parser, RCURLEY_BRACKET, "}");

    update_sym_index(local_variable_frame);

    parser->bc_builder->opcodes[jmp_reference] = parser->bc_builder->current_builder_location;

    switch (peek_next_token(parser)->type) {
        case ELIF:
        elif_statement(parser);
        break;
        case ELSE:
        else_statement(parser);
        break;
    }
    parser->bc_builder->opcodes[main_ref] = parser->bc_builder->current_builder_location;
}

void if_statement(struct Parser* parser) {
    generic_condition(parser, IF, "if");
}

void else_statement(struct Parser* parser) {
    match_token(parser, ELSE, "else");
    match_token(parser, LCURLEY_BRACKET, "{");
    int local_variable_frame = get_sym_index();

    uint32_t left_bracket_match = parser->token_index;
    while(peek_next_token(parser)->type != RCURLEY_BRACKET && peek_next_token(parser)->type != T_EOF) 
        run_statements(parser);
    if (peek_next_token(parser)->type != RCURLEY_BRACKET) 
        fatal_token_error("Mismatched { token", &parser->lexer->tokens[left_bracket_match]);
    match_token(parser, RCURLEY_BRACKET, "}");

    update_sym_index(local_variable_frame);
  
}

void elif_statement(struct Parser* parser) {
    generic_condition(parser, ELIF, "elif");
}

void while_statement(struct Parser* parser) {
    match_token(parser, WHILE, "while");
 
    uint32_t start_ref = parser->bc_builder->current_builder_location;
    struct ASTNode* ast_tree;
    make_ast_from_expr(&ast_tree, parser);

    build_expression(parser->bc_builder, ast_tree);
    match_token(parser, LCURLEY_BRACKET, "{");

    uint32_t jmp_reference = get_jmp_reference(parser->bc_builder, ast_tree);
    destroy_ast_node(ast_tree);
    int local_variable_frame = get_sym_index();

    uint32_t left_bracket_match = parser->token_index;

    while(peek_next_token(parser)->type != RCURLEY_BRACKET && peek_next_token(parser)->type != T_EOF) {
        run_statements(parser);
        break_statement(parser);
    }
    if (peek_next_token(parser)->type != RCURLEY_BRACKET) 
        fatal_token_error("Mismatched { token", &parser->lexer->tokens[left_bracket_match]);

    match_token(parser, RCURLEY_BRACKET, "}");

    update_sym_index(local_variable_frame);

    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = JMP;
    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = start_ref;

    parser->bc_builder->opcodes[jmp_reference] = parser->bc_builder->current_builder_location;
}

void break_statement(struct Parser* parser) {

}

void func(struct Parser* parser) {
    match_token(parser, VOID, "void");

    struct Token* token_symbol = peek_next_token(parser);
    match_token(parser, ID, "function identifier");
  //  struct Symbol* symbol = add(token_symbol->token_string);
    
    //if (!symbol) 
    //    add_symbol(token_symbol->token_string, 0.0, FUNC);
    //else
     //   fatal_token_error("Redefination of function", token_symbol);

    match_token(parser, LPAR, "(");
    match_token(parser, RPAR, ")");

    match_token(parser, LCURLEY_BRACKET, "{");
    match_token(parser, RCURLEY_BRACKET, "}");

    log_symbols();
}