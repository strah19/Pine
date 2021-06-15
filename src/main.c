#include "../include/lexer.h"
#include "../include/expression.h"
#include "../include/parser.h"

int main(int argc, char *argv[])
{
    struct LexerLoader loader;
    loader.file_path = "main.pine";
    struct Lexer *lexer;
    create_buffer_for_lexer(&loader);
    lexer = create_lexer(loader.buffer);

    struct Parser* parser = create_parser(lexer); 

    begin_debug_benchmark();
        run_tokenizer(lexer);
        log_token_data(lexer);
        run_parser(parser);
    end_debug_benchmark("Pine");

    destroy_parser(parser);
    destroy_lexer(lexer);

    return 0;
}