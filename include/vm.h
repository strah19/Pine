#ifndef VM_H
#define VM_H

#include <stdint.h>

struct Object {
    uint8_t type;   //Opcode is a single byte

    union {
        uint32_t u32;
        int32_t i32;
        uint8_t i8;
        uint8_t u8;
        
        void* ptr;
    };
};

struct VMStack {
    int top;
    int size;
    struct Object* stack;
};

enum {
    OP_PUSH_CHAR = 'c',
    OP_SYS_WRITE = 'p',
    OP_PUSH_INT = 'i',
    OP_ADD = 'a',
    OP_HALT = 'h',

    OP_VAR_TYPE_INT,
    OP_VAR_TYPE_USINT
};

extern struct VMStack vm_create_stack(int size);

extern int vm_push_stack(struct VMStack* stack, struct Object object);

extern struct Object vm_pop_stack(struct VMStack* stack);

extern struct Object vm_peek_stack(struct VMStack* stack);

extern void run_vm(uint32_t* buf);

#endif // !VM_H