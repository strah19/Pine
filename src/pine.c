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

bool is_char_digit(char character) {
    return (character >= 48 && character <= 57);
}

bool is_char_good_for_variable_name(char character) {
    return ((character >= 65 && character <= 90) || (character >= 97 && character <= 122)) ? true : false;
}

int adv_string_compare(const char *ptr0, const char *ptr1, int len) {
    int fast = len / sizeof(size_t) + 1;
    int offset = (fast - 1) * sizeof(size_t);
    int current_block = 0;

    if (len <= sizeof(size_t))
        fast = 0;

    size_t *lptr0 = (size_t *)ptr0;
    size_t *lptr1 = (size_t *)ptr1;

    while (current_block < fast) {
        if ((lptr0[current_block] ^ lptr1[current_block])) {
            int pos;
            for (pos = current_block * sizeof(size_t); pos < len; ++pos) 
                if ((ptr0[pos] ^ ptr1[pos]) || (ptr0[pos] == 0) || (ptr1[pos] == 0))
                    return (int)((unsigned char)ptr0[pos] - (unsigned char)ptr1[pos]);        
        }

        ++current_block;
    }

    while (len > offset)
    {
        if ((ptr0[offset] ^ ptr1[offset]))
            return (int)((unsigned char)ptr0[offset] - (unsigned char)ptr1[offset]);
        ++offset;
    }

    return 0;
}

void remove_whitespaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') 
            ++d;
    } while (*s++ = *d++);
}