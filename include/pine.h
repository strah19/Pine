#ifndef PINE_H
#define PINE_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

extern bool is_char_digit(char character);

extern bool is_char_good_for_variable_name(char character, uint32_t token_len);

extern void remove_whitespaces(char* s);

extern int maxim(int num1, int num2);

extern int minim(int num1, int num2);

extern void begin_debug_benchmark();

extern float end_debug_benchmark(const char* label);

#endif // PINE_H