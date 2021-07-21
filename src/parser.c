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

uint32_t comparison_statement(struct Parser* parser);
void assignment_statement(struct Parser* parser);
void expression_assignment(struct Parser* parser, struct ASTNode** root);
void print_statement(struct Parser* parser);


void if_statement(struct Parser* parser);
void else_statement(struct Parser* parser);
void elif_statement(struct Parser* parser);
int equal_statement(struct Parser* parser, int end_token, struct Token* var_token, struct ASTNode** root);
void while_statement(struct Parser* parser);

void function_statement(struct Parser* parser);
void function_definition(struct Parser* parser, int func_id);
void return_statement(struct Parser* parser);
void function_call(struct Parser* parser, int func_id);

bool find_const(struct Parser* parser);
void match_token(struct Parser* parser, enum TokenType type);
void id_found(struct Parser* parser);
int check_for_var_redefination(struct Token* token);

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
    while (PARSER_LOOP) 
        if(!run_statements(parser))
            break;
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
        fatal_token_error(ELSE_WITHOUT_IF_ERROR, token);
        break;
    case ELIF:
        fatal_token_error(ELIF_WITHOUT_IF_ERROR, token);
        break;
    case WHILE:
        while_statement(parser);
        break;
    case RETURN:
        return_statement(parser);
        break;
    default:
        fatal_token_error(UNDEFINED_TOKEN_ERROR, token);
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

void match_token(struct Parser* parser, enum TokenType type) {
    struct Token* token = peek_next_token(parser);
    if (token) 
        if (token->type == type)
            retrieve_next_token(parser);
        else 
            fatal_compiler_error("Expected", convert_type_to_str(type), token->token_info.token_line);
}

void id_found(struct Parser* parser) {
    if (peek_offset_token(parser, 1)->type == LPAR)
        function_statement(parser);
    else
        assignment_statement(parser);
}

bool check_id_arg(int id) {
    return (id < -2);
}

int get_arg_id(int id) {
    return (abs(id) - 3);
}

bool find_const(struct Parser* parser) {
    if (peek_next_token(parser)->type == CONST) {
        match_token(parser, CONST);
        return true;
    }
    return false;
}

void expression_assignment(struct Parser* parser, struct ASTNode** root) {
    struct ASTNode* ast_tree;
    make_ast_from_expr(&ast_tree, parser);
    *root = ast_tree;
}

void print_statement(struct Parser* parser) {
    match_token(parser, PRINT);
    
    struct ASTNode* ast;
    make_ast_from_expr(&ast, parser);

    build_expression(parser->bc_builder, ast);
    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = SYS_WRITE;

    destroy_ast_node(ast);
    match_token(parser, END_EXPRESSION);
}

int check_for_var_redefination(struct Token* token) {
    if (search_type_symbol(token->token_string, VAR) == UNKNOWN_ID)
        return add_symbol(token->token_string, VAR)->index;
    else
        fatal_token_error(VAR_REDEFINITION_ERROR, token);
    return UNKNOWN_ID;
}

void fill_var_sym_data(int var_id, struct VariableType* var_type) {
    get_symbols()[var_id].var.type = var_type->type;
    get_symbols()[var_id].var.size = var_type->size;
    get_symbols()[var_id].var.value_type = var_type->value_type;
}

void assignment_statement(struct Parser* parser) {
    struct ASTNode* ast = NULL;

    struct Token* var_token = peek_next_token(parser);
    match_token(parser, ID);

    int var_id = UNKNOWN_ID;
    bool is_const = false;

    if (peek_next_token(parser)->type == COLON) {
        match_token(parser, peek_next_token(parser)->type);
        is_const = find_const(parser);

        struct VariableType* var_type = get_variable_types(peek_next_token(parser)->type);

        if (var_type == NULL)
            fatal_token_error(UNKNOWN_TYPE_ERROR, peek_next_token(parser));

        retrieve_next_token(parser);

        var_id = check_for_var_redefination(var_token);

        fill_var_sym_data(var_id, var_type);

        if (get_sym_index() > parser->bc_builder->data_size)
            parser->bc_builder->data_size++;

        if (peek_next_token(parser)->type == END_EXPRESSION) {
            if (is_const) 
                fatal_token_error(CONST_MUST_BE_DEFINED_ERROR, peek_next_token(parser));

            match_token(parser, peek_next_token(parser)->type);

            ast = create_ast_node(EQUAL, create_ast_node_fill(ID, NULL, NULL, ast, var_type->value_type), create_ast_node_fill(var_type->value_type, NULL, NULL, ast, var_type->value_type));
            ast->left->var_id = get_symbols()[var_id].var.id;

            build_decleration(parser->bc_builder, ast);
            destroy_ast_node(ast);
            return;
        }
    }
    else if (search_type_symbol(var_token->token_string, VAR) == UNKNOWN_ID)
        fatal_token_error(UNDEFINED_ID_ERROR, var_token);

    parser->token_index = equal_statement(parser, parser->token_index, var_token, &ast);

    if (is_const) 
        get_symbols()[var_id].var.is_const = is_const;

    match_token(parser, END_EXPRESSION);

    begin_sem();
    validate_ast(ast);

    build_assignment(parser->bc_builder, ast);
    destroy_ast_node(ast);
}

