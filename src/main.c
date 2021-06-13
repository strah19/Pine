#include "../include/lexer.h"
#include "../include/gen.h"
#include "../include/expression.h"
#include "../include/parser.h"

int main(int argc, char *argv[])
{
    struct LexerLoader loader;
    loader.file_path = "main.pine";
    struct Lexer *lexer;
    create_buffer_for_lexer(&loader);
    lexer = create_lexer(loader.buffer);
    initialize_output("output.S");

    struct Parser* parser = create_parser(lexer); 

    run_tokenizer(lexer);
    log_token_data(lexer);
    run_parser(parser);

    generate_code();

    destroy_parser(parser);
    destroy_lexer(lexer);
    close_output();

    return 0;
}