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

#include "sav.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compression.h"
#include "song_buffer.h"

#define LSDJ_SAV_PROJECT_COUNT 32
#define HEADER_START LSDJ_SONG_BUFFER_BYTE_COUNT

//! Representation of an entire LSDj save state
struct lsdj_sav_t
{
    //! The song in active working memory
    lsdj_song_buffer_t workingMemorySongBuffer;

    //! The project slots
    /*! If one of these is NULL, it means the slot isn't used by the sav */
    lsdj_project_t* projects[LSDJ_SAV_PROJECT_COUNT];
    
    //! Index of the project that is currently being edited
    /*! Indices start at 0, a value of LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX means there is no active project */
    unsigned char activeProjectIndex;
    
    //! Reserved empty memory
    unsigned char reserved8120[30];
};

typedef struct
{
	char project_names[LSDJ_SAV_PROJECT_COUNT * 8];
	unsigned char versions[LSDJ_SAV_PROJECT_COUNT * 1];
	unsigned char empty[30];
	char init[2];
	unsigned char active_project;
} header_t;

//! Allocate a sav in memory
lsdj_sav_t* lsdj_sav_alloc(lsdj_error_t** error)
{
    lsdj_sav_t* sav = (lsdj_sav_t*)calloc(sizeof(lsdj_sav_t), 1);
    if (sav == NULL)
    {
        lsdj_error_optional_new(error, "could not allocate sav");
        return NULL;
    }
    
    return sav;
}

lsdj_sav_t* lsdj_sav_new(lsdj_error_t** error)
{
    // Allocate memory for a new sav
    lsdj_sav_t* sav = lsdj_sav_alloc(error);
    if (sav == NULL)
        return NULL;

    // Clear the working memory song buffer
    memset(&sav->workingMemorySongBuffer, 0, sizeof(sav->workingMemorySongBuffer));
    
    // Clear the projects
    for (int i = 0; i < LSDJ_SAV_PROJECT_COUNT; ++i)
    {
        sav->projects[i] = NULL;
    }
    
    // None of the project slots is active
    sav->activeProjectIndex = LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX;
    
    return sav;
}

void lsdj_sav_free(lsdj_sav_t* sav)
{
    if (sav)
    {
        for (int i = 0; i < LSDJ_SAV_PROJECT_COUNT; ++i)
        {
            if (sav->projects[i])
                lsdj_project_free(sav->projects[i]);
        }
        
        free(sav);
    }
}

void lsdj_sav_set_working_memory_song_buffer(lsdj_sav_t* sav, const lsdj_song_buffer_t* songBuffer)
{
    memcpy(&sav->workingMemorySongBuffer, &songBuffer, sizeof(songBuffer));
}

const lsdj_song_buffer_t* lsdj_sav_get_working_memory_song_buffer(const lsdj_sav_t* sav)
{
    return &sav->workingMemorySongBuffer;
}

// void lsdj_sav_set_working_memory_song_from_project(lsdj_sav_t* sav, unsigned char index, lsdj_error_t** error)
// {
//     lsdj_project_t* project = sav->projects[index];
//     if (project == NULL)
//         return lsdj_error_new(error, "no active project at given index");

//     lsdj_song_buffer_t* song = lsdj_project_get_song_memory(project);
//     assert(song != NULL);
    
//     lsdj_sav_set_working_memory_song_memory(sav, song, index);
// }

void lsdj_sav_set_active_project_index(lsdj_sav_t* sav, unsigned char index)
{
    sav->activeProjectIndex = index;
}

unsigned char lsdj_sav_get_active_project_index(const lsdj_sav_t* sav)
{
    return sav->activeProjectIndex;
}

// lsdj_project_t* lsdj_project_new_from_working_memory_song(const lsdj_sav_t* sav, lsdj_error_t** error)
// {
//     // Try and copy the song
//     lsdj_song_buffer_t* song = lsdj_sav_get_working_memory_song_memory(sav);
    
//     lsdj_project_t* newProject = lsdj_project_new(error);
//     if (error && *error)
//         return NULL;
    
//     unsigned char active = lsdj_sav_get_active_project(sav);
    
//     char name[9];
//     memset(name, '\0', 9);
//     unsigned char version = 0;
//     if (active != LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX)
//     {
//         lsdj_project_t* oldProject = lsdj_sav_get_project(sav, active);
//         lsdj_project_get_name(oldProject, name, 8);
//         version = lsdj_project_get_version(oldProject);
//     }
    