int equal_statement(struct Parser* parser, int end_token, struct Token* var_token, struct ASTNode** root) {
    *root = create_ast_node(EQUAL, NULL, NULL);

    int id = search_type_symbol(var_token->token_string, VAR);
    if (id < -2)
        (*root)->left = create_ast_node_fill(ID, NULL, NULL, *root, get_function()->arg_info[abs(id) - 3].var_info.value_type);
    else
        (*root)->left = create_ast_node_fill(ID, NULL, NULL, *root, get_symbols()[search_type_symbol(var_token->token_string, VAR)].var.value_type);

    if (id == UNKNOWN_ID)
        fatal_token_error(UNDEFINED_ID_ERROR, var_token);

    if (check_id_arg(id))
        (*root)->left->var_id = get_function()->arg_info[get_arg_id(id)].var_info.id;
    else
        (*root)->left->var_id = get_symbols()[id].var.id;

    if (var_token->type != ID)
        fatal_token_error(MUST_BE_LVALUE_ERROR, var_token);
    if (check_id_arg(id)) {
        if (get_function()->arg_info[get_arg_id(id)].var_info.is_const)
            fatal_token_error(LVALUE_CONST_ERROR, var_token);
    }
    else
        if (get_symbols()[search_type_symbol(var_token->token_string, VAR)].var.is_const)
            fatal_token_error(LVALUE_CONST_ERROR, var_token);

    match_token(parser, EQUAL);

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
                fatal_token_error(MUST_BE_LVALUE_ERROR, next_var);

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
    match_token(parser, LCURLEY_BRACKET);
    uint32_t left_bracket_match = parser->token_index;
    while(peek_next_token(parser)->type != RCURLEY_BRACKET && peek_next_token(parser)->type != T_EOF) 
        run_statements(parser);
    if (peek_next_token(parser)->type != RCURLEY_BRACKET) 
        fatal_token_error(MISMATCHED_LCURLY_ERROR, &parser->lexer->tokens[left_bracket_match]);
    match_token(parser, RCURLEY_BRACKET);
}

uint32_t comparison_statement(struct Parser* parser) {
    struct ASTNode* ast_tree;
    make_ast_from_expr(&ast_tree, parser);

    build_expression(parser->bc_builder, ast_tree);

    uint32_t jmp_reference = get_jmpn_reference(parser->bc_builder);
    destroy_ast_node(ast_tree);    

    return jmp_reference;
}

void generic_condition(struct Parser* parser, uint32_t condition_type) {
    match_token(parser, condition_type);

    uint32_t jmp_reference = comparison_statement(parser);
    int local_variable_frame = get_sym_index();

    run_scope(parser);

    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = JMP;
    uint32_t main_ref = parser->bc_builder->current_builder_location;
    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = UNKNOWN_ID;

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
    generic_condition(parser, IF);
}

void else_statement(struct Parser* parser) {
    match_token(parser, ELSE);
    int local_variable_frame = get_sym_index();

    run_scope(parser);

    update_sym_index(local_variable_frame);
}

void elif_statement(struct Parser* parser) {
    generic_condition(parser, ELIF);
}

void while_statement(struct Parser* parser) {
    match_token(parser, WHILE);
 
    uint32_t start_ref = parser->bc_builder->current_builder_location;
    uint32_t jmp_reference = comparison_statement(parser);
    int local_variable_frame = get_sym_index();

    run_scope(parser);

    update_sym_index(local_variable_frame);

    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = JMP;
    parser->bc_builder->opcodes[parser->bc_builder->current_builder_location++] = start_ref;

    parser->bc_builder->opcodes[jmp_reference] = parser->bc_builder->current_builder_location;
}

