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

#ifndef LSDJ_PROJECT_H
#define LSDJ_PROJECT_H

/* Projects are basically song buffers with a name and a version. They are
   used to represent the song slots in a save file (which you can load from
   LSDJ's load/sav/erase screen), as well as loaded in .lsdsng's, because
   they too contain a project name and version. */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "error.h"
#include "song_buffer.h"
#include "vio.h"

//! The length of project names
#define LSDJ_PROJECT_NAME_LENGTH (8)

//! The maximum size of an lsdsng is a version byte +  project name + full song
#define LSDSNG_MAX_SIZE (1 + LSDJ_PROJECT_NAME_LENGTH + LSDJ_SONG_BUFFER_BYTES_COUNT)

//! Representation of a project within an LSDJ sav file, or an imported .lsdsng
typedef struct lsdj_project_t lsdj_project_t;

//! Create a new project
/*! Creates a new project with an empty name, version #0 and empty (factory) song to go
    @note Every call must be paired with an lsdj_project_free()
    @param error If passed a pointer will be filled with sensible data in case of an error */
lsdj_project_t* lsdj_project_new(lsdj_error_t** error);

//! Frees a project from memory
/*! Call this when you no longer need a project.
    @note Every call must be paired with an lsdj_project_new() */
void lsdj_project_free(lsdj_project_t* project);
    
// Deserialize a project from LSDSNG
lsdj_project_t* lsdj_project_read_lsdsng(lsdj_vio_t* vio, lsdj_error_t** error);
lsdj_project_t* lsdj_project_read_lsdsng_from_file(const char* path, lsdj_error_t** error);
lsdj_project_t* lsdj_project_read_lsdsng_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error);
    
// Find out whether given data is likely a valid lsdsng
// Note: this is not a 100% guarantee that the data will load, we're just checking
// some heuristics.
// Returns 0 if invalid, 1 if valid. Error contains information about why.
//
// First version consumes the vio (doesn't seek() back to the beginning)
int lsdj_project_is_likely_valid_lsdsng(lsdj_vio_t* vio, lsdj_error_t** error);
int lsdj_project_is_likely_valid_lsdsng_file(const char* path, lsdj_error_t** error);
int lsdj_project_is_likely_valid_lsdsng_memory(const unsigned char* data, size_t size, lsdj_error_t** error);
    
// Write a project to an lsdsng file
// Returns the number of bytes written
size_t lsdj_project_write_lsdsng(const lsdj_project_t* project, lsdj_vio_t* vio, lsdj_error_t** error);
size_t lsdj_project_write_lsdsng_to_file(const lsdj_project_t* project, const char* path, lsdj_error_t** error);
size_t lsdj_project_write_lsdsng_to_memory(const lsdj_project_t* project, unsigned char* data, size_t size, lsdj_error_t** error);

// Change data in a project
void lsdj_project_set_name(lsdj_project_t* project, const char* data, size_t size);
void lsdj_project_get_name(const lsdj_project_t* project, char* data, size_t size);
void lsdj_project_set_version(lsdj_project_t* project, unsigned char version);
unsigned char lsdj_project_get_version(const lsdj_project_t* project);

// Copy a full song's byte data into the project
void lsdj_project_set_song_memory(lsdj_project_t* project, const lsdj_song_buffer_t* song);

// Extract the byte data of this songs project
const lsdj_song_buffer_t* lsdj_project_get_song_memory(const lsdj_project_t* project);
    
#ifdef __cplusplus
}
#endif

#endif
