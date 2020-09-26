#include "../include/lexer.h"

bool
AssertLexer ( struct Lexer* lexer ) {
    return (lexer != NULL);
}

struct Lexer*
CreateLexer ( const char* source_file_path ) {
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

int
FindDigits ( struct Lexer* lexer, struct TokenInfo token_pos ) {
    static int number_size = 0;
    if ( IsCharDigit( lexer->current_possible_token[number_size] ) ) {
        number_size++;
    }
    else {
        if ( number_size > 0 ) {
            VectorPushBack( lexer->tokens, AddToken( INTEGER, lexer->current_possible_token, token_pos, lexer->copy_string_token ) );
            number_size = 0;
            return CREATING_TOKEN;
        }
    }
    return NEED_MORE_STRING_FOR_TOKEN;
}

int
MakeToken ( struct Lexer* lexer, struct TokenInfo token_pos ) {
    if ( AssertLexer( lexer ) ) {


        if ( FindDigits( lexer, token_pos ) == CREATING_TOKEN ) {
            return CREATING_TOKEN;
        }

        if ( StringCompare( lexer->current_possible_token, "int", 3) == 0 ) {
            VectorPushBack( lexer->tokens, AddToken( INTEGER, lexer->current_possible_token, token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }

        static int var_name_length = 0;
        if ( IsCharGoodForVariableName( lexer->current_possible_token[var_name_length] ) ) {
            var_name_length++;
        }
        else {
            if ( var_name_length > 0 ) {
                int size = ( sizeof( char ) * strlen( lexer->current_possible_token ) );
                char top_char = lexer->current_possible_token[size - 1];
                lexer->current_possible_token[size - 1] = ' ';
                VectorPushBack( lexer->tokens, AddToken( ID, lexer->current_possible_token, token_pos, lexer->copy_string_token ) );
                var_name_length = 0;
                for ( int i = 0; i < size; i++ ) {
                    lexer->current_possible_token[i] = ' ';
                }
                lexer->current_possible_token[0] = top_char;
                if ( lexer->current_possible_token[0] == ';' ) {
                    VectorPushBack( lexer->tokens, AddToken( END_EXPRESSION, lexer->current_possible_token, token_pos, lexer->copy_string_token  ) );
                    return CREATING_TOKEN;           
                }
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
            }
        }
    }

    return NEED_MORE_STRING_FOR_TOKEN;
}

void
RunTokenizer( struct Lexer* lexer ) {
    int current_character = 0;
    int current_lexer_state = 0;

    struct TokenInfo token_position = { 0, 0 };

    int token_size = 0;

    while( ( current_character = fgetc( lexer->source_file ) ) != EOF ) {
        switch ( current_lexer_state ) {
        case NEED_MORE_STRING_FOR_TOKEN: {
            if ( current_character != ' ' && current_character != '\n' && token_size < MAX_TOKEN_SIZE ) {
                lexer->current_possible_token[token_size] = ( char ) current_character;
                token_size++;
            }
            printf( "Searching for token... %s\n", lexer->current_possible_token );
        }
        break;
        case CREATING_TOKEN: {
            printf( "Found token! %s\n", lexer->current_possible_token );
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

        current_lexer_state = MakeToken( lexer, token_position );

        if ( current_character == '\n' ) {
            token_position.token_line++;
            printf( "%d\n", token_position.token_line );
            token_position.token_pos = 0;
        }    
        token_position.token_pos++;            
    }
}