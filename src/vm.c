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

typedef uint32_t* (*instruction)(uint32_t *, struct VMStack *);

uint32_t* opcode_nop(uint32_t* ip, struct VMStack* stack) {
    return ip + 1;
}

uint32_t* opcode_push_char(uint32_t* ip, struct VMStack* stack) {
    struct Object o;
    o.type = OP_PUSH_CHAR;
    o.u8 = *(ip + 1);
    vm_push_stack(stack, o);
    return ip + 2;
}

uint32_t* opcode_push_int(uint32_t* ip, struct VMStack* stack) {
    struct Object o;
    o.type = OP_VAR_TYPE_INT;
    o.i32 = *(ip + 1);

    vm_push_stack(stack, o);
    return ip + 2;   
}

uint32_t* opcode_add(uint32_t* ip, struct VMStack* stack) {
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
    case OP_VAR_TYPE_INT:
        printf("%d", o.i32);
        break;
    case OP_VAR_TYPE_USINT:
        printf("%u", o.u32);
        break;
    }
}

uint32_t* opcode_sys_write(uint32_t* ip, struct VMStack* stack) {
    struct Object o = vm_pop_stack(stack);
    sys_write_type_call(o);
    return ip + 1;
}

void run_vm(uint32_t* buf) {
    uint32_t* opcodes;
    uint32_t* ip = NULL;
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