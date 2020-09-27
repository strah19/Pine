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
        if ( StringCompare( lexer->current_possible_token, "int", 3) == 0 ) {
            VectorPushBack( lexer->tokens, AddToken( INT_VAR, "int", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( StringCompare( lexer->current_possible_token, "for", 3) == 0 ) {
            VectorPushBack( lexer->tokens, AddToken( FOR, "for", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( StringCompare( lexer->current_possible_token, "return", 6 ) == 0 ) {
            VectorPushBack( lexer->tokens, AddToken( RETURN, "return", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( StringCompare( lexer->current_possible_token, "main", 5) == 0 ) {
            VectorPushBack( lexer->tokens, AddToken( MAIN, "main", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( StringCompare( lexer->current_possible_token, "elif", 5) == 0 ) {
            VectorPushBack( lexer->tokens, AddToken( ELIF, "elif", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( StringCompare( lexer->current_possible_token, "else", 5) == 0 ) {
            VectorPushBack( lexer->tokens, AddToken( ELSE, "else", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( StringCompare( lexer->current_possible_token, "if", 2 ) == 0 ) {
            VectorPushBack( lexer->tokens, AddToken( IF, "if", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( StringCompare( lexer->current_possible_token, "<=", 2 ) == 0 ) {
            VectorPushBack( lexer->tokens, AddToken( LESS_THAN_EQUAL, "<=", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( StringCompare( lexer->current_possible_token, ">=", 2 ) == 0 ) {
            VectorPushBack( lexer->tokens, AddToken( GREATER_THAN_EQUAL, ">=", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }

        if ( IsCharDigit( lexer->current_possible_token[0] ) ) {
            if ( !IsCharDigit( ( char ) lexer->next_char ) ) {
                VectorPushBack( lexer->tokens, AddToken( INTEGER, lexer->current_possible_token, token_pos, lexer->copy_string_token ) );
                return CREATING_TOKEN;
            }
        }
        if ( lexer->current_possible_token[0] == ';' ) {
            VectorPushBack( lexer->tokens, AddToken( END_EXPRESSION, ";", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == '(' ) {
            VectorPushBack( lexer->tokens, AddToken( LPAR, "(", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == ')' ) {
            VectorPushBack( lexer->tokens, AddToken( RPAR, ")", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == '[' ) {
            VectorPushBack( lexer->tokens, AddToken( LBRACKET, "[", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == ']' ) {
            VectorPushBack( lexer->tokens, AddToken( RBRACKET, "]", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == '{' ) {
            VectorPushBack( lexer->tokens, AddToken( LCURLEY_BRACKET, "{", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == '}' ) {
            VectorPushBack( lexer->tokens, AddToken( RCURLEY_BRACKET, "}", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == '<' ) {
            VectorPushBack( lexer->tokens, AddToken( LESS_THAN, "<", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == '>' ) {
            VectorPushBack( lexer->tokens, AddToken( GREATER_THAN, ">", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }

        if ( lexer->current_possible_token[0] == '+' ) {
            VectorPushBack( lexer->tokens, AddToken( ADD, "+", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == '-' ) {
            VectorPushBack( lexer->tokens, AddToken( SUBTRACT, "-", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == '*' ) {
            VectorPushBack( lexer->tokens, AddToken( MULTIPLE, "*", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( lexer->current_possible_token[0] == '/' ) {
            VectorPushBack( lexer->tokens, AddToken( DIVIDE, "/", token_pos, lexer->copy_string_token  ) );
            return CREATING_TOKEN;           
        }
        if ( IsCharGoodForVariableName( lexer->current_possible_token[0] ) ) {
            if ( !IsCharGoodForVariableName( ( char ) lexer->next_char ) ) {
                VectorPushBack( lexer->tokens, AddToken( ID, lexer->current_possible_token, token_pos, lexer->copy_string_token ) );
                return CREATING_TOKEN;                
            }
        }
    }

    return NEED_MORE_STRING_FOR_TOKEN;
}

void
RunTokenizer( struct Lexer* lexer ) {
    int get_char = 0;       
    int current_character = 0;
    int next_character = 0;
    bool lexer_is_running = true;
    int current_state = 0;
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
            /* Do everything in here! */
            //printf ( "Current char: { %c } Next char: { %c }\n", current_character, next_character );

            switch ( current_lexer_state ) {
            case NEED_MORE_STRING_FOR_TOKEN: {
                if ( current_character != ' ' && current_character != '\n' && token_size < MAX_TOKEN_SIZE ) {
                    lexer->current_possible_token[token_size] = ( char ) current_character;
                    token_size++;
                }
                //printf( "Searching for token... %s\n", lexer->current_possible_token );
            }
            break;
            case CREATING_TOKEN: {
                //printf( "Found token! %s\n", lexer->current_possible_token );
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
            //printf( "%d\n", token_position.token_line );
            token_position.token_pos = 0;
        }    
        token_position.token_pos++;

        wait_on_loop = true;
    }
}