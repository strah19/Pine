#ifndef PINE_H
#define PINE_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

extern bool is_char_digit(char character);

extern bool is_char_good_for_variable_name(char character);

extern int adv_string_compare(const char *ptr0, const char *ptr1, int len);

extern void remove_whitespaces(char* s);

#endif // PINE_H