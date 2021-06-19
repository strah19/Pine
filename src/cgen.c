/**
 * @file cgen.c
 * @author strah19
 * @date June 19 2021
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
 * This file contains code for generating C output code that can 
 * be executed as a binary.
 */

#include "../include/cgen.h"
#include "../include/err.h"

#include <stdio.h>

#define FILE_NAME_LEN 256
#define C_OUT_FILE_MODE "w"
#define C_OUT_FILE_TYPE ".c"

const char* C_preamble_buffer = 
"void __code_gen() {\n";

const char* C_postamble_buffer = 
"}\n"
"int main(int argc, char *argv[]) {\n"
"\t__code_gen();\n"
"\treturn 0; }";

FILE* c_output;
char file_name[FILE_NAME_LEN];

void begin_code_gen(const char* obj_name) {
    memset(file_name, 0, FILE_NAME_LEN);
    strcpy(file_name, obj_name);

    strcat(file_name, C_OUT_FILE_TYPE);
    c_output = fopen(file_name, C_OUT_FILE_MODE);

    if (!c_output) 
        fatal_error("Could not open C-Output file");

    fprintf(c_output, C_preamble_buffer);
}

void finalize_code() {
    fprintf(c_output, C_postamble_buffer);
}

void compile_and_link(const char* obj_name) {
    char cmd_buf[FILE_NAME_LEN] = "gcc ";
    strcat(cmd_buf, file_name);
    strcat(cmd_buf, " -o ");
    strcat(cmd_buf, obj_name);

    fclose(c_output);    
    system(cmd_buf);
}