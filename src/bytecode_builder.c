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

uint32_t calculate_bin_operator(struct ByteCodeBuilder* bc_builder, struct ASTNode* bin) {
    if (bin && bin->left && bin->right) {
        switch(bin->op) {
        case ADD:
            return IADD;
        case SUBTRACT:
            return ISUB;
        case MULTIPLE:
            return IMUL;
        case DIVIDE:
            return IDIV;
        case DOUBLE_EQUAL:
            return IEQ;
        case OR:
            return IOR;
        case AND:
            return IAND;
        case LESS_THAN:
            return ILT;
        case GREATER_THAN:
            return IGT;
        case LESS_THAN_EQUAL:
            return ILTE;
        case GREATER_THAN_EQUAL:
            return IGTE;
        case NOT:
            return INEQ;
    }
    }

    return HALT;
}

void push_iconst(struct ByteCodeBuilder* bc_builder, int val) {
    bc_builder->opcodes[bc_builder->current_builder_location++] = ICONST;
    bc_builder->opcodes[bc_builder->current_builder_location++] = val; 
}

void global_load(struct ByteCodeBuilder* bc_builder, int var_id) {
    bc_builder->opcodes[bc_builder->current_builder_location++] = GLOAD;
    bc_builder->opcodes[bc_builder->current_builder_location++] = var_id;    
}

void bc_equal(struct ByteCodeBuilder* bc_builder, struct ASTNode* root) {
    if (!root) return;

    bc_equal(bc_builder, root->left);
    bc_equal(bc_builder, root->right);

    if (root->op != EQUAL) {
        if (root->op == INTEGER && root->parent) {
            if (root->parent->op == EQUAL && root != root->parent->left) 
                push_iconst(bc_builder, root->int_val);
        }
        else if (root->op == ID && root->parent) {
            if (root->parent->op == EQUAL && root != root->parent->left) 
                global_load(bc_builder, root->var_id);
        }
        else if (root->left != NULL && root->right != NULL) {
            if (root->left->op == ID && root->right->op == ID) {
                if (root->left->order < root->right->order) {
                    global_load(bc_builder, root->right->var_id);
                    global_load(bc_builder, root->left->var_id);
                }
                else {
                    global_load(bc_builder, root->left->var_id);
                    global_load(bc_builder, root->right->var_id); 
                }

                bc_builder->opcodes[bc_builder->current_builder_location++] = calculate_bin_operator(bc_builder, root);
            }
            else if(root->right->op == ID) {
                if (root->left->order < root->right->order) {
                    global_load(bc_builder, root->right->var_id);
                    push_iconst(bc_builder, root->left->int_val);
                }
                else {
                    push_iconst(bc_builder, root->left->int_val);
                    global_load(bc_builder, root->right->var_id);                   
                }
                bc_builder->opcodes[bc_builder->current_builder_location++] = calculate_bin_operator(bc_builder, root);
            }
            else if(root->left->op == ID) {
                if (root->left->order < root->right->order) {
                    push_iconst(bc_builder, root->right->int_val);
                    global_load(bc_builder, root->left->var_id);
                }
                else {
                    global_load(bc_builder, root->left->var_id);      
                    push_iconst(bc_builder, root->right->int_val);
                }
                bc_builder->opcodes[bc_builder->current_builder_location++] = calculate_bin_operator(bc_builder, root);
            }
            else {
                if (root->op != AND && root->op != OR) {
                    root->int_val = run_bin_exp(root);
                    push_iconst(bc_builder, root->int_val);
                }
            }
        }
    }
    else {
        bc_builder->opcodes[bc_builder->current_builder_location++] = GSTORE;
        bc_builder->opcodes[bc_builder->current_builder_location++] = root->left->var_id;
    }
}

void bc_decleration(struct ByteCodeBuilder* bc_builder, struct ASTNode* root) {
    bc_equal(bc_builder, root);
    analyize_opcode_storage(bc_builder);
}

uint32_t beg_if_statement(struct ByteCodeBuilder* bc_builder, struct ASTNode* comparative_statement) {
    bc_builder->opcodes[bc_builder->current_builder_location++] = JMPN;
    uint32_t ref = bc_builder->current_builder_location;
    bc_builder->opcodes[bc_builder->current_builder_location++] = -1;

    return ref;
}