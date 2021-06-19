/**
 * @file bytecode_builder.c
 * @author strah19
 * @date June 15 2021
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
 * This file contains code for generating Pine's own bytecode that
 * will be run inside a vm. This will make it is easy for cross
 * platform and can later be converted into native machine code 
 * depending on platform (possibly using LLVM).
 */

#include "../include/bytecode_builder.h"