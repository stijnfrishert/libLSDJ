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

#include "project.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compression.h"

struct lsdj_project_t
{
    //! The name of the project
    char name[LSDJ_PROJECT_NAME_LENGTH];
    
    //! The version of the song
    /*! @note This is a simply song version counter increased with every song save in LSDJ; it has nothing to do with LSDJ versions of the sav format version. */
    unsigned char version;
    
    //! The song belonging to this project
    /*! Uncompressed, but you'll need to call a parsing function to get a sensible lsdj_song_t structured object. */
    lsdj_song_t song;

    //! The allocator used to create this project
    const lsdj_allocator_t* allocator;
};


// --- Allocation --- //

lsdj_project_t* lsdj_project_alloc(const lsdj_allocator_t* allocator, lsdj_error_t** error)
{
    void* memory = lsdj_allocate_or_malloc(allocator, sizeof(lsdj_project_t));;
    if (memory == NULL)
    {
        lsdj_error_optional_new(error, "could not allocate project");
        return NULL;
    }

    lsdj_project_t* project = (lsdj_project_t*)memory;
    project->allocator = allocator;
    
    return project;
}

lsdj_project_t* lsdj_project_new(const lsdj_allocator_t* allocator, lsdj_error_t** error)
{
    lsdj_project_t* project = lsdj_project_alloc(allocator, error);
    if (project == NULL)
    {
        lsdj_error_optional_new(error, "could not allocate memory for project");
        return NULL;
    }
    
    memset(project->name, '\0', sizeof(project->name));
    project->version = 0;
    memset(&project->song, 0, sizeof(lsdj_song_t));
    
    return project;
}

lsdj_project_t* lsdj_project_copy(const lsdj_project_t* project, const lsdj_allocator_t* allocator, lsdj_error_t** error)
{
    lsdj_project_t* copy = lsdj_project_alloc(allocator, error);
    if (copy == NULL)
    {
        lsdj_error_optional_new(error, "could not allocate memory for project");
        return NULL;
    }

    memcpy(copy->name, project->name, sizeof(project->name));
    copy->version = project->version;
    memcpy(&copy->song, &project->song, sizeof(project->song));

    return copy;
}

void lsdj_project_free(lsdj_project_t* project)
{
    if (project)
        lsdj_deallocate_or_free(project->allocator, project);
}


// --- Changing Data --- //

void lsdj_project_set_name(lsdj_project_t* project, const char* data, size_t size)
{
    //! @todo: Should I sanitize this name into something LSDj accepts?
    strncpy(project->name, data, LSDJ_PROJECT_NAME_LENGTH);
}

void lsdj_project_get_name(const lsdj_project_t* project, char* data)
{
    strncpy(data, project->name, LSDJ_PROJECT_NAME_LENGTH);
}

size_t lsdj_project_get_name_length(const lsdj_project_t* project)
{
    return strnlen(project->name, LSDJ_PROJECT_NAME_LENGTH);
}

void lsdj_project_set_version(lsdj_project_t* project, unsigned char version)
{
    project->version = version;
}

unsigned char lsdj_project_get_version(const lsdj_project_t* project)
{
    return project->version;
}

void lsdj_project_set_song(lsdj_project_t* project, const lsdj_song_t* song)
{
    memcpy(&project->song, song, sizeof(lsdj_song_t));
}

lsdj_song_t* lsdj_project_get_song(lsdj_project_t* project)
{
    return &project->song;
}

const lsdj_song_t* lsdj_project_get_song_const(const lsdj_project_t* project)
{
    return &project->song;
}


// --- I/O --- //

lsdj_project_t* lsdj_project_read_lsdsng(lsdj_vio_t* rvio, const lsdj_allocator_t* allocator, lsdj_error_t** error)
{
    lsdj_project_t* project = lsdj_project_alloc(allocator, error);
    if (project == NULL)
        return NULL;
    
    if (!lsdj_vio_read(rvio, project->name, LSDJ_PROJECT_NAME_LENGTH, NULL))
    {
        lsdj_error_optional_new(error, "could not read project name");
        lsdj_project_free(project);
        return NULL;
    }
    
    if (!lsdj_vio_read_byte(rvio, &project->version, NULL))
    {
        lsdj_error_optional_new(error, "could not read project version");
        lsdj_project_free(project);
        return NULL;
    }

    // Decompress the song data
    lsdj_song_t song;
    memset(&song, 0, sizeof(lsdj_song_t));
    
    lsdj_memory_access_state_t state;
    state.begin = state.cur = song.bytes;
    state.size = sizeof(song.bytes);
    
    lsdj_vio_t wvio = lsdj_create_memory_vio(&state);
    
    if (!lsdj_decompress(rvio, NULL, &wvio, NULL, lsdj_vio_tell(rvio), false, error))
    {
        lsdj_project_free(project);
        return NULL;
    }
    
    lsdj_project_set_song(project, &song);
    
    return project;
}

lsdj_project_t* lsdj_project_read_lsdsng_from_file(const char* path, const lsdj_allocator_t* allocator, lsdj_error_t** error)
{
    if (path == NULL)
    {
        lsdj_error_optional_new(error, "path is NULL");
        return NULL;
    }
    
    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        char message[512];
        snprintf(message, 512, "could not open %s for reading", path);
        lsdj_error_optional_new(error, message);
        return NULL;
    }

    lsdj_vio_t rvio = lsdj_create_file_vio(file);
    
    lsdj_project_t* project = lsdj_project_read_lsdsng(&rvio, allocator, error);
    
    fclose(file);
    
    return project;
}

