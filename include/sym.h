#ifndef SYM_H
#define SYM_H

enum SymType {
    VAR, FUNC
};

enum VarTypes {
    TYPE_INT_LIT, TYPE_VOID, TYPE_NONE
};

struct Symbol {
    char* name;
    float value;
    enum SymType type;
    enum VarTypes var_type;
};

extern int find_global_symbol(char* name);

extern int add_symbol(char* name, float value);

extern struct Symbol* get_global_symbol(char* name);

extern void log_symbols();

extern struct Symbol* get_symbols();

#endif //!SYM_H