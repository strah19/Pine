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
#include <string.h>
#include <stdio.h>
#include <ctype.h>

struct VMStack vm_create_stack(int size) {
    struct VMStack stack;

    stack.top = 0;
    stack.size = size;
    stack.stack = (struct Object*)malloc(sizeof(struct Object) * size);

    return stack;
}

struct VM create_vm(uint32_t data_size, uint32_t main) {
    struct VM vm;
    vm.stack = vm_create_stack(1028);
    vm.ip = main;
    vm.data = malloc(sizeof(struct Object) * data_size);
    vm.fp = 0;

    return vm;
}

struct Object obj_push(int32_t val) {
    struct Object o;
    o.i32 = val;
    return o;
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

typedef void (*instruction)(struct VM* vm);

void op_nop(struct VM* vm) {
    vm->ip++;
}

void op_charconst(struct VM* vm) {
    struct Object o;
    o.type = CHARCONST;
    o.u8 = (char) vm->opcodes[vm->ip + 1];
    vm_push_stack(&vm->stack, o);
    vm->ip += 2;
}

void op_iconst(struct VM* vm) {
    struct Object o;

    o.type = ICONST;
    o.i32 = vm->opcodes[vm->ip + 1];
    vm_push_stack(&vm->stack, o);

    vm->ip += 2; 
}

void op_iadd(struct VM* vm) {
    struct Object o2 = vm_pop_stack(&vm->stack);
    struct Object o1 = vm_pop_stack(&vm->stack);

    struct Object result;
    result.type = o2.type;
    result.i32 = o2.i32 + o1.i32;

    vm_push_stack(&vm->stack, result);
    vm->ip++;
}

void op_isub(struct VM* vm) {
    struct Object o2 = vm_pop_stack(&vm->stack);
    struct Object o1 = vm_pop_stack(&vm->stack);

    struct Object result;
    result.type = o2.type;
    result.i32 = o2.i32 - o1.i32;

    vm_push_stack(&vm->stack, result);
    vm->ip++;
}

void op_imul(struct VM* vm) {
    struct Object o2 = vm_pop_stack(&vm->stack);
    struct Object o1 = vm_pop_stack(&vm->stack);

    struct Object result;
    result.type = o2.type;
    result.i32 = o2.i32 * o1.i32;

    vm_push_stack(&vm->stack, result);
    vm->ip++;
}

void op_idiv(struct VM* vm) {
    struct Object o2 = vm_pop_stack(&vm->stack);
    struct Object o1 = vm_pop_stack(&vm->stack);

    struct Object result;
    result.type = o2.type;
    result.i32 = o2.i32 / o1.i32;

    vm_push_stack(&vm->stack, result);
    vm->ip++;
}

void op_syswrite(struct VM* vm) {
    struct Object o = vm_pop_stack(&vm->stack);
   // printf("\n%d\n", o.i32);
    vm->ip++;
}

void op_gload(struct VM* vm) {
    uint32_t addr = vm->opcodes[vm->ip + 1];
    struct Object o = vm->data[addr];

    vm_push_stack(&vm->stack, o);

    vm->ip += 2;
}

void op_gstore(struct VM* vm) {
    struct Object o = vm_pop_stack(&vm->stack);
    uint32_t addr = vm->opcodes[vm->ip + 1];
    vm->data[addr] = o;
    vm->ip += 2;
}

void op_load(struct VM* vm) {
    vm->ip++;
    int32_t offset = vm->opcodes[vm->ip++];
    vm_push_stack(&vm->stack, vm->stack.stack[(vm->fp - 1) + offset]);
}

void op_store(struct VM* vm) {
    vm->ip++;
    int32_t offset = vm->opcodes[vm->ip++];
    vm->stack.stack[(vm->fp - 1) + offset] = vm->stack.stack[vm->stack.top--];
}

void op_jmp(struct VM* vm) {
    vm->ip = vm->opcodes[vm->ip + 1];
}

void op_jmpt(struct VM* vm) {
    struct Object o = vm_pop_stack(&vm->stack);
    if (o.i32) vm->ip = vm->opcodes[vm->ip + 1];
}

void op_jmpn(struct VM* vm) {
    struct Object o = vm_pop_stack(&vm->stack);
    if (!o.i32) vm->ip = vm->opcodes[vm->ip + 1];
}

void op_call(struct VM* vm) {
    struct Object address;
    struct Object num_args;

    vm->ip++;
    address.i32 = vm->opcodes[vm->ip];
    vm->ip++;
    num_args.i32 = vm->opcodes[vm->ip];

    vm_push_stack(&vm->stack, num_args);

    vm_push_stack(&vm->stack, obj_push(vm->fp));
    vm_push_stack(&vm->stack, obj_push(vm->ip));

    vm->fp = vm->stack.top;
    vm->ip = address.i32;
}

void op_ret(struct VM* vm) {
    struct Object ret = vm_pop_stack(&vm->stack);

    vm->stack.top = vm->fp;
    vm->ip = vm_pop_stack(&vm->stack).i32;
    vm->fp = vm_pop_stack(&vm->stack).i32;
    int32_t args = vm_pop_stack(&vm->stack).i32;
    vm->stack.top -= args;
    vm_push_stack(&vm->stack, ret);

    vm->ip++;
}

int main(int argc, char **argv) {
    uint32_t opcodes[] = {
        LOAD, -3,
        LOAD, -4,
        IDIV,
        SYS_WRITE,
        CALL, 13, 0,
        POP,
        ICONST, 80,
        RET,

        ICONST, 3,
        SYS_WRITE,
        ICONST, 0,
        RET,
        
        ICONST, 15, 
        ICONST, 5, 
        CALL, 0, 2,
        SYS_WRITE,
        HALT        // stop program
    };

    struct VM vm = create_vm(1, 19);
    vm.opcodes = opcodes;

    instruction ops[256];
    for(int i = 0; i < 256; i++) {
        ops[i] = op_nop;
    }

    ops[CHARCONST] = op_charconst;
    ops[SYS_WRITE] = op_syswrite;
    ops[ICONST] = op_iconst;
    ops[IADD] = op_iadd;
    ops[ISUB] = op_isub;
    ops[IMUL] = op_imul;
    ops[IDIV] = op_idiv;
    ops[GLOAD] = op_gload;
    ops[GSTORE] = op_gstore;
    ops[JMP] = op_jmp;
    ops[JMPT] = op_jmpt;
    ops[JMPN] = op_jmpn;
    ops[CALL] = op_call;
    ops[RET] = op_ret;
    ops[LOAD] = op_load;

    while(vm.opcodes[vm.ip] != HALT) {
        printf("%04x:\t%d\t", vm.ip, vm.opcodes[vm.ip]);
        ops[vm.opcodes[vm.ip]](&vm);

        printf("[ ");
        for(int i = 0; i < vm.stack.top; i++){
            printf("%d ", vm.stack.stack[i].i32);
        }
        printf("]\n");
    }    

    free(vm.stack.stack);
    free(vm.data);
}