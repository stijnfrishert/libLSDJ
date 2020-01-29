/*
 
 This file is a part of liblsdj, a C library for managing everything
 that has to do with LSDJ, software for writing music (chiptune) with
 your gameboy. For more information, see:
 
 * https://github.com/stijnfrishert/liblsdj
 * http://www.littlesounddj.com
 
 --------------------------------------------------------------------------------
 
 MIT License
 
 Copyright (c) 2018 - 2020 Stijn Frishert
 
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

#ifndef LSDJ_VIO_H
#define LSDJ_VIO_H

/*! VIO is the Virtual I/O system LibLSDj uses to read data from *anywhere*
	By filling in the lsdj_vio_t struct, users are able to provide their own
	read/write functions + custom data.

	This way, all functions that need to read or write data can so agnostic
	about where they are actually reading/writing. */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>

//! The signature of a virtual I/O read function
typedef size_t (*lsdj_vio_read_t)(void* ptr, size_t size, void* user_data);

//! The signature of a virtual I/O write function
typedef size_t (*lsdj_vio_write_t)(const void* ptr, size_t size, void* user_data);

//! The signature of a virtual I/O tell function
typedef long (*lsdj_vio_tell_t)(void* user_data);

//! The signature of a virtual I/O seek function
typedef long (*lsdj_vio_seek_t)(long offset, int whence, void* user_data);

typedef struct
{
	//! This function is called to read data
    lsdj_vio_read_t read;

    //! This function is called to write data
    lsdj_vio_write_t write;

    //! This function is called to retrieve the current reading position
    lsdj_vio_tell_t tell;

    //! This function is called to change the current reading position
    lsdj_vio_seek_t seek;

    //! Custom data necessary for the functions to do their work
    void* user_data;
} lsdj_vio_t;


// --- File --- //
    
// Virtual I/O read function for file access
size_t lsdj_fread(void* ptr, size_t size, void* user_data);

// Virtual I/O write function for file access
size_t lsdj_fwrite(const void* ptr, size_t size, void* user_data);

// Virtual I/O tell function for file access
long lsdj_ftell(void* user_data);

// Virtual I/O seek function for file access
long lsdj_fseek(long offset, int whence, void* user_data);


// --- Memory --- //

//! Structure used for virtual I/O into memory
/*! You probably won't ever have to use this yourself, *read/write_from_memory() functions
	exist for more data structures, wrapping around this system. */
typedef struct
{
    unsigned char* begin;
    unsigned char* cur;
    size_t size;
} lsdj_memory_access_state_t;

// Virtual I/O read function for memory access
size_t lsdj_mread(void* ptr, size_t size, void* user_data);

// Virtual I/O write function for memory access
size_t lsdj_mwrite(const void* ptr, size_t size, void* user_data);

// Virtual I/O tell function for memory access
long lsdj_mtell(void* user_data);

// Virtual I/O seek function for memory access
long lsdj_mseek(long offset, int whence, void* user_data);
    
#ifdef __cplusplus
}
#endif

#endif
