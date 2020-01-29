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
    
    //! The song buffer belonging to this project
    /*! Uncompressed, but you'll need to call a parsing function to get a sensible lsdj_song_t structured object. */
    lsdj_song_buffer_t song;
};

lsdj_project_t* alloc_project(lsdj_error_t** error)
{
    lsdj_project_t* project = (lsdj_project_t*)calloc(sizeof(lsdj_project_t), 1);
    if (project == NULL)
    {
        lsdj_error_optional_new(error, "could not allocate project");
        return NULL;
    }
    
    return project;
}

lsdj_project_t* lsdj_project_new(lsdj_error_t** error)
{
    lsdj_project_t* project = alloc_project(error);
    if (project == NULL)
        return NULL;
    
    memset(project->name, '\0', sizeof(project->name));
    project->version = 0;
    memset(&project->song, 0, sizeof(project->song));
    
    return project;
}

void lsdj_project_free(lsdj_project_t* project)
{
    free(project);
}

void lsdj_project_set_name(lsdj_project_t* project, const char* data, size_t size)
{
    //! @todo: Should I sanitize this name into something LSDj accepts?
    strncpy(project->name, data, size < LSDJ_PROJECT_NAME_LENGTH ? size : LSDJ_PROJECT_NAME_LENGTH);
}

