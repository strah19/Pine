#ifndef SYM_H
#define SYM_H

#include <stdint.h>
#include "../include/token.h"

enum SymType {
    VAR, FUNC, OBJ, ENUM
};

struct VarSym {
    enum TokenType type;
    uint32_t size;
    enum TokenType value_type;
};

struct FuncSym {
    uint32_t bytecode_address;
    uint32_t index;
    uint32_t arg_nums;
    struct VarSym return_info;
    struct VarSym* arg_info;
};

struct Symbol {
    struct Symbol* next;

    //Name is the key
    char* name;

    //Function data or variable is used as the value
    enum SymType type;
    uint32_t id;
    union {
        struct VarSym var;
        struct FuncSym function;
    };
};

struct VariableType {
    char* name;
    enum TokenType type;
    uint32_t size;
    enum TokenType value_type;
};

extern uint32_t search_type_symbol(char* name, enum SymType type);

extern struct Symbol* add_symbol(char* name, enum SymType type);

extern struct Symbol* get_symbol(char* name, enum SymType type);

extern struct Symbol* get_symbols();

extern uint32_t get_sym_index();

extern void update_sym_index(uint32_t index);

extern void log_symbols();

extern struct VariableType* get_variable_types(enum TokenType type);

#endif //!SYM_H