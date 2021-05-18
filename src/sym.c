#include "../include/sym.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define NSYMBOL 1024
static struct Symbol GSyms[NSYMBOL];
static int GSymIndex = 0;

int FindGlobal(char* name)
{
    for(int i = 0; i < GSymIndex; i++)
    {
        if(strcmp(GSyms[i].name, name) == 0)
            return i;
    }

    return -1;
}

int AddGlobal(char* name)
{
    if(FindGlobal(name ) == -1)
    {
        GSyms[GSymIndex++].name = name;
        GSyms[GSymIndex].value = 0.0;
    }
    else 
    {
        fprintf(stderr, "Error: Symbol '%s' is already defined.\n", name);
        exit(EXIT_FAILURE);
    }

    return GSymIndex;
}

struct Symbol GetSymbol(char* name)
{
    int index = FindGlobal(name);
    if(index != -1)
    {
        return GSyms[index];
    }
    else
    {
        struct Symbol symbol;
        symbol.value = 0.0;
        symbol.name = "";
        return symbol;
    }
}