void function_statement(struct Parser* parser) {
    struct Token* function_token = peek_next_token(parser);
    match_token(parser, ID);

    int func_id = search_type_symbol(function_token->token_string, FUNC);

    if (func_id == UNKNOWN_ID) {
        if (get_function())
            fatal_token_error(FUNC_IN_FUNC_ERROR, function_token);
        func_id = add_symbol(function_token->token_string, FUNC)->index;
    }
    else if (!get_symbols()[func_id].function.created)
        fatal_token_error(FUNC_REDEFINITION_ERROR, function_token);

    if (get_symbols()[func_id].function.created)
        function_call(parser, func_id);
    else
        function_definition(parser, func_id);
}

void function_definition(struct Parser* parser, int func_id) {
    match_token(parser, LPAR);
    int id = ARG_OFFSET;
    get_symbols()[func_id].function.arg_nums = 0;
    get_symbols()[func_id].function.created = true;

    fill_return_info(&get_symbols()[func_id].function, NONE, 0, NONE, false, UNKNOWN_ID);

    while (peek_next_token(parser)->type != RPAR) {
        struct Token* var_token = peek_next_token(parser);
        match_token(parser, ID);
        match_token(parser, COLON);

        bool is_const = find_const(parser);

        struct VariableType* var_type = get_variable_types(peek_next_token(parser)->type);
        retrieve_next_token(parser);

        fill_func_info(&get_symbols()[func_id].function.arg_info[get_symbols()[func_id].function.arg_nums++], var_type, &id, is_const, var_token->token_string);

        if (peek_next_token(parser)->type != RPAR)
            match_token(parser, COMMA);
    }
    match_token(parser, RPAR);

    if (peek_next_token(parser)->type == ARROW_PTR) {
        match_token(parser, ARROW_PTR);

        struct VariableType* var_type = get_variable_types(peek_next_token(parser)->type);

        if (var_type == NULL)
            fatal_token_error(UNKNOWN_TYPE_ERROR, peek_next_token(parser));

        match_token(parser, peek_next_token(parser)->type);

        fill_return_info(&get_symbols()[func_id].function, var_type->type, var_type->size, var_type->value_type, false, UNKNOWN_ID);
    }

    int local_variable_frame = get_sym_index();
    uint32_t jmp_reference = get_jmp_reference(parser->bc_builder);
    uint32_t start_of_function = parser->bc_builder->current_builder_location;
    get_symbols()[func_id].function.bytecode_address = start_of_function;

    set_current_function(&get_symbols()[func_id].function);
    run_scope(parser);
    set_current_function(NULL);

    build_function_return_no_value(parser->bc_builder);

    parser->bc_builder->opcodes[jmp_reference] = parser->bc_builder->current_builder_location;
    update_sym_index(local_variable_frame);
}

void function_call(struct Parser* parser, int func_id) {
    int arg_tracker = 0;

    match_token(parser, LPAR);
    struct ASTNode* ast[MAX_ARGS];
    while (peek_next_token(parser)->type != RPAR) {
        make_ast_from_expr(&ast[arg_tracker], parser);

        if (peek_next_token(parser)->type != RPAR) 
            match_token(parser, COMMA);
        arg_tracker++;
    }

    for (int i = arg_tracker - 1; i >= 0; i--) {
        build_expression(parser->bc_builder, ast[i]);
        destroy_ast_node(ast[i]);
    }

    if (arg_tracker != get_symbols()[func_id].function.arg_nums) 
        fatal_error(MISMATCHED_ARG_NUMS_ERROR);

    match_token(parser, RPAR);
    match_token(parser, END_EXPRESSION);
    
    build_function_call(parser->bc_builder, func_id);
}

void return_statement(struct Parser* parser) {
    if (get_function() == NULL)
        fatal_error(RETURN_WITHOUT_FUN_ERROR);

    match_token(parser, RETURN);
    if (peek_next_token(parser)->type == END_EXPRESSION) {
        struct FuncSym* function = get_function();

        if (function->return_info.type != NONE) {
            fatal_error(RETURN_WITHOUT_VALUE_IN_NONE_VOID_FUN_ERROR);
        }

        build_function_return_no_value(parser->bc_builder);
    }
    else {
        struct ASTNode* ast;
        make_ast_from_expr(&ast, parser);
        build_expression(parser->bc_builder, ast);
        destroy_ast_node(ast);

        build_function_return(parser->bc_builder);
    }
    match_token(parser, END_EXPRESSION);
}