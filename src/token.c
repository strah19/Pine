#include "../include/token.h"

extern int 
MoveTokenCounter ( int value ) {
    static int code_token_id_counter = -1;
    code_token_id_counter += value;
    return code_token_id_counter;
}

struct Token* 
AddToken ( enum TokenType type, const char* in_source_token, struct TokenInfo token_info, char* copy_string ) {
    struct Token* token;
    token = malloc( sizeof( struct Token ) );
    if ( token == NULL ) {
        exit( EXIT_FAILURE );
    }

    memset( token, 0, sizeof( struct Token ) );

    if (in_source_token != NULL) {
        copy_string = malloc( strlen( in_source_token ) + 1 );
        strcpy( copy_string, in_source_token );
        token->token_string = copy_string;
    }
    token->token_info = token_info;
    token->token_info.token_pos -= ( sizeof( char ) * strlen( token->token_string ) );
    printf( "Token size: %d\n", ( sizeof( char ) * strlen( token->token_string ) ) );
    token->code_id = MoveTokenCounter( 1 );
    token->type = type;

    return token;
}
