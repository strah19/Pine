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

extern int add_symbol(char* name, float value, enum SymType type);

extern struct Symbol* get_global_symbol(char* name);

extern struct Symbol* get_global_symbol_funcs(char* name);

extern void log_symbols();

extern struct Symbol* get_symbols();

extern int get_sym_index();

extern void update_sym_index(int index);

#endif //!SYM_H