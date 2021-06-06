/**
 * @file gen.c
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
 * This file contains code generation that will output into an
 * .asm file for direct compilation into another language.
 */

#include <stdio.h>

#include "../include/err.h"
#include "../include/gen.h"

#define OUTPUT_FILE_MODE "w"

static FILE* output_file;

void initialize_output(const char* file_name) {
    output_file = fopen(file_name, OUTPUT_FILE_MODE);

    if(output_file == NULL)
        fatal_error("could not open output file");
}

void generate_code() {
    fputs("section	.text\n"
        "\tglobal _start\n"	
        "_start:\n", output_file);
}

void close_output() {
    fclose(output_file);
}