//     lsdj_project_set_song(newProject, copy);
//     lsdj_project_set_name(newProject, name, 8);
//     lsdj_project_set_version(newProject, version);
    
//     return newProject;
// }

// unsigned int lsdj_sav_get_project_count(const lsdj_sav_t* sav)
// {
//     return LSDJ_SAV_PROJECT_COUNT;
// }

// void lsdj_sav_set_project(lsdj_sav_t* sav, unsigned char index, lsdj_project_t* project, lsdj_error_t** error)
// {
//     if (project == NULL)
//         return lsdj_error_new(error, "project is NULL");
    
//     lsdj_project_free(sav->projects[index]);
//     sav->projects[index] = project;
// }

// void lsdj_sav_erase_project(lsdj_sav_t* sav, unsigned char index, lsdj_error_t** error)
// {
//     lsdj_project_free(sav->projects[index]);
//     sav->projects[index] = lsdj_project_new(error);
// }

// lsdj_project_t* lsdj_sav_get_project(const lsdj_sav_t* sav, unsigned char project)
// {
//     return sav->projects[project];
// }

// // Read compressed project data from memory sav file
// void read_compressed_blocks(lsdj_vio_t* vio, lsdj_project_t** projects, lsdj_error_t** error)
// {
//     // Read the block allocation table
//     unsigned char blocks_alloc_table[BLOCK_COUNT];
//     if (vio->read(blocks_alloc_table, sizeof(blocks_alloc_table), vio->user_data) != sizeof(blocks_alloc_table))
//         return lsdj_error_new(error, "could not read block allocation table");
    
//     // Pointers for storing decompressed song data
//     // Handle decompression
//     for (int i = 0; i < BLOCK_COUNT; ++i)
//     {
//         unsigned char p = blocks_alloc_table[i];
//         if (p == 0xFF)
//             continue;
        
//         lsdj_project_t* project = projects[p];
//         if (lsdj_project_get_song(project) != NULL)
//             continue;
        
//         lsdj_song_buffer_t data;
//         memset(data.bytes, 0x00, sizeof(data));
        
//         vio->seek(HEADER_START + (i + 1) * BLOCK_SIZE, SEEK_SET, vio->user_data);
                
//         lsdj_memory_access_state_t mem;
//         mem.cur = mem.begin = data.bytes;
//         mem.size = sizeof(data);
        
//         long block1position = HEADER_START + BLOCK_SIZE;
        
//         lsdj_vio_t wvio;
//         wvio.write = lsdj_mwrite;
//         wvio.tell = lsdj_mtell;
//         wvio.seek = lsdj_mseek;
//         wvio.user_data = &mem;
//         lsdj_decompress(vio, &wvio, &block1position, BLOCK_SIZE, error);
//         if (error && *error)
//             return;
        
//         // Read the song from memory
//         lsdj_song_t* song = lsdj_song_read_from_memory(data.bytes, sizeof(data.bytes), error);
//         if (error && *error)
//         {
//             lsdj_song_free(song);
//             return;
//         }
        
//         lsdj_project_set_song(project, song);
//     }
// }

// lsdj_sav_t* lsdj_sav_read(lsdj_vio_t* vio, lsdj_error_t** error)
// {
//     // Check for incorrect input
//     if (vio->read == NULL)
//     {
//         lsdj_error_new(error, "vio->read is NULL");
//         return NULL;
//     }
    
//     if (vio->seek == NULL)
//     {
//         lsdj_error_new(error, "vio->seek is NULL");
//         return NULL;
//     }
    
//     if (vio->tell == NULL)
//     {
//         lsdj_error_new(error, "vio->tell is NULL");
//         return NULL;
//     }
    
//     lsdj_sav_t* sav = lsdj_sav_alloc(error);
//     if (sav == NULL)
//         return NULL;
    
//     // Skip memory representing the working song (we'll get to that)
//     const long begin = vio->tell(vio->user_data);
//     if (begin == -1L)
//     {
//         lsdj_error_new(error, "could not tell begin of sav read");
//         lsdj_sav_free(sav);
//         return NULL;
//     }
//     vio->seek(begin + HEADER_START, SEEK_SET, vio->user_data);
    
//     // Read the header block, before we start processing each song
// 	header_t header;
// 	vio->read(&header, sizeof(header), vio->user_data);
    
//     // Check the initialization characters. If they're not 'jk', we're
//     // probably not dealing with an actual LSDJ sav format file.
//     if (header.init[0] != 'j' || header.init[1] != 'k')
//     {
//         lsdj_sav_free(sav);
//         lsdj_error_new(error, "SRAM initialization check wasn't 'jk'");
//         return NULL;
//     }
    
