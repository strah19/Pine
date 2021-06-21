  
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

struct VM {
    struct VMStack stack;
    struct Object* data;
    uint32_t* opcodes;
    uint32_t ip;
    int32_t fp;
};

enum OpCodes {
    IADD,
    ISUB,
    IMUL,
    IDIV,
    IMOD,
    IEQ,
    INEQ,
    ILT,
    IGT,
    ILTE,
    IGTE,
    ICONST,
    POP,
    SYS_WRITE,
    STORE,
    GSTORE,
    LOAD,
    GLOAD,
    CHARCONST,
    HALT,
    JMP,
    JMPT,
    JMPN,
    CALL,
    RET
};

extern struct VMStack vm_create_stack(int size);

extern int vm_push_stack(struct VMStack* stack, struct Object object);

extern struct Object vm_pop_stack(struct VMStack* stack);

extern struct Object vm_peek_stack(struct VMStack* stack);

#endif // !VM_H