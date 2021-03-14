#include "../include/lexer.h"

bool AssertLexer ( struct Lexer* lexer ) {
    return (lexer != NULL);
}

struct Lexer* CreateLexer ( const char* source_file_path ) {
    struct Lexer* lexer;
    lexer = malloc( sizeof( struct Lexer ) );
    if ( lexer == NULL ) {
        exit( EXIT_FAILURE );
    }

    memset( lexer, 0, sizeof( struct Lexer ) );
    if ( source_file_path == NULL ) {
        exit( EXIT_FAILURE );
    }

    lexer->source_file = fopen( source_file_path, LEXER_FILE_MODE );
    if ( lexer->source_file == NULL) {
        exit( EXIT_FAILURE );
    }

    lexer->tokens = CreateVector( sizeof( struct Token ) );

    return lexer;
}

int MakeToken ( struct Lexer* lexer, struct TokenInfo token_pos ) {
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
        else if ( IsCharDigit( lexer->current_possible_token[0] ) ) {
            if ( !IsCharDigit( ( char ) lexer->next_char ) ) {
                VectorPushBack( lexer->tokens, AddToken( INTEGER, lexer->current_possible_token, token_pos, lexer->copy_string_token ) );
                return CREATING_TOKEN;
            }
        }
        else if ( IsCharGoodForVariableName( lexer->current_possible_token[0] ) ) {
            if ( !IsCharGoodForVariableName( ( char ) lexer->next_char ) ) {
                VectorPushBack( lexer->tokens, AddToken( ID, lexer->current_possible_token, token_pos, lexer->copy_string_token ) );
                return CREATING_TOKEN;                
            }
        }

        for(size_t i = 0; i <  sizeof(token_pairs) / sizeof(token_pairs[0]); i++) {
            if( StringCompare( lexer->current_possible_token, token_pairs[i].in_code_name, strlen(token_pairs[i].in_code_name)) == 0) {
                VectorPushBack( lexer->tokens, AddToken( token_pairs[i].type, token_pairs[i].in_code_name, token_pos, lexer->copy_string_token ) );
                return CREATING_TOKEN; 
            }
        }
    }

    return NEED_MORE_STRING_FOR_TOKEN;
}

void RunTokenizer( struct Lexer* lexer ) {
    int get_char = 0;       
    int current_character = 0;
    int next_character = 0;
    bool lexer_is_running = true;
    int token_size = 0;
    bool wait_on_loop = false;
    struct TokenInfo token_position = { 0, 0 };
    int current_lexer_state = NEED_MORE_STRING_FOR_TOKEN;

    while ( lexer_is_running ) {
        get_char = fgetc( lexer->source_file );

        if ( !wait_on_loop ) {
            current_character = get_char;
        }
        
        if ( get_char == EOF ) {
            lexer_is_running = false;
        }

        next_character = get_char;

        lexer->current_char = current_character;
        lexer->next_char = next_character;
        if ( wait_on_loop ) {
            switch ( current_lexer_state ) {
            case NEED_MORE_STRING_FOR_TOKEN: {
                if ( current_character != ' ' && current_character != '\n' && token_size < MAX_TOKEN_SIZE ) {
                    lexer->current_possible_token[token_size] = ( char ) current_character;
                    token_size++;
                }
            }
            break;
            case CREATING_TOKEN: {
                for ( int i = 0; i < token_size; i++ ) {
                    lexer->current_possible_token[i] = ' ';
                }

                token_size = 0;
                if ( current_character != ' ' && current_character != '\n' && token_size < MAX_TOKEN_SIZE ) {
                    lexer->current_possible_token[token_size] = ( char ) current_character;
                    token_size++;
                }
            }
            break;
            }
        }

        current_lexer_state = MakeToken( lexer, token_position );

        current_character = next_character;
        if ( current_character == '\n' ) {
            token_position.token_line++;
            token_position.token_pos = 0;
        }    
        token_position.token_pos++;

        wait_on_loop = true;
    }
}

void LogTokenData ( struct Lexer* lexer ) {
    int pointer = 0;
    for (size_t i = 0; i < lexer->tokens->size; i++ ) {
        struct Token* token = lexer->tokens->array[i];
        printf( "Token[%d], Type: %d, { %s } ln: %d, pos: %d\n", token->code_id, token->type, token->token_string, token->token_info.token_line, token->token_info.token_pos );
    }
}

void DestroyLexer ( struct Lexer* lexer ) {
    fclose( lexer->source_file );
    FreeVector( lexer->tokens );
    free(lexer->copy_string_token);
    free( lexer );
}