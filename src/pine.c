/**
 * @file pine.c
 * @author strah19
 * @date May 23 2021
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * @section DESCRIPTION
 *
 * This file contains generic utility functions that can be 
 * used across the project files.
 */

#include "../include/pine.h"

#include <time.h>

bool is_char_digit(char character) {
    return (character >= 48 && character <= 57);
}

bool is_char_good_for_variable_name(char character) {
    return ((character >= 65 && character <= 90) || (character >= 97 && character <= 122)) ? true : false;
}

void remove_whitespaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') 
            ++d;
    } while (*s++ = *d++);
}

int max(int num1, int num2) {
    return (num1 > num2 ) ? num1 : num2;
}

int min(int num1, int num2) {
    return (num1 > num2 ) ? num2 : num1;
}

static clock_t bench_mark_clock;
void begin_debug_benchmark() {
    bench_mark_clock = clock();
}

void end_debug_benchmark(const char* label) {
    clock_t end = clock();
    double time_spent = (double)(end - bench_mark_clock);
    printf("Benchmark time for %s is %f ms.\n", label, time_spent);
}