lsdj_project_t* lsdj_project_read_lsdsng_from_memory(const unsigned char* data, size_t size, const lsdj_allocator_t* allocator, lsdj_error_t** error)
{
    if (data == NULL)
    {
        lsdj_error_optional_new(error, "data is NULL");
        return NULL;
    }
    
    lsdj_memory_access_state_t state;
    state.begin = state.cur = (unsigned char*)data;
    state.size = size;

    lsdj_vio_t rvio = lsdj_create_memory_vio(&state);
    
    return lsdj_project_read_lsdsng(&rvio, allocator, error);
}

bool lsdj_project_is_likely_valid_lsdsng(lsdj_vio_t* vio, lsdj_error_t** error)
{
    /*! @todo See if the name is alphanumeric */
    
    // Find out about the file "size"
    const long begin = lsdj_vio_tell(vio);
    if (!lsdj_vio_seek(vio, 0, SEEK_END))
    {
        lsdj_error_optional_new(error, "Could not move to the end of the lsdsng");
        return false;
    }
    
    const long size = lsdj_vio_tell(vio) - begin;
    if (!lsdj_vio_seek(vio, 0, SEEK_SET))
    {
        lsdj_error_optional_new(error, "Could not move to the beginning of the lsdsng");
        return false;
    }

    // Find out if the file size modulo's to the compression block size
    /*! @todo What if someone gives up a buffer bigger than the project size?
        Would still be correct, but not pass this heuristic */
    if ((size - LSDJ_PROJECT_NAME_LENGTH - 1) % LSDJ_BLOCK_SIZE != 0)
    {
        lsdj_error_optional_new(error, "data length does not correspond to that of a valid lsdsng");
        return false;
    }
    
    return true;
}

bool lsdj_project_is_likely_valid_lsdsng_file(const char* path, lsdj_error_t** error)
{
    if (path == NULL)
    {
        lsdj_error_optional_new(error, "path is NULL");
        return false;
    }
    
    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        char message[512];
        snprintf(message, 512, "could not open %s for reading", path);
        lsdj_error_optional_new(error, message);
        return false;
    }
    
    lsdj_vio_t rvio = lsdj_create_file_vio(file);
    
    bool result = lsdj_project_is_likely_valid_lsdsng(&rvio, error);
    
    fclose(file);
    return result;
}

bool lsdj_project_is_likely_valid_lsdsng_memory(const unsigned char* data, size_t size, lsdj_error_t** error)
{
    if (data == NULL)
    {
        lsdj_error_optional_new(error, "data is NULL");
        return 0;
    }
    
    lsdj_memory_access_state_t state;
    state.begin = state.cur = (unsigned char*)data;
    state.size = size;
    
    lsdj_vio_t rvio = lsdj_create_memory_vio(&state);
    
    return lsdj_project_is_likely_valid_lsdsng(&rvio, error);
}

bool lsdj_project_write_lsdsng(const lsdj_project_t* project, lsdj_vio_t* wvio, size_t* writeCounter, lsdj_error_t** error)
{
    // Write the name
    if (!lsdj_vio_write(wvio, project->name, LSDJ_PROJECT_NAME_LENGTH, writeCounter))
    {
        lsdj_error_optional_new(error, "could not write project name for lsdsng");
        return false;
    }
    
    // Write the version
    if (!lsdj_vio_write_byte(wvio, project->version, writeCounter))
    {
        lsdj_error_optional_new(error, "could not write project version for lsdsng");
        return false;
    }
    
    // Compress and write the song buffer
    const lsdj_song_t* song = lsdj_project_get_song_const(project);
    return lsdj_compress(song->bytes, wvio, 1, writeCounter, error);
}

bool lsdj_project_write_lsdsng_to_file(const lsdj_project_t* project, const char* path, size_t* writeCounter, lsdj_error_t** error)
{
    if (path == NULL)
    {
        lsdj_error_optional_new(error, "path is NULL");
        return false;
    }
    
    if (project == NULL)
    {
        lsdj_error_optional_new(error, "project is NULL");
        return false;
    }

    FILE* file = fopen(path, "wb");
    if (file == NULL)
    {
        char message[512];
        snprintf(message, 512, "could not open %s for writing\n%s", path, strerror(errno));
        lsdj_error_optional_new(error, message);
        return false;
    }

    lsdj_vio_t wvio = lsdj_create_file_vio(file);
    const bool result = lsdj_project_write_lsdsng(project, &wvio, writeCounter, error);
    fclose(file);

    return result;
}

bool lsdj_project_write_lsdsng_to_memory(const lsdj_project_t* project, unsigned char* data, size_t* writeCounter, lsdj_error_t** error)
{
    if (project == NULL)
    {
        lsdj_error_optional_new(error, "project is NULL");
        return false;
    }
    
    if (data == NULL)
    {
        lsdj_error_optional_new(error, "data is NULL");
        return false;
    }
    
    lsdj_memory_access_state_t state;
    state.begin = state.cur = data;
    state.size = LSDSNG_MAX_SIZE;
    
    lsdj_vio_t wvio = lsdj_create_memory_vio(&state);
    
    return lsdj_project_write_lsdsng(project, &wvio, writeCounter, error);
}
