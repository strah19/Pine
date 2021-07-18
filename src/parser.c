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
#include "../include/sem.h"

#define PARSER_LOOP 1

void run_scope(struct Parser* parser);
bool run_statements(struct Parser* parser);

void expression_assignment(struct Parser* parser, struct ASTNode** root);
uint32_t comparison_statement(struct Parser* parser);
void print_statement(struct Parser* parser);
void assignment_statement(struct Parser* parser);
void id_found(struct Parser* parser);

void if_statement(struct Parser* parser);
void else_statement(struct Parser* parser);
void elif_statement(struct Parser* parser);
int equal_statement(struct Parser* parser, int end_token, struct Token* var_token, struct ASTNode** root);
void while_statement(struct Parser* parser);

void function_definition(struct Parser* parser);

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
    case PRINT:
        print_statement(parser);
        break;
    case ID:
        id_found(parser);
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

struct Token* peek_offset_token(struct Parser* parser, int offset) {
    return (parser->token_index + offset <= parser->lexer->size) ? &parser->lexer->tokens[parser->token_index + offset] : NULL;
}

void match_token(struct Parser* parser, enum TokenType type, const char* what) {
    struct Token* token = peek_next_token(parser);
    if (token) 
        if (token->type == type)
            retrieve_next_token(parser);
        else 
            fatal_compiler_error("Expected", what, token->token_info.token_line);
}

void id_found(struct Parser* parser) {
    if (peek_offset_token(parser, 1)->type == LPAR)
        function_definition(parser);
    else
        assignment_statement(parser);
}

void expression_assignment(struct Parser* parser, struct ASTNode** root) {
    struct ASTNode* ast_tree;
    make_ast_from_expr(&ast_tree, parser);
    *root = ast_tree;
}

//Temporary: will create an actually interface for the system calls.
void print_statement(struct Parser* parser) {
    match_token(parser, PRINT, "print");
    
    struct ASTNode* ast;
    make_ast_from_expr(&ast, parser);

    build_print(parser->bc_builder, ast);

    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = SYS_WRITE;

    destroy_ast_node(ast);

    match_token(parser, END_EXPRESSION, ";");
}

int check_for_var_redefination(struct Token* token) {
    if (search_type_symbol(token->token_string, VAR) == -1) 
        return add_symbol(token->token_string, VAR)->index;
    else
        fatal_token_error("Redefinition of variable", token);
    return -1;
}

void fill_var_sym_data(int var_id, struct VariableType* var_type) {
    get_symbols()[var_id].var.type = var_type->type;
    get_symbols()[var_id].var.size = var_type->size;
    get_symbols()[var_id].var.value_type = var_type->value_type;
}

void assignment_statement(struct Parser* parser) {
    struct ASTNode* ast = NULL;

    struct Token* var_token = peek_next_token(parser);
    match_token(parser, ID, "identifier");

    int var_id = -1;
    bool is_const = false;

    if (peek_next_token(parser)->type == COLON) {
        match_token(parser, peek_next_token(parser)->type, ":");

        if (peek_next_token(parser)->type == CONST) {
            is_const = true;
            match_token(parser, CONST, "const");
        }
        struct VariableType* var_type = get_variable_types(peek_next_token(parser)->type);
        retrieve_next_token(parser);

        var_id = check_for_var_redefination(var_token);

        fill_var_sym_data(var_id, var_type);

        if (get_sym_index() > parser->bc_builder->data_size)
            parser->bc_builder->data_size++;

        if (peek_next_token(parser)->type == END_EXPRESSION) {
            if (is_const) 
                fatal_token_error("Need to give const a value", peek_next_token(parser));

            match_token(parser, peek_next_token(parser)->type, ";");

            ast = create_ast_node(EQUAL, create_ast_node_fill(ID, NULL, NULL, ast, var_type->value_type), create_ast_node_fill(var_type->value_type, NULL, NULL, ast, var_type->value_type));
            ast->left->var_id = get_symbols()[var_id].var.id;

            build_decleration(parser->bc_builder, ast);
            destroy_ast_node(ast);
            return;
        }
    }
    else if (search_type_symbol(var_token->token_string, VAR) == -1)
        fatal_token_error("Undefined variable", var_token);

    parser->token_index = equal_statement(parser, parser->token_index, var_token, &ast);
   

    if (is_const) {
        get_symbols()[var_id].var.is_const = is_const;
    }

    match_token(parser, END_EXPRESSION, ";");

    begin_sem();
    validate_ast(ast);

    log_ast(ast);
    build_assignment(parser->bc_builder, ast);
    destroy_ast_node(ast);
}

