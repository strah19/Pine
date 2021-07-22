#include "../include/lexer.h"
#include "../include/expression.h"
#include "../include/parser.h"
#include "../include/bytecode_builder.h"
#include "../include/vm.h"
#include "../include/sym.h"
#include "../include/err.h"

int main(int argc, char *argv[]) {
    if (argc < 2)
        fatal_error(NO_INPUT_ERROR);

    struct LexLoader loader = create_buffer_for_lexer(argv[1]);

    struct Lexer *lexer;
    lexer = create_lexer(&loader);

    struct ByteCodeBuilder* bc_builder = create_bc_builder();
    struct Parser* parser = create_parser(lexer, bc_builder); 

    begin_debug_benchmark();
        run_tokenizer(lexer);
        run_parser(parser);
    end_debug_benchmark("Pine");
    
    log_bytecode_in_file(bc_builder);
    finialize_bytecode(bc_builder);
    init_vm();  

    run_vm(bc_builder->data_size, bc_builder->opcodes, 0);
    
    destroy_lexer(lexer);
    destroy_bc_builder(bc_builder);
    destroy_parser(parser);
    free(loader.text);

    return 0;
}