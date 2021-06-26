#ifndef SYM_H
#define SYM_H

#include <stdint.h>

enum SymType {
    VAR, FUNC
};

enum VarTypes {
    TYPE_INT_LIT, TYPE_VOID, TYPE_NONE
};

struct FuncSym {
    uint32_t bytecode_address;
    uint32_t index;
    uint32_t arg_nums;
    enum VarTypes return_type;
    enum VarTypes* arg_types;
};

struct VarSym {
    enum VarTypes type;
    uint32_t index;
    uint32_t size;
};

struct Symbol {
    char* name;
    enum SymType type;

    union {
        struct FuncSym function;
        struct VarSym var;
    };
};

extern uint32_t search_all_symbol(char* name);

extern uint32_t search_type_symbol(char* name, enum SymType type);

extern uint32_t add_symbol(char* name, enum SymType type);

extern struct Symbol* get_symbol(char* name, enum SymType type);

extern struct Symbol* get_symbols();

extern uint32_t get_sym_index();

extern void update_sym_index(uint32_t index);

extern void log_symbols();

#endif //!SYM_H