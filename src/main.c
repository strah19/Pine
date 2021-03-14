#include "../include/lexer.h"

int main(int argc, char* argv[])
{
    struct LexerLoader loader;
    loader.file_path = "main.pine";
    struct Lexer* lexer;

    lexer = CreateLexer(CreateBufferForLexer(&loader)); 

    RunTokenizer( lexer );
    LogTokenData( lexer );

    DestroyLexer( lexer );
    free(loader.buffer);
    return 0;
}