//     // Allocate data for all the projects and store their names
//     for (int i = 0; i < LSDJ_SAV_PROJECT_COUNT; ++i)
//     {
//         lsdj_project_t* project = lsdj_project_new(error);
//         if (error && *error)
//         {
//             lsdj_sav_free(sav);
//             return NULL;
//         }
        
//         lsdj_project_set_name(project, &header.project_names[i * 8], 8);
//         lsdj_project_set_version(project, header.versions[i]);
        
//         sav->projects[i] = project;
//     }
    
//     // Store the active project index
//     sav->activeProjectIndex = header.active_project;
    
//     // Store the reserved empty memory at 0x8120
//     // Not sure what's really in there, but might as well keep it intact
//     memcpy(sav->reserved8120, header.empty, sizeof(sav->reserved8120));
    
//     // Read the compressed projects
//     read_compressed_blocks(vio, sav->projects, error);
//     if (error && *error)
//     {
//         lsdj_sav_free(sav);
//         return NULL;
//     }
    
//     // Read the working song
//     const long end = vio->tell(vio->user_data);
//     if (end == -1L)
//     {
//         lsdj_error_new(error, "could not tell end of sav read");
//         lsdj_sav_free(sav);
//         return NULL;
//     }
    
//     vio->seek(begin, SEEK_SET, vio->user_data);
//     lsdj_song_buffer_t song_data;
//     if (vio->read(song_data.bytes, sizeof(song_data.bytes), vio->user_data) != sizeof(song_data.bytes))
//     {
//         lsdj_sav_free(sav);
//         lsdj_error_new(error, "could not read compressed song data");
//         return NULL;
//     }
    
//     sav->song = lsdj_song_new(error);
//     if (error && *error)
//     {
//         lsdj_sav_free(sav);
//         return NULL;
//     }
    
//     sav->song = lsdj_song_read_from_memory(song_data.bytes, sizeof(song_data.bytes), error);
    
//     vio->seek(end, SEEK_SET, vio->user_data);
    
//     return sav;
// }

// lsdj_sav_t* lsdj_sav_read_from_file(const char* path, lsdj_error_t** error)
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

//     lsdj_sav_t* sav = lsdj_sav_read(&vio, error);
    
//     fclose(file);
//     return sav;
// }

// lsdj_sav_t* lsdj_sav_read_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error)
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
    
//     return lsdj_sav_read(&vio, error);
// }

// int lsdj_sav_is_likely_valid(lsdj_vio_t* vio, lsdj_error_t** error)
// {
//     // Check for incorrect input
//     if (vio->read == NULL)
//     {
//         lsdj_error_new(error, "vio->read is NULL");
//         return 0;
//     }
    
//     if (vio->seek == NULL)
//     {
//         lsdj_error_new(error, "vio->seek is NULL");
//         return 0;
//     }
    
//     // Move to the initialization bytes
//     vio->seek(HEADER_START + 0x13E, SEEK_CUR, vio->user_data);
    
//     // Ensure these bytes are 'jk', that's what LSDJ sets them to on RAM init
//     char buffer[2];
//     vio->read(buffer, sizeof(buffer), vio->user_data);
    
//     if (buffer[0] != 'j' || buffer[1] != 'k')
//     {
//         lsdj_error_new(error, "Memory 0x813E isn't 'jk'");
//         return 0;
//     }
    
//     return 1;
// }

// int lsdj_sav_is_likely_valid_file(const char* path, lsdj_error_t** error)
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
    
//     int result = lsdj_sav_is_likely_valid(&vio, error);
    
//     fclose(file);
//     return result;
// }

// int lsdj_sav_is_likely_valid_memory(const unsigned char* data, size_t size, lsdj_error_t** error)
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
    
//     return lsdj_sav_is_likely_valid(&vio, error);
// }

// void lsdj_sav_write(const lsdj_sav_t* sav, lsdj_vio_t* vio, lsdj_error_t** error)
// {
//     // Write the working project
//     lsdj_song_buffer_t song_data;
//     lsdj_song_write_to_memory(sav->song, song_data.bytes, sizeof(song_data.bytes), error);
//     if (vio->write(song_data.bytes, sizeof(song_data.bytes), vio->user_data) != sizeof(song_data.bytes))
//         return lsdj_error_new(error, "could not write compressed song data");

