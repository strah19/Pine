#ifndef BYTECODE_BUILDER_H
#define BYTECODE_BUILDER_H

#include "../include/expression.h"

#include <stdint.h>

void begin_code_gen(const char* obj_name);

void finalize_code();

void compile_and_link(const char* obj_name);

#endif // !BYTECODE_BUILDER_H