#ifndef BYTECODE_BUILDER_H
#define BYTECODE_BUILDER_H

#include "../include/opcodes.h"

struct ByteCode {
    int opcode;
    int args;
};

#endif // !BYTECODE_BUILDER_H