//     // Create the header for writing
//     header_t header;
//     memset(&header, 0, sizeof(header));
//     header.init[0] = 'j';
//     header.init[1] = 'k';
//     header.active_project = sav->activeProjectIndex;
//     memcpy(header.empty, sav->reserved8120, sizeof(sav->reserved8120));

//     // Create the block allocation table for writing
//     unsigned char block_alloc_table[BLOCK_COUNT];
//     memset(&block_alloc_table, 0xFF, sizeof(block_alloc_table));
//     unsigned char* table_ptr = block_alloc_table;

//     // Write project specific data
//     unsigned char blocks[BLOCK_COUNT][BLOCK_SIZE];
    
//     unsigned char current_block = 1;
//     memset(blocks, 0, sizeof(blocks));
    
//     for (int i = 0; i < LSDJ_SAV_PROJECT_COUNT; ++i)
//     {
//         lsdj_project_t* project = sav->projects[i];
        
//         // Write project name
//         char name[LSDJ_PROJECT_NAME_LENGTH];
//         lsdj_project_get_name(project, name, LSDJ_PROJECT_NAME_LENGTH);
//         strncpy(&header.project_names[i * 8], name, LSDJ_PROJECT_NAME_LENGTH < 8 ? LSDJ_PROJECT_NAME_LENGTH : 8);
        
//         // Write project version
//         header.versions[i] = lsdj_project_get_version(project);
        
//         lsdj_song_t* song = lsdj_project_get_song(project);
//         if (song)
//         {
//             // Compress the song to memory
//             lsdj_song_buffer_t song_data;
//             lsdj_song_write_to_memory(song, song_data.bytes, sizeof(song_data.bytes), error);
            
//             lsdj_memory_access_state_t mem;
//             mem.cur = mem.begin = blocks[current_block - 1];
//             mem.size = sizeof(blocks);
            
//             lsdj_vio_t wvio;
//             wvio.write = lsdj_mwrite;
//             wvio.seek = lsdj_mseek;
//             wvio.tell = lsdj_mtell;
//             wvio.user_data = &mem;
            
//             unsigned int written_block_count = lsdj_compress(song_data.bytes, BLOCK_SIZE, current_block, BLOCK_COUNT, &wvio, error);
//             if (error && *error)
//                 return;
            
//             if (written_block_count == 0)
//                 printf("not enough space for compressing %s\n", name);
            
//             current_block += written_block_count;
//             for (int j = 0; j < written_block_count; ++j)
//                 *table_ptr++ = (unsigned char)i;
//         }
//     }
    
//     // Write the header and blocks
//     if (vio->write(&header, sizeof(header), vio->user_data) != sizeof(header))
//         return lsdj_error_new(error, "could not write header");
//     if (vio->write(&block_alloc_table, sizeof(block_alloc_table), vio->user_data) != sizeof(block_alloc_table))
//         return lsdj_error_new(error, "could not write block allocation table");
//     if (vio->write(blocks, sizeof(blocks), vio->user_data) != sizeof(blocks))
//         return lsdj_error_new(error, "could not write blocks");
// }

// void lsdj_sav_write_to_file(const lsdj_sav_t* sav, const char* path, lsdj_error_t** error)
// {
//     if (path == NULL)
//         return lsdj_error_new(error, "path is NULL");
    
//     if (sav == NULL)
//         return lsdj_error_new(error, "sav is NULL");
    
//     FILE* file = fopen(path, "wb");
//     if (file == NULL)
//     {
//         char message[512];
//         snprintf(message, 512, "could not open %s for writing", path);
//         return lsdj_error_new(error, message);
//     }
    
//     lsdj_vio_t vio;
//     vio.write = lsdj_fwrite;
//     vio.tell = lsdj_ftell;
//     vio.seek = lsdj_fseek;
//     vio.user_data = file;
    
//     lsdj_sav_write(sav, &vio, error);
    
//     fclose(file);
// }

// void lsdj_sav_write_to_memory(const lsdj_sav_t* sav, unsigned char* data, size_t size, lsdj_error_t** error)
// {
//     if (sav == NULL)
//         return lsdj_error_new(error, "sav is NULL");
    
//     if (data == NULL)
//         return lsdj_error_new(error, "data is NULL");
    
//     lsdj_memory_access_state_t mem;
//     mem.begin = data;
//     mem.cur = mem.begin;
//     mem.size = size;
    
//     lsdj_vio_t vio;
//     vio.write = lsdj_mwrite;
//     vio.tell = lsdj_mtell;
//     vio.seek = lsdj_mseek;
//     vio.user_data = &mem;
    
//     lsdj_sav_write(sav, &vio, error);
// }