void lsdj_project_get_name(const lsdj_project_t* project, char* data)
{
    const size_t len = strnlen(project->name, LSDJ_PROJECT_NAME_LENGTH);
    strncpy(data, project->name, len);
    for (size_t i = len; i < LSDJ_PROJECT_NAME_LENGTH; i += 1)
        data[i] = '\0';
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

void lsdj_project_set_song_buffer(lsdj_project_t* project, const lsdj_song_buffer_t* song)
{
    memcpy(&project->song, song, sizeof(lsdj_song_buffer_t));
}

const lsdj_song_buffer_t* lsdj_project_get_song_buffer(const lsdj_project_t* project)
{
    return &project->song;
}

// lsdj_project_t* lsdj_project_read_lsdsng(lsdj_vio_t* vio, lsdj_error_t** error)
// {
//     lsdj_project_t* project = alloc_project(error);
    
//     if (vio->read(project->name, LSDJ_PROJECT_NAME_LENGTH, vio->user_data) != LSDJ_PROJECT_NAME_LENGTH)
//     {
//         lsdj_error_new(error, "could not read project name");
//         lsdj_project_free(project);
//         return NULL;
//     }
    
//     if (vio->read(&project->version, 1, vio->user_data) != 1)
//     {
//         lsdj_error_new(error, "could not read project version");
//         lsdj_project_free(project);
//         return NULL;
//     }

//     // Decompress the song data
//     lsdj_memory_access_state_t mem;
//     mem.begin = mem.cur = project->song.bytes;
//     mem.size = sizeof(project->song.bytes);
    
//     lsdj_vio_t wvio;
//     wvio.write = lsdj_mwrite;
//     wvio.tell = lsdj_mtell;
//     wvio.seek = lsdj_mseek;
//     wvio.user_data = &mem;
    
//     lsdj_decompress(vio, &wvio, NULL, BLOCK_SIZE, error);
//     if (error && *error)
//         return NULL;
    
//     return project;
// }

// lsdj_project_t* lsdj_project_read_lsdsng_from_file(const char* path, lsdj_error_t** error)
// {
//     if (path == NULL)
//     {
//         lsdj_error_new(error, "path is NULL");
//         return NULL;
//     }
    
//     FILE* file = fopen(path, "rb");
//     if (file == NULL)
//     {
//         char message[512];
//         snprintf(message, 512, "could not open %s for reading", path);
//         lsdj_error_new(error, message);
//         return NULL;
//     }
    
//     lsdj_vio_t vio;
//     vio.read = lsdj_fread;
//     vio.tell = lsdj_ftell;
//     vio.seek = lsdj_fseek;
//     vio.user_data = file;
    
//     lsdj_project_t* project = lsdj_project_read_lsdsng(&vio, error);
    
//     fclose(file);
    
//     return project;
// }

// lsdj_project_t* lsdj_project_read_lsdsng_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error)
// {
//     if (data == NULL)
//     {
//         lsdj_error_new(error, "data is NULL");
//         return NULL;
//     }
    
//     lsdj_memory_access_state_t mem;
//     mem.begin = (unsigned char*)data;
//     mem.cur = mem.begin;
//     mem.size = size;
    
//     lsdj_vio_t vio;
//     vio.read = lsdj_mread;
//     vio.tell = lsdj_mtell;
//     vio.seek = lsdj_mseek;
//     vio.user_data = &mem;
    
//     return lsdj_project_read_lsdsng(&vio, error);
// }

// int lsdj_project_is_likely_valid_lsdsng(lsdj_vio_t* vio, lsdj_error_t** error)
// {
//     // Check for incorrect input
//     if (vio->tell == NULL)
//     {
//         lsdj_error_new(error, "vio->tell is NULL");
//         return 0;
//     }
    
//     if (vio->seek == NULL)
//     {
//         lsdj_error_new(error, "vio->seek is NULL");
//         return 0;
//     }
    
//     const long begin = vio->tell(vio->user_data);
//     vio->seek(0, SEEK_END, vio->user_data);
    
//     const long size = vio->tell(vio->user_data) - begin;
//     if ((size - LSDJ_PROJECT_NAME_LENGTH - 1) % 0x200 != 0)
//     {
//         lsdj_error_new(error, "data length does not correspond to that of a valid lsdsng");
//         return 0;
//     }
    
//     return 1;
// }

// int lsdj_project_is_likely_valid_lsdsng_file(const char* path, lsdj_error_t** error)
// {
//     if (path == NULL)
//     {
//         lsdj_error_new(error, "path is NULL");
//         return 0;
//     }
    
//     FILE* file = fopen(path, "rb");
//     if (file == NULL)
//     {
//         char message[512];
//         snprintf(message, 512, "could not open %s for reading", path);
//         lsdj_error_new(error, message);
//         return 0;
//     }
    
//     lsdj_vio_t vio;
//     vio.read = lsdj_fread;
//     vio.tell = lsdj_ftell;
//     vio.seek = lsdj_fseek;
//     vio.user_data = file;
    
//     int result = lsdj_project_is_likely_valid_lsdsng(&vio, error);
    
//     fclose(file);
//     return result;
// }

// int lsdj_project_is_likely_valid_lsdsng_memory(const unsigned char* data, size_t size, lsdj_error_t** error)
// {
//     if (data == NULL)
//     {
//         lsdj_error_new(error, "data is NULL");
//         return 0;
//     }
    
//     lsdj_memory_access_state_t mem;
//     mem.begin = (unsigned char*)data;
//     mem.cur = mem.begin;
//     mem.size = size;
    
//     lsdj_vio_t vio;
//     vio.read = lsdj_mread;
//     vio.tell = lsdj_mtell;
//     vio.seek = lsdj_mseek;
//     vio.user_data = &mem;
    
//     return lsdj_project_is_likely_valid_lsdsng(&vio, error);
// }

// size_t lsdj_project_write_lsdsng(const lsdj_project_t* project, lsdj_vio_t* vio, lsdj_error_t** error)
// {
//     size_t write_size = 0;
    
//     write_size += vio->write(project->name, LSDJ_PROJECT_NAME_LENGTH, vio->user_data);
//     if (write_size != LSDJ_PROJECT_NAME_LENGTH)
//     {
//         lsdj_error_new(error, "could not write project name for lsdsng");
//         return write_size;
//     }
    
//     if (vio->write(&project->version, 1, vio->user_data) != 1)
//     {
//         lsdj_error_new(error, "could not write project version for lsdsng");
//         return write_size;
//     }
//     write_size += 1;
    
//     // Compress the song
//     const size_t block_count = lsdj_compress(project->song.bytes, BLOCK_SIZE, 1, BLOCK_COUNT, vio, error);
//     write_size += block_count * BLOCK_SIZE;

//     assert(write_size <= LSDSNG_MAX_SIZE);
//     return write_size;
// }

// size_t lsdj_project_write_lsdsng_to_file(const lsdj_project_t* project, const char* path, lsdj_error_t** error)
// {
//     if (path == NULL)
//     {
//         lsdj_error_new(error, "path is NULL");
//         return 0;
//     }
    
//     if (project == NULL)
//     {
//         lsdj_error_new(error, "project is NULL");
//         return 0;
//     }
    
//     FILE* file = fopen(path, "wb");
//     if (file == NULL)
//     {
//         char message[512];
//         snprintf(message, 512, "could not open %s for writing\n%s", path, strerror(errno));
//         lsdj_error_new(error, message);
//         return 0;
//     }
    
//     lsdj_vio_t vio;
//     vio.write = lsdj_fwrite;
//     vio.tell = lsdj_ftell;
//     vio.seek = lsdj_fseek;
//     vio.user_data = file;
    
//     const size_t write_size = lsdj_project_write_lsdsng(project, &vio, error);
    
//     fclose(file);

//     return write_size;
// }

// size_t lsdj_project_write_lsdsng_to_memory(const lsdj_project_t* project, unsigned char* data, size_t size, lsdj_error_t** error)
// {
//     if (project == NULL)
//     {
//         lsdj_error_new(error, "project is NULL");
//         return 0;
//     }
    
//     if (data == NULL)
//     {
//         lsdj_error_new(error, "data is NULL");
//         return 0;
//     }
    
//     lsdj_memory_access_state_t mem;
//     mem.begin = data;
//     mem.cur = mem.begin;
//     mem.size = size;
    
//     lsdj_vio_t vio;
//     vio.write = lsdj_mwrite;
//     vio.tell = lsdj_mtell;
//     vio.seek = lsdj_mseek;
//     vio.user_data = &mem;
    
//     return lsdj_project_write_lsdsng(project, &vio, error);
// }
