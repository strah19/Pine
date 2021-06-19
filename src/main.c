#include "../include/lexer.h"
#include "../include/expression.h"
#include "../include/parser.h"
#include "../include/cgen.h"
#include "../include/vm.h"

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Must enter a file for the compiler to run AND an executable name.\n");
        exit(EXIT_FAILURE);
    }

    struct LexLoader loader = create_buffer_for_lexer(argv[1]);

    struct Lexer *lexer;
    lexer = create_lexer(&loader);

    struct Parser* parser = create_parser(lexer); 


    begin_code_gen(argv[2]);

    begin_debug_benchmark();
        run_tokenizer(lexer);
        run_parser(parser);
    end_debug_benchmark("Pine");

    log_token_data(lexer);

    finalize_code();
    compile_and_link(argv[2]);

    destroy_parser(parser);
    destroy_lexer(lexer);
    free(loader.text);

    return 0;
}