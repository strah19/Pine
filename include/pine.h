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

extern int max(int num1, int num2);

extern int min(int num1, int num2);

#endif // PINE_H