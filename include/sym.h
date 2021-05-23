#ifndef SYM_H
#define SYM_H

struct Symbol {
    char* name;
    float value;
};

extern int find_global_symbol(char* name);

extern int add_symbol(char* name, float value);

extern struct Symbol* get_global_symbol(char* name);

extern void log_symbols();

#endif //!SYM_H