/**
 * @file vm.c
 * @author strah19
 * @date June 16 2021
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
 * This file contains code for running our Pine bytecode from a file.
 */

#include "../include/vm.h"

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct VMStack vm_create_stack(int size) {
    struct VMStack stack;

    stack.top = 0;
    stack.size = size;
    stack.stack = (struct Object*)malloc(sizeof(struct Object) * size);

    return stack;
}

int vm_push_stack(struct VMStack* stack, struct Object object) {
    stack->stack[stack->top++] = object;
    return stack->top;
}

struct Object vm_pop_stack(struct VMStack* stack) {
    return stack->stack[--(stack->top)];
}

struct Object vm_peek_stack(struct VMStack* stack) {
    return stack->stack[stack->top];
}

uint8_t* load_bytecode(const char* filepath) {
    FILE* file;
    int size;

    uint8_t* opcodes = NULL;

    if(file = fopen(filepath, "r")) {
        struct stat st;

        if (fstat(fileno(file), &st) != -1) {
            opcodes = (uint8_t *) malloc(st.st_size);
            fread((void*) opcodes, 1, st.st_size, file);
        }
        else {
            printf("Failed to load stats of bytecode file '%s'.\n", filepath);
            exit(EXIT_FAILURE);
        }
    }
    else {
        printf("Failed to load in bytecode file '%s'.\n", filepath);
        exit(EXIT_FAILURE);
    }

    return opcodes;
}   

typedef uint8_t* (*instruction)(uint8_t *, struct VMStack *);

uint8_t* opcode_nop(uint8_t* ip, struct VMStack* stack) {
    return ip + 1;
}

uint8_t* opcode_push_char(uint8_t* ip, struct VMStack* stack) {
    struct Object o;
    o.type = 'c';
    o.u8 = *(ip + 1);
    vm_push_stack(stack, o);
    return ip + 2;
}

uint8_t* opcode_sysout(uint8_t* ip, struct VMStack* stack) {
    struct Object o = vm_pop_stack(stack);
    putchar(o.u8);
    return ip + 1;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Must enter a file for the pvm to run.\n");
        exit(EXIT_FAILURE);
    }

    uint8_t* opcodes;
    uint8_t* ip = NULL;
    instruction ops[256];

    struct VMStack data;

    opcodes = load_bytecode(argv[1]);
    data = vm_create_stack(1028);
    ip = opcodes;

    for(int i = 0; i < 256; i++) {
        ops[i] = opcode_nop;
    }

    ops[OP_PUSH_CHAR] = opcode_push_char;
    ops[OP_SYS_WRITE] = opcode_sysout;

    while(*ip != OP_HALT) {
        ip = ops[*ip](ip, &data);
    }

    free(opcodes);
    free(data.stack);
}