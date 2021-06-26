#ifndef BYTECODE_BUILDER_H
#define BYTECODE_BUILDER_H

#include <stdint.h>
#include "../include/expression.h"

struct ByteCodeBuilder {
    uint32_t* opcodes;
    uint32_t current_builder_location;
    uint32_t opcode_size;

    uint32_t data_size;
};

extern struct ByteCodeBuilder* create_bc_builder();

extern void destroy_bc_builder(struct ByteCodeBuilder* bc_builder);

extern void analyize_opcode_storage(struct ByteCodeBuilder* bc_builder);

extern void build_decleration(struct ByteCodeBuilder* bc_builder, struct ASTNode* root);

extern void finialize_bytecode(struct ByteCodeBuilder* bc_builder);

extern uint32_t calculate_bin_operator(struct ByteCodeBuilder* bc_builder, struct ASTNode* bin);

extern uint32_t get_jmp_reference(struct ByteCodeBuilder* bc_builder, struct ASTNode* comparative_statement);

extern void build_expression(struct ByteCodeBuilder* bc_builder, struct ASTNode* root);

#endif // !BYTECODE_BUILDER_H