#include "../include/lexer.h"
#include "../include/expression.h"
#include "../include/parser.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Must enter a file for the compiler to run.\n");
        exit(EXIT_FAILURE);
    }

    struct LexLoader loader = create_buffer_for_lexer(argv[1]);

    struct Lexer *lexer;
    lexer = create_lexer(&loader);

    struct Parser* parser = create_parser(lexer); 

    begin_debug_benchmark();
        run_tokenizer(lexer);
    float t = end_debug_benchmark("Pine");

    if (strcmp(argv[2],"debug") == 0) 
        log_token_data(lexer);

    printf("Lexer took %f ms.\n", t);

    destroy_parser(parser);
    destroy_lexer(lexer);
    free(loader.text);

    return 0;
}