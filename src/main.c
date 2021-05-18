#include "../include/lexer.h"
#include "../include/expression.h"
#include "../include/parser.h"
#include "../include/stack.h"

#define MAX_INPUT_LEN 256

int main(int argc, char *argv[])
{
    struct LexerLoader loader;
    loader.file_path = "main.pine";
    struct Lexer *lexer;
    CreateBufferForLexer(&loader);
    lexer = CreateLexer(loader.buffer);

    struct Parser* parser = CreateParser(lexer); 

    RunTokenizer(lexer);
    LogTokenData(lexer);
    RunParser(parser);

    DestroyParser(parser);
    DestroyLexer(lexer);
    /*
    This is for loading in a text file.
    struct LexerLoader loader;
    loader.file_path = "main.pine";
    struct Lexer *lexer;
    CreateBufferForLexer(&loader);
    lexer = CreateLexer(loader.buffer);
    */
    /*
    bool running = true;
    //lexer = CreateLexer("");

    while(running)
    {
        printf("Enter PEMDAS Equation: ");
        char input[MAX_INPUT_LEN];
        fgets(input, MAX_INPUT_LEN, stdin);

        lexer->input_text = input;
        RunTokenizer(lexer);
        struct Stack* postfix_not = RunExpression(lexer, END_EXPRESSION);

        printf("Expression Output: \n");
        for(int i = 0; i < postfix_not->top + 1; i++)
        {
            struct SYNode* node = postfix_not->array[i];
            printf("Node Value: %f, Node Op: %d\n", node->value, node->op);
        }
        struct ASTNode ast = CreateASTreeFromExpression(postfix_not);
        printf("Input: %s\n", input);
        printf("Output: %f\n", CalculateExpression(postfix_not));
        LogTokenData(lexer);

        RunParser(parser);
        ClearLexerData(lexer);
        free(postfix_not);
    }

    DestroyLexer(lexer);
    */

    return 0;
}