#include "../include/lexer.h"
#include "../include/expression.h"
#include "../include/parser.h"
#include "../include/bytecode_builder.h"
#include "../include/vm.h"
#include "../include/sym.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Must enter a file for the compiler to run.\n");
        exit(EXIT_FAILURE);
    }

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

    int32_t opcodes[] = {
        ICONST, 0,
        CALL, 6, 1,
        HALT,

        //Function start
        LOAD, -3,
        ICONST, 1,
        IADD,
        STORE,-3,
        LOAD, -3,
        SYS_WRITE,
        LOAD, -3,
        ICONST, 10,
        ILT,
        JMPN, 28,   //Pos 22
        LOAD, -3,
        CALL, 6, 1,
        ICONST, 0,
        RET,
    };

    run_vm(bc_builder->data_size, bc_builder->opcodes, 0);
    
    destroy_lexer(lexer);
    destroy_bc_builder(bc_builder);
    destroy_parser(parser);
    free(loader.text);

    return 0;
}