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
#include "../include/expression.h"

#include <stdio.h>

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

void analyze_opcode_storage(struct ByteCodeBuilder* bc_builder) {
    if (bc_builder->current_builder_location + 1 > bc_builder->opcode_size) {
        bc_builder->opcodes = realloc(bc_builder->opcodes, (sizeof(uint32_t) * bc_builder->opcode_size) * 2);
        if (!bc_builder->opcodes) 
            fatal_error("Failed to resize opcodes");
        bc_builder->opcode_size *= 2;
    }
}

uint32_t convert_opcode_operator(struct ByteCodeBuilder* bc_builder, struct ASTNode* bin) {
    if (bin && bin->left && bin->right) {
        switch(bin->op) {
            case T_PLUS: return IADD;
            case T_MINUS: return ISUB;
            case T_STAR: return IMUL;
            case T_SLASH: return IDIV;
            case T_DOUBLE_EQUAL: return IEQ;
            case T_OR: return IOR;
            case T_AND: return IAND;
            case T_LESS_THAN: return ILT;
            case T_GREATER_THAN: return IGT;
            case T_LESS_THAN_EQUAL: return ILTE;
            case T_GREATER_THAN_EQUAL: return IGTE;
            case T_NOT: return INEQ;
        }   
    }

    return HALT;
}

void push_node(struct ByteCodeBuilder* bc_builder, struct ASTNode* root) {
    switch (root->type) {
    case T_INTEGER:
        bc_builder->opcodes[bc_builder->current_builder_location++] = ICONST;
        bc_builder->opcodes[bc_builder->current_builder_location++] = root->int_val;
        break;
    case T_CHAR:
        bc_builder->opcodes[bc_builder->current_builder_location++] = CHARCONST;
        bc_builder->opcodes[bc_builder->current_builder_location++] = root->char_val;
        break;
    }
}

void load_variable(struct ByteCodeBuilder* bc_builder, int var_id) {
    if (var_id < 0) 
        bc_builder->opcodes[bc_builder->current_builder_location++] = LOAD;
    else 
        bc_builder->opcodes[bc_builder->current_builder_location++] = GLOAD;
    bc_builder->opcodes[bc_builder->current_builder_location++] = var_id;    
}

void store_variable(struct ByteCodeBuilder* bc_builder, int var_id) {
    if (var_id < 0)
        bc_builder->opcodes[bc_builder->current_builder_location++] = STORE;
    else
        bc_builder->opcodes[bc_builder->current_builder_location++] = GSTORE;
    bc_builder->opcodes[bc_builder->current_builder_location++] = var_id;
}

void build_node(struct ByteCodeBuilder* bc_builder, struct ASTNode* root) {
    if (root->op == T_ID)
        load_variable(bc_builder, root->var_id);
    if (root->op == T_INTEGER || root->op == T_CHAR)
        push_node(bc_builder, root);
}

static void build_expression_core(struct ByteCodeBuilder* bc_builder, struct ASTNode* root) {
    if (!root || !root->left || !root->right) return;

    build_expression_core(bc_builder, root->left);
    build_expression_core(bc_builder, root->right);

    if (root->op != T_EQUAL) {
        build_node(bc_builder, root->left);
        build_node(bc_builder, root->right);

        bc_builder->opcodes[bc_builder->current_builder_location++] = convert_opcode_operator(bc_builder, root);
    }
    else {
        build_node(bc_builder, root->right);
        store_variable(bc_builder, root->left->var_id);

        if (root->parent != NULL) 
            load_variable(bc_builder, root->left->var_id);
    }
}

void build_expression(struct ByteCodeBuilder* bc_builder, struct ASTNode* root) {
    build_expression_core(bc_builder, root);
    build_node(bc_builder, root);
}

void build_decleration(struct ByteCodeBuilder* bc_builder, struct ASTNode* root) {
    push_node(bc_builder, root->left);
    store_variable(bc_builder, root->left->var_id);
}

void build_assignment(struct ByteCodeBuilder* bc_builder, struct ASTNode* root) {
    build_expression(bc_builder, root->right);
    build_node(bc_builder, root->right);
    store_variable(bc_builder, root->left->var_id);
}

uint32_t get_jmpn_reference(struct ByteCodeBuilder* bc_builder) {
    bc_builder->opcodes[bc_builder->current_builder_location++] = JMPN;
    uint32_t ref = bc_builder->current_builder_location;
    bc_builder->opcodes[bc_builder->current_builder_location++] = UNKNOWN_ID;

    return ref;
}

uint32_t get_jmp_reference(struct ByteCodeBuilder* bc_builder) {
    bc_builder->opcodes[bc_builder->current_builder_location++] = JMP;
    uint32_t ref = bc_builder->current_builder_location;
    bc_builder->opcodes[bc_builder->current_builder_location++] = UNKNOWN_ID;

    return ref;
}

void log_bytecode_in_file(struct ByteCodeBuilder* bc_builder) {
    FILE* file = fopen("bytecodes.txt", "w");

    uint32_t* opcodes = bc_builder->opcodes;

    if (file) {
        size_t n = sizeof(opcodes) / sizeof(uint32_t);

        for (int i = 0; i < bc_builder->current_builder_location; i++)
            fprintf(file, "%d ", opcodes[i]);
    }
    else 
        fatal_error("Could not open file for bytecode dump");

    fclose(file);
}

void build_function_return(struct ByteCodeBuilder* bc_builder) {
    bc_builder->opcodes[bc_builder->current_builder_location++] = RET;
}

void build_function_return_no_value(struct ByteCodeBuilder* bc_builder) {
    bc_builder->opcodes[bc_builder->current_builder_location++] = ICONST;
    bc_builder->opcodes[bc_builder->current_builder_location++] = 0;
    build_function_return(bc_builder);
}

void build_function_call(struct ByteCodeBuilder* bc_builder, int func_id) {
    bc_builder->opcodes[bc_builder->current_builder_location++] = CALL;
    bc_builder->opcodes[bc_builder->current_builder_location++] = get_symbols()[func_id].function.bytecode_address;
    bc_builder->opcodes[bc_builder->current_builder_location++] = get_symbols()[func_id].function.arg_nums;
    bc_builder->opcodes[bc_builder->current_builder_location++] = POP;
}