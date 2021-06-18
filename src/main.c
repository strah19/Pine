#include "../include/lexer.h"
#include "../include/expression.h"
#include "../include/parser.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Must enter a file for the compiler to run and a name for executable.\n");
        exit(EXIT_FAILURE);
    }

    struct LexerLoader loader;
    loader.file_path = argv[1];
    struct Lexer *lexer;
    create_buffer_for_lexer(&loader);
    lexer = create_lexer(loader.buffer);

    struct Parser* parser = create_parser(lexer); 

    begin_debug_benchmark();
        run_tokenizer(lexer);
    end_debug_benchmark("Pine");

    log_token_data(lexer);

    destroy_parser(parser);
    destroy_lexer(lexer);



    return 0;
}