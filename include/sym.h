#ifndef SYM_H
#define SYM_H

struct Symbol
{
    char* name;
    float value;
};

extern int FindGlobal(char* name);

extern int AddGlobal(char* name);

extern struct Symbol GetSymbol(char* name);

#endif //!SYM_H