int equal_statement(struct Parser* parser, int end_token, struct Token* var_token, struct ASTNode** root) {
    *root = create_ast_node(EQUAL, NULL, NULL);

    (*root)->left = create_ast_node_fill(ID, NULL, NULL, *root, get_symbols()[search_type_symbol(var_token->token_string, VAR)].var.value_type);
    int id = search_type_symbol(var_token->token_string, VAR);
    if (id == -1)
        fatal_token_error("Undefined variable", var_token);
    (*root)->left->var_id = get_symbols()[id].var.id;

    if (var_token->type != ID)
        fatal_token_error("Value needs to be modifiable lvalue", var_token);
    else if (get_symbols()[search_type_symbol(var_token->token_string, VAR)].var.is_const)
        fatal_token_error("Value cannot be a const", var_token);

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
        (*root)->right->parent = *root;
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

void run_scope(struct Parser* parser) {
    match_token(parser, LCURLEY_BRACKET, "{");
    uint32_t left_bracket_match = parser->token_index;
    while(peek_next_token(parser)->type != RCURLEY_BRACKET && peek_next_token(parser)->type != T_EOF) 
        run_statements(parser);
    if (peek_next_token(parser)->type != RCURLEY_BRACKET) 
        fatal_token_error("Mismatched { token", &parser->lexer->tokens[left_bracket_match]);    
    match_token(parser, RCURLEY_BRACKET, "}");
}

uint32_t comparison_statement(struct Parser* parser) {
    struct ASTNode* ast_tree;
    make_ast_from_expr(&ast_tree, parser);

    build_expression(parser->bc_builder, ast_tree);

    uint32_t jmp_reference = get_jmp_reference(parser->bc_builder);
    destroy_ast_node(ast_tree);    

    return jmp_reference;
}

void generic_condition(struct Parser* parser, uint32_t condition_type, const char* name) {
    match_token(parser, condition_type, name);

    uint32_t jmp_reference = comparison_statement(parser);
    int local_variable_frame = get_sym_index();

    run_scope(parser);

    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = JMP;
    uint32_t main_ref = parser->bc_builder->current_builder_location;
    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = -1;

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
    int local_variable_frame = get_sym_index();

    run_scope(parser);

    update_sym_index(local_variable_frame);
}

void elif_statement(struct Parser* parser) {
    generic_condition(parser, ELIF, "elif");
}

void while_statement(struct Parser* parser) {
    match_token(parser, WHILE, "while");
 
    uint32_t start_ref = parser->bc_builder->current_builder_location;
    uint32_t jmp_reference = comparison_statement(parser);
    int local_variable_frame = get_sym_index();

    run_scope(parser);

    update_sym_index(local_variable_frame);

    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = JMP;
    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = start_ref;

    parser->bc_builder->opcodes[jmp_reference] = parser->bc_builder->current_builder_location;
}

void function_definition(struct Parser* parser) {
    struct Token* function_token = peek_next_token(parser);
    match_token(parser, ID, "function identifier");

    int func = search_type_symbol(function_token->token_string, FUNC);

    if (func == -1)
        func = add_symbol(function_token->token_string, FUNC)->index;
    else if(!get_symbols()[func].function.created)
        fatal_token_error("Redefinition of function", function_token);

    if (get_symbols()[func].function.created) {
        //This is the call of the function.
        match_token(parser, LPAR, "(");
        match_token(parser, RPAR, ")");
        match_token(parser, END_EXPRESSION, ";");

        parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = CALL;
        parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = get_symbols()[func].function.bytecode_address;
        parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = get_symbols()[func].function.arg_nums;
        parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = POP;
    }
    else {
        //This is the actual definition.

        match_token(parser, LPAR, "(");
        int id = -3;
        while (peek_next_token(parser)->type != RPAR) {
            struct Token* var_token = peek_next_token(parser);
            match_token(parser, ID, "identifier");

            bool is_const = false;
            match_token(parser, peek_next_token(parser)->type, ":");

            if (peek_next_token(parser)->type == CONST) {
                is_const = true;
                match_token(parser, CONST, "const");
            }

            struct VariableType* var_type = get_variable_types(peek_next_token(parser)->type);
            retrieve_next_token(parser);
           
            get_symbols()[func].function.arg_info[get_symbols()[func].function.arg_nums].is_const = is_const;
            get_symbols()[func].function.arg_info[get_symbols()[func].function.arg_nums].type = var_type->type;
            get_symbols()[func].function.arg_info[get_symbols()[func].function.arg_nums].value_type = var_type->value_type;
            get_symbols()[func].function.arg_info[get_symbols()[func].function.arg_nums].size = var_type->size;
            get_symbols()[func].function.arg_info[get_symbols()[func].function.arg_nums++].id = id--;      

            if(peek_next_token(parser)->type != RPAR)
                match_token(parser, COMMA, ",");
        }
        match_token(parser, RPAR, ")");

        match_token(parser, COLON, ":");
        match_token(parser, T_FUNC, "func");

        int local_variable_frame = get_sym_index();
        parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = JMP;
        uint32_t jmp_reference = parser->bc_builder->current_builder_location;
        parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = -1;
        uint32_t start_of_function = parser->bc_builder->current_builder_location;

        run_scope(parser);

        parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = ICONST;
        parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = get_symbols()[func].function.arg_nums;
        parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = RET;
        parser->bc_builder->opcodes[jmp_reference] = parser->bc_builder->current_builder_location;

        update_sym_index(local_variable_frame);

        //Load up the sym data
        get_symbols()[func].function.created = true;
        get_symbols()[func].function.bytecode_address = start_of_function;
    }
}