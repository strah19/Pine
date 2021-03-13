#include "../include/lexer.h"

int main(int argc, char* argv[])
{
    struct Lexer* lexer;
    lexer = CreateLexer( "main.pine" ); 

    RunTokenizer( lexer );
    LogTokenData( lexer );

    DestroyLexer( lexer );
    return 0;
}