#include "../include/lexer.h"

/*
int
OperatorTokenizer ( struct Lexer* lexer, struct TokenInfo token_pos ) {
    if ( lexer->current_possible_token[0] == '+' ) {
        VectorPushBack( lexer->tokens, AddToken( ADD, lexer->current_possible_token, token_pos, lexer->copy_string_token ) );
        return CREATING_TOKEN;
    }
    if ( lexer->current_possible_token[0] == '-' ) {
        VectorPushBack( lexer->tokens, AddToken( SUBTRACT, lexer->current_possible_token, token_pos, lexer->copy_string_token  ) );
        return CREATING_TOKEN;
    }
    if ( lexer->current_possible_token[0] == '*' ) {
        VectorPushBack( lexer->tokens, AddToken( MULTIPLE, lexer->current_possible_token, token_pos, lexer->copy_string_token ) );
        return CREATING_TOKEN;
    }
    if ( lexer->current_possible_token[0] == '/' ) {
        VectorPushBack( lexer->tokens, AddToken( DIVIDE, lexer->current_possible_token, token_pos, lexer->copy_string_token  ) );
        return CREATING_TOKEN;
    }
}

int
MakeToken ( struct Lexer* lexer, struct TokenInfo token_pos ) {
    if ( AssertLexer( lexer ) ) {
        if ( lexer->current_possible_token[0] == '=' ) {
            VectorPushBack( lexer->tokens, AddToken( EQUAL, lexer->current_possible_token,  token_pos, lexer->copy_string_token ) );
            struct Token* token_1 = lexer->tokens->array[lexer->tokens->size - 1];
            struct Token* token_2 = lexer->tokens->array[lexer->tokens->size - 2];

            if ( token_1->type == EQUAL && token_2->type == EQUAL ) {
                if ( token_1->token_info.token_pos - token_2->token_info.token_pos == 1 ) {
                    VectorErase( lexer->tokens, lexer->tokens->size - 1, sizeof( struct Token ) );
                    VectorErase( lexer->tokens, lexer->tokens->size - 2, sizeof( struct Token ) );
                    MoveTokenCounter( -2 );
                    VectorPushBack( lexer->tokens, AddToken( DOUBLE_EQUAL, "==", token_pos, lexer->copy_string_token ) );
                }
            }           
            return CREATING_TOKEN;
        }

        if ( StringCompare( lexer->current_possible_token, "if", 2) == 0 ) {
            VectorPushBack( lexer->tokens, AddToken( IF, lexer->current_possible_token, token_pos, lexer->copy_string_token ) );
            return CREATING_TOKEN;           
        }
        if ( StringCompare( lexer->current_possible_token, "int", 3) == 0 ) {
            VectorPushBack( lexer->tokens, AddToken( INTEGER, lexer->current_possible_token, token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == '(' ) {
            VectorPushBack( lexer->tokens, AddToken( LPAR, lexer->current_possible_token, token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == ')' ) {
            VectorPushBack( lexer->tokens, AddToken( RPAR, lexer->current_possible_token, token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }

        if ( OperatorTokenizer( lexer, token_pos ) == CREATING_TOKEN ) {
            return CREATING_TOKEN;
        }

        static int number_size = 0;
        if ( IsCharDigit( lexer->current_possible_token[number_size] ) > -1 ) {
            number_size++;
        }
        else {
            if ( number_size > 0 ) {
                VectorPushBack( lexer->tokens, AddToken( INTEGER, lexer->current_possible_token, token_pos, lexer->copy_string_token ) );
                number_size = 0;
                return CREATING_TOKEN;
            }
        }

        static int var_name_length = 0;
        if ( IsCharGoodForVariableName( lexer->current_possible_token[var_name_length] ) ) {
            var_name_length++;
        }
        else {
            if ( var_name_length > 0 ) {
                VectorPushBack( lexer->tokens, AddToken( ID, lexer->current_possible_token, token_pos, lexer->copy_string_token ) );
                var_name_length = 0;
                return CREATING_TOKEN;
            }
        }
    }

    return NEED_MORE_STRING_FOR_TOKEN;
}
*/
void 
LogTokenData ( struct Lexer* lexer ) {
    int pointer = 0;
    for (size_t i = 0; i < lexer->tokens->size; i++ ) {
        struct Token* token = lexer->tokens->array[i];
        printf( "Token[%d], Type: %d, { %s } ln: %d, pos: %d\n", token->code_id, token->type, token->token_string, token->token_info.token_line, token->token_info.token_pos );
    }
}

void 
DestroyLexer ( struct Lexer* lexer ) {
    fclose( lexer->source_file );
    FreeVector( lexer->tokens );
    free(lexer->copy_string_token);
    free( lexer );
}

int main(int argc, char* argv[])
{
    struct Lexer* lexer;
    lexer = CreateLexer( "main.pine" );

    RunTokenizer( lexer );

    LogTokenData( lexer );
    DestroyLexer( lexer );
    return 0;
}