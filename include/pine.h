#ifndef PINE_H
#define PINE_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

extern bool
IsCharDigit ( char character );

extern bool
IsCharGoodForVariableName ( char charcter );

extern int 
StringCompare ( const char *ptr0, const char *ptr1, int len );

#endif // PINE_H