#ifndef SYM_H
#define SYM_H

#include <stdint.h>
#include "../include/token.h"

#define MAX_ARGS 16

enum SymType {
    VAR, FUNC, OBJ, ENUM
};

struct Symbol;

struct VarSym {
    enum TokenType type;
    uint32_t size;
    enum TokenType value_type;
    bool is_const;
    uint32_t id;
};

struct ArgSym {
    struct VarSym var_info;
    char* name;
};

struct FuncSym {
    uint32_t bytecode_address;
    uint32_t arg_nums;
    struct VarSym return_info;
    struct ArgSym arg_info[MAX_ARGS];
    bool created;
};

struct Symbol {
    //Name is the key
    char* name;

    //Function data or variable is used as the value
    enum SymType type;
    uint32_t index;
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

extern void set_current_function(struct FuncSym* func);

extern struct FuncSym* get_function();

#endif //!SYM_H