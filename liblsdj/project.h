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

#include <stdbool.h>
#include <stddef.h>

#include "allocator.h"
#include "error.h"
#include "song_buffer.h"
#include "vio.h"

//! The length of project names
#define LSDJ_PROJECT_NAME_LENGTH (8)

//! The maximum size of an lsdsng is a version byte +  project name + full song
#define LSDSNG_MAX_SIZE (1 + LSDJ_PROJECT_NAME_LENGTH + LSDJ_SONG_BUFFER_BYTE_COUNT)

//! Representation of a project within an LSDJ sav file, or an imported .lsdsng
typedef struct lsdj_project_t lsdj_project_t;


// --- Allocation --- //

//! Create a new project
/*! Creates a new project with an empty name, version #0 and zeroed out song

    @param allocator The allocator (or null) used for memory (de)allocation
    @note Every call must be paired with an lsdj_project_free() */
lsdj_project_t* lsdj_project_new(const lsdj_allocator_t* allocator, lsdj_error_t** error);

//! Copy a project into a new project
/*! Creates a new project and copies the data into it.

    @param allocator The allocator (or null) used for memory (de)allocation
	@note Every call must be paired with an lsdj_project_free() */
lsdj_project_t* lsdj_project_copy(const lsdj_project_t* project, const lsdj_allocator_t* allocator, lsdj_error_t** error);

//! Frees a project from memory
/*! Call this when you no longer need a project. */
void lsdj_project_free(lsdj_project_t* project);


// --- Changing data --- //

//! Change the name of a project
/*! @param data A pointer to char data
	@param size The length of the name sent in (maximum LSDJ_PROJECT_NAME_LENGTH) */
void lsdj_project_set_name(lsdj_project_t* project, const char* data, size_t size);

//! Retrieve the name of this project
/*! @param data A pointer to char data of at least LSDJ_PROJECT_NAME_LENGTH long */
void lsdj_project_get_name(const lsdj_project_t* project, char* data);

//! Retrieve the length of a project's name
/*! This won't ever be larger than LSDJ_PROJECT_NAME_LENGTH */
size_t lsdj_project_get_name_length(const lsdj_project_t* project);

//! Change the version number of a project
/*! @note This has nothing to do with your LSDj or format version, it's just a project version */
void lsdj_project_set_version(lsdj_project_t* project, unsigned char version);

//! Retrieve the version number of the project
/*! @note This has nothing to do with your LSDj or format version, it's just a project version */
unsigned char lsdj_project_get_version(const lsdj_project_t* project);

//! Copy a full song's byte data into the project
/*! A song buffer's data is copied into the project.
	This leaves the original song buffer intact */
void lsdj_project_set_song_buffer(lsdj_project_t* project, const lsdj_song_buffer_t* songBuffer);

//! Retrieve the song buffer for this project
/*! Song buffers contain the actual song data for a project. */
const lsdj_song_buffer_t* lsdj_project_get_song_buffer(const lsdj_project_t* project);


// --- I/O --- //

//! Read an LSDJ Project from an .lsdsng I/O streeam
/*! This function uses liblsdj's virtual I/O system. There are other convenience functions to
	directly read from memory or file.

	@return The project (or NULL in case of an error) which you need to call lsdj_project_free() on */
lsdj_project_t* lsdj_project_read_lsdsng(lsdj_vio_t* rvio, const lsdj_allocator_t* allocator, lsdj_error_t** error);

//! Read an LSDJ Project from an .lsdsng file
/*! @return The project (or NULL in case of an error) which you need to call lsdj_project_free() on */
lsdj_project_t* lsdj_project_read_lsdsng_from_file(const char* path, const lsdj_allocator_t* allocator, lsdj_error_t** error);

//! Read an LSDJ Project from an .lsdsng in memory
/*! @return The project (or NULL in case of an error) which you need to call lsdj_project_free() on */
lsdj_project_t* lsdj_project_read_lsdsng_from_memory(const unsigned char* data, size_t size, const lsdj_allocator_t* allocator, lsdj_error_t** error);
    
//! Find out whether given data is likely a valid lsdsng
/*! @note This is not a 100% guarantee that the data will load, we're just checking some heuristics. */
bool lsdj_project_is_likely_valid_lsdsng(lsdj_vio_t* vio, lsdj_error_t** error);

//! Find out whether a file is likely a valid lsdsng
/*! @note This is not a 100% guarantee that the data will load, we're just checking some heuristics. */
bool lsdj_project_is_likely_valid_lsdsng_file(const char* path, lsdj_error_t** error);

//! Find out whether a memory address likely contains a valid lsdsng
/*! @note This is not a 100% guarantee that the data will load, we're just checking some heuristics. */
bool lsdj_project_is_likely_valid_lsdsng_memory(const unsigned char* data, size_t size, lsdj_error_t** error);
    
//! Write a project to an .lsdsng I/O stream
/*! This function uses liblsdj's virtual I/O system. There are other convenience functions to
	directly write to memory or file.
 
    @param project The project to be written to stream
    @param vio The virtual stream into which the project is written
    @param writeCounter The amount of bytes written is _added_ to this value, if provided (you should initialize this)
    @param error A description of the error that occured, if provided

	@return Whether the write was successful */
bool lsdj_project_write_lsdsng(const lsdj_project_t* project, lsdj_vio_t* vio, size_t* writeCounter, lsdj_error_t** error);

//! Write a project to file
/*! @param project The project to be written to file
    @param path The path to the file where the lsdsng should be written on disk
    @param writeCounter The amount of bytes written is _added_ to this value, if provided (you should initialize this)
    @param error A description of the error that occured, if provided

    @return Whether the write was successful */
bool lsdj_project_write_lsdsng_to_file(const lsdj_project_t* project, const char* path, size_t* writeCounter, lsdj_error_t** error);

//! Write a project to a memory
/*! @param project The project to be written to memory
    @param data Pointer to the write buffer, should be at least LSDSNG_MAX_SIZE in size
    @param writeCounter The amount of bytes written is _added_ to this value, if provided (you should initialize this)
    @param error A description of the error that occured, if provided

    @return Whether the write was successful */
bool lsdj_project_write_lsdsng_to_memory(const lsdj_project_t* project, unsigned char* data, size_t* writeCounter, lsdj_error_t** error);
    
#ifdef __cplusplus
}
#endif

#endif
