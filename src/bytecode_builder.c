/**
 * @file bytecode_builder.c
 * @author strah19
 * @date June 15 2021
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * @section DESCRIPTION
 *
 * This file contains code for generating Pine's own bytecode that
 * will be run inside a vm. This will make it is easy for cross
 * platform and can later be converted into native machine code 
 * depending on platform (possibly using LLVM).
 */

#include "../include/bytecode_builder.h"
#include "../include/err.h"
#include "../include/opcodes.h"
#include "../include/sym.h"

#define INITIAL_OPCODE_SIZE 256

struct ByteCodeBuilder* create_bc_builder() {
    struct ByteCodeBuilder* bc_builder;
    bc_builder = malloc(sizeof(struct ByteCodeBuilder));

    if (!bc_builder)
        fatal_error("ByteCodeBuilder was NULL");

    bc_builder->opcodes = malloc(sizeof(uint32_t) * INITIAL_OPCODE_SIZE);
    bc_builder->opcode_size = INITIAL_OPCODE_SIZE;
    bc_builder->data_size = 0;
    bc_builder->current_builder_location = 0;

    return bc_builder;
}

void destroy_bc_builder(struct ByteCodeBuilder* bc_builder) {
    free(bc_builder->opcodes);
    free(bc_builder);
}

void finialize_bytecode(struct ByteCodeBuilder* bc_builder) {
    bc_builder->opcodes[bc_builder->current_builder_location++] = HALT;
}

void analyize_opcode_storage(struct ByteCodeBuilder* bc_builder) {
    if (bc_builder->current_builder_location + 1 > bc_builder->opcode_size) {
        bc_builder->opcodes = realloc(bc_builder->opcodes, (sizeof(uint32_t) * bc_builder->opcode_size) * 2);
        if (!bc_builder->opcodes) 
            fatal_error("Failed to resize opcodes");
        bc_builder->opcode_size *= 2;
    }
}

void bc_equal(struct ByteCodeBuilder* bc_builder, struct ASTNode* root) {
    if (!root || root->op != EQUAL) return;

    bc_equal(bc_builder, root->right);
    
    bc_builder->opcodes[bc_builder->current_builder_location++] = ICONST;

    int val;
    if (root->right->op == ID)
        val = (int) get_symbols()[root->right->var_id].value;
    else
        val = root->right->int_val;
    bc_builder->opcodes[bc_builder->current_builder_location++] = val;
    bc_builder->opcodes[bc_builder->current_builder_location++] = GSTORE;
    bc_builder->opcodes[bc_builder->current_builder_location++] = root->left->var_id;
    root->int_val = val;
}

void bc_decleration(struct ByteCodeBuilder* bc_builder, struct ASTNode* root) {
    bc_equal(bc_builder, root);
    bc_builder->data_size++;
}