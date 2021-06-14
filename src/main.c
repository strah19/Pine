#include "../include/lexer.h"
#include "../include/expression.h"
#include "../include/parser.h"
#include <time.h>

int main(int argc, char *argv[])
{
    struct LexerLoader loader;
    loader.file_path = "main.pine";
    struct Lexer *lexer;
    create_buffer_for_lexer(&loader);
    lexer = create_lexer(loader.buffer);

    struct Parser* parser = create_parser(lexer); 

    clock_t begin = clock();
    run_tokenizer(lexer);
    log_token_data(lexer);
    
    run_parser(parser);

    clock_t end = clock();
    double time_spent = (double)(end - begin);
    printf("Execuation time in ms: %f", time_spent);


    destroy_parser(parser);
    destroy_lexer(lexer);

    return 0;
}