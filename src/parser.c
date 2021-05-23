#include "../include/parser.h"
#include "../include/stack.h"
#include "../include/expression.h"
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
        struct Token* token = peek_next_token(parser);
        
        if (token->type == T_EOF)
            break;
        switch (token->type) {
        case PRINT:
            print_statement(parser);
            break;
        case INT:
            variable_decleration_statement(parser);
            break;
        case ID:
            assignment_statement(parser);
            break;
        default:
            fatal_token_error("Undefined token", token);
        }
    }
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

void print_statement(struct Parser* parser) {
    match_token(parser, PRINT, "print");

    struct Token* token = peek_next_token(parser);
    
    struct Expression expression;

    parser->token_index += run_expression(&expression, parser->lexer, parser->token_index);
    printf("%f\n", calculate_expression(&expression));

    destroy_expression(&expression);
    match_token(parser, END_EXPRESSION, ";");
}

void assignment_statement(struct Parser* parser) {
    struct Token* token = peek_next_token(parser);
    match_token(parser, ID, "identifier");
    
    if (find_global_symbol(token->token_string) == -1)
        fatal_token_error("Undecleared variable", token);

    struct Symbol* var = get_global_symbol(token->token_string);  
    parser->token_index = equal_statement(parser, parser->token_index);
    match_token(parser, END_EXPRESSION, ";");
}

void variable_decleration_statement(struct Parser* parser) {
    match_token(parser, INT, "int");

    struct Token* token = peek_next_token(parser);

    match_token(parser, ID, "identifier");
    parser->token_index = equal_statement(parser, parser->token_index);

    match_token(parser, END_EXPRESSION, ";");
}

int equal_statement(struct Parser* parser, int end_token) {
    struct Token* var_token = parser->lexer->tokens->array[parser->token_index - 1];
    if (var_token->type != ID)
        fatal_token_error("Value needs to be lvalue", var_token);
    match_token(parser, EQUAL, "=");

    struct Token* token = peek_next_token(parser);

    int start_of_expression = parser->token_index;

    while (1) {
        if (token->type == EQUAL || token->type == END_EXPRESSION)
            break;
        token = retrieve_next_token(parser);
    }
    parser->token_index--;

    if (token->type == EQUAL)
        end_token = equal_statement(parser, end_token);
    if (token->type == END_EXPRESSION)
    {
        parser->token_index = start_of_expression;
        struct Expression expression;
    
        parser->token_index += run_expression(&expression, parser->lexer, parser->token_index);

        struct Symbol* variable = get_global_symbol(var_token->token_string);
        if (variable == NULL)
            add_symbol(var_token->token_string, calculate_expression(&expression));
        else
            variable->value = calculate_expression(&expression);
        
        end_token = parser->token_index;
        destroy_expression(&expression);
        return end_token;
    }
    parser->token_index = start_of_expression;

    struct Expression expression;
    
    parser->token_index += run_expression(&expression, parser->lexer, parser->token_index);

    struct Symbol* variable = get_global_symbol(var_token->token_string);
    if (variable == NULL)
        add_symbol(var_token->token_string, calculate_expression(&expression));
    else
        variable->value = calculate_expression(&expression);
    destroy_expression(&expression);

    return end_token;
}