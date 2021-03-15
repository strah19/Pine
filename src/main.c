#include "../include/lexer.h"
#include "../include/shunt_yard_algorithim.h"

int main(int argc, char *argv[])
{
    struct LexerLoader loader;
    loader.file_path = "main.pine";
    struct Lexer *lexer;

    lexer = CreateLexer(CreateBufferForLexer(&loader));

    RunTokenizer(lexer);
    LogTokenData(lexer);
    RunShuntYardAlgorithim(lexer);

    DestroyLexer(lexer);
    free(loader.buffer);
    return 0;
}