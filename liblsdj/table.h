/*
 
 This file is a part of liblsdj, a C library for managing everything
 that has to do with LSDJ, software for writing music (chiptune) with
 your gameboy. For more information, see:
 
 * https://github.com/stijnfrishert/liblsdj
 * http://www.littlesounddj.com
 
 --------------------------------------------------------------------------------
 
 MIT License
 
 Copyright (c) 2018 Stijn Frishert
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 */

#ifndef LSDJ_TABLE_H
#define LSDJ_TABLE_H

#include "command.h"

// The default constant length of a table
#define TABLE_LENGTH 16
    
typedef struct
{
    // The volume column of the table
    unsigned char volumes[TABLE_LENGTH];
    
    // The transposition column of the table
    unsigned char transpositions[TABLE_LENGTH];
    
    // The first effect command column of the table
    lsdj_command_t commands1[TABLE_LENGTH];
    
    // The second effect command column of the table
    lsdj_command_t commands2[TABLE_LENGTH];
} lsdj_table_t;

// Copy a table
lsdj_table_t* lsdj_copy_table(const lsdj_table_t* table);
    
// Clear all table data to factory settings
void lsdj_clear_table(lsdj_table_t* table);

#endif
