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
    struct Stack* postfix = RunShuntYardAlgorithim(lexer);

    printf("Lexer output: %d\n", RunPostfixNotation(postfix));

    DestroyLexer(lexer);
    free(loader.buffer);
    free(postfix);

    return 0;
}