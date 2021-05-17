#include "../include/parser.h"
#include "../include/stack.h"
#include "../include/expression.h"

struct Parser* CreateParser(struct Lexer* lexer)
{
    struct Parser* parser;
    parser = malloc(sizeof(struct Parser));

    memset(parser, 0, sizeof(struct Parser));
    if(parser == NULL)
    {
        //Use error code here.
        exit(EXIT_FAILURE);
    }

    parser->lexer = lexer;
    parser->token_index = 0;

    return parser;
}

void DestroyParser(struct Parser* parser)
{
    free(parser);
}

void RunParser(struct Parser* parser)
{
    while(1)
    {
        struct Token* token = PeekNextToken(parser);
        
        if(token->type == T_EOF)
                break;

        switch(token->type)
        {
        case PRINT:
            MatchToken(parser, PRINT, "print");
            struct Expression expression;

            RunExpression(&expression, parser->lexer, END_EXPRESSION, parser->token_index);
            printf("%f\n", CalculateExpression(&expression));

            DestroyExpression(&expression);
            break;
        default:
            fprintf(stderr, "Error: Undefined token, '%s' found at line %d.\n", token->token_string, token->token_info.token_line);
        }

        while(1)
        {
            token = RetreaveNextToken(parser);
            if(token->type == END_EXPRESSION)
                break;
        }

    }
}

struct Token* PeekNextToken(struct Parser* parser)
{
    return (parser->token_index < parser->lexer->tokens->size) ? parser->lexer->tokens->array[parser->token_index] : NULL;
}

struct Token* RetreaveNextToken(struct Parser* parser)
{
    return (parser->token_index < parser->lexer->tokens->size) ? parser->lexer->tokens->array[parser->token_index++] : NULL;
}

void MatchToken(struct Parser* parser, enum TokenType type, const char* what)
{
    struct Token* token = PeekNextToken(parser);
    if(token)
    {
        if(token->type == type)
        {
            RetreaveNextToken(parser);
        }
        else 
        {
            printf("Error::Expected %s at line %d.\n", what, token->token_info.token_line);
        }
    }
}