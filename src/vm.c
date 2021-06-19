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
#include <ctype.h>

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
    o.type = OP_PUSH_CHAR;
    o.u8 = *(ip + 1);
    vm_push_stack(stack, o);
    return ip + 2;
}

uint8_t* opcode_push_int(uint8_t* ip, struct VMStack* stack) {
    struct Object o;

    uint32_t index_offset = 1;
    if (*(ip + 1) == 'u') {
        o.type = OP_VAR_TYPE_USINT;
        index_offset++;
    }
    else
        o.type = OP_VAR_TYPE_SINT;

    char int_buf[64];
    uint32_t index = 0;

    if (*(ip + index_offset) == '-') 
        index_offset++;

    while(isdigit((int) *(ip + index + index_offset))) 
        int_buf[index++] = *(ip + index + index_offset);

    o.i32 = (index_offset > 1) ? -(atoi(int_buf)) : atoi(int_buf);

    vm_push_stack(stack, o);
    return ip + index + index_offset;   
}

uint8_t* opcode_add(uint8_t* ip, struct VMStack* stack) {
    struct Object o2 = vm_pop_stack(stack);
    struct Object o1 = vm_pop_stack(stack);

    struct Object result;
    result.type = o2.type;
    result.i32 = o1.i32 + o2.i32;

    vm_push_stack(stack, result);
    return ip + 1;
}

void sys_write_type_call(struct Object o) {
    switch(o.type) {
    case OP_VAR_TYPE_SINT:
        printf("%d", o.i32);
        break;
    case OP_VAR_TYPE_USINT:
        printf("%u", o.u32);
        break;
    }
}

uint8_t* opcode_sys_write(uint8_t* ip, struct VMStack* stack) {
    struct Object o = vm_pop_stack(stack);
    sys_write_type_call(o);
    return ip + 1;
}

void run_vm(uint8_t* buf) {
    uint8_t* opcodes;
    uint8_t* ip = NULL;
    instruction ops[256];

    struct VMStack data;

    opcodes = buf;
    data = vm_create_stack(1028);
    ip = opcodes;

    for(int i = 0; i < 256; i++) {
        ops[i] = opcode_nop;
    }

    ops[OP_PUSH_CHAR] = opcode_push_char;
    ops[OP_SYS_WRITE] = opcode_sys_write;
    ops[OP_PUSH_INT] = opcode_push_int;
    ops[OP_ADD] = opcode_add;

    while(*ip != OP_HALT) {
        ip = ops[*ip](ip, &data);
    }

    free(opcodes);
    free(data.stack);    
}