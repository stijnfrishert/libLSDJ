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

//! Representation of an entire LSDj save state
struct lsdj_sav_t
{
    //! The song in active working memory
    lsdj_song_buffer_t working_memory_song_buffer;

    //! Index of the project that is currently being edited
    /*! Indices start at 0, a value of LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX means there is no active project */
    unsigned char active_project_index;

    //! The project slots
    /*! If one of these is NULL, it means the slot isn't used by the sav */
    lsdj_project_t* projects[LSDJ_SAV_PROJECT_COUNT];
    
    //! Reserved empty memory
    unsigned char reserved8120[30];
};

typedef struct
{
	char project_names[LSDJ_PROJECT_NAME_LENGTH][LSDJ_SAV_PROJECT_COUNT];
	unsigned char project_versions[LSDJ_SAV_PROJECT_COUNT];
	unsigned char empty[30];
	char init[2];
	unsigned char active_project;
    unsigned char block_allocation_table[BLOCK_COUNT];
} header_t;


// --- Allocation --- //

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
    {
        lsdj_error_optional_new(error, "could not allocate memory for sav");
        return NULL;
    }

    // Clear the working memory song buffer
    memset(&sav->working_memory_song_buffer, 0, sizeof(sav->working_memory_song_buffer));
    
    // Clear the projects
    for (int i = 0; i < LSDJ_SAV_PROJECT_COUNT; i++)
    {
        sav->projects[i] = NULL;
    }
    
    // None of the project slots is active
    sav->active_project_index = LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX;
    
    return sav;
}

lsdj_sav_t* lsdj_sav_copy(const lsdj_sav_t* sav, lsdj_error_t** error)
{
    // Allocate memory for a new sav
    lsdj_sav_t* copy = lsdj_sav_alloc(error);
    if (copy == NULL)
    {
        lsdj_error_optional_new(error, "could not allocate memory for sav");
        return NULL;
    }

    memcpy(&copy->working_memory_song_buffer, &sav->working_memory_song_buffer, sizeof(sav->working_memory_song_buffer));
    copy->active_project_index = sav->active_project_index;
    memcpy(copy->reserved8120, sav->reserved8120, sizeof(sav->reserved8120));

    for (int i = 0; i < LSDJ_SAV_PROJECT_COUNT; i++)
    {
        if (sav->projects[i] != NULL)
            copy->projects[i] = lsdj_project_copy(sav->projects[i], error);
        else
            copy->projects[i] = NULL;
    }

    // If something went wrong, free any copied over projects
    if (error && *error)
    {
        for (int i = 0; i < LSDJ_SAV_PROJECT_COUNT; i++)
            lsdj_project_free(copy->projects[i]);

        return NULL;
    }

    return copy;
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


// --- Changing Data --- //

void lsdj_sav_set_working_memory_song(lsdj_sav_t* sav, const lsdj_song_buffer_t* songBuffer)
{
    memcpy(&sav->working_memory_song_buffer, &songBuffer, sizeof(songBuffer));
}

bool lsdj_sav_set_working_memory_song_from_project(lsdj_sav_t* sav, unsigned char index, lsdj_error_t** error)
{
    if (index >= LSDJ_SAV_PROJECT_COUNT)
    {
        lsdj_error_optional_new(error, "The index is out of bounds");
        return false;
    }

    const lsdj_project_t* project = lsdj_sav_get_project(sav, index);
    if (project == NULL)
    {
        lsdj_error_optional_new(error, "no active project at given index");
        return false;
    }

    const lsdj_song_buffer_t* song = lsdj_project_get_song_buffer(project);
    assert(song != NULL);
    
    lsdj_sav_set_working_memory_song(sav, song);
    lsdj_sav_set_active_project_index(sav, index);

    return true;
}

const lsdj_song_buffer_t* lsdj_sav_get_working_memory_song(const lsdj_sav_t* sav)
{
    return &sav->working_memory_song_buffer;
}

void lsdj_sav_set_active_project_index(lsdj_sav_t* sav, unsigned char index)
{
    sav->active_project_index = index;
}

unsigned char lsdj_sav_get_active_project_index(const lsdj_sav_t* sav)
{
    return sav->active_project_index;
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

void lsdj_sav_set_project_copy(lsdj_sav_t* sav, unsigned char index, const lsdj_project_t* project, lsdj_error_t** error)
{
    lsdj_project_t* slot = sav->projects[index];
    if (slot)
        lsdj_project_free(slot);

    sav->projects[index] = lsdj_project_copy(project, error);
}

void lsdj_sav_set_project_move(lsdj_sav_t* sav, unsigned char index, lsdj_project_t* project)
{
    lsdj_project_t* slot = sav->projects[index];
    if (slot)
        lsdj_project_free(slot);

    sav->projects[index] = project;
}

void lsdj_sav_erase_project(lsdj_sav_t* sav, unsigned char index)
{
    lsdj_sav_set_project_move(sav, index, NULL);
}

const lsdj_project_t* lsdj_sav_get_project(const lsdj_sav_t* sav, unsigned char index)
{
    return sav->projects[index];
}

// Read compressed project data from memory sav file
bool decompress_blocks(lsdj_vio_t* rvio, header_t* header, lsdj_project_t** projects, lsdj_error_t** error)
{    
    // Pointers for storing decompressed song data
    // Handle decompression
    for (int i = 0; i < BLOCK_COUNT; ++i)
    {
        unsigned char p = header->block_allocation_table[i];
        if (p == 0xFF)
            continue;

        // Create the project if this is the first block we come across
        if (projects[p] == NULL)
        {
            lsdj_project_t* project = lsdj_project_new(error);
            if (project == NULL)
            {
                lsdj_error_optional_new(error, "could not create project for sav loading");
                return false;
            }

            lsdj_project_set_name(project, header->project_names[i], LSDJ_PROJECT_NAME_LENGTH);
            lsdj_project_set_version(project, header->project_versions[i]);

            lsdj_song_buffer_t song_buffer;
            lsdj_memory_access_state_t state;
            state.begin = state.cur = song_buffer.bytes;
            state.size = sizeof(song_buffer.bytes);

            lsdj_vio_t wvio = lsdj_create_memory_vio(&state);

            if (lsdj_decompress(rvio, &wvio, true, error) == false)
            {
                lsdj_project_free(project);
                return false;
            }

            lsdj_project_set_song_buffer(project, &song_buffer);

            projects[p] = project;
        }
    }

    return true;
}

lsdj_sav_t* lsdj_sav_read(lsdj_vio_t* rvio, lsdj_error_t** error)
{
    // Check for incorrect input
    if (rvio->read == NULL)
    {
        lsdj_error_optional_new(error, "vio->read is NULL");
        return NULL;
    }
    
    if (rvio->seek == NULL)
    {
        lsdj_error_optional_new(error, "vio->seek is NULL");
        return NULL;
    }
    
    if (rvio->tell == NULL)
    {
        lsdj_error_optional_new(error, "vio->tell is NULL");
        return NULL;
    }
    
    lsdj_sav_t* sav = lsdj_sav_alloc(error);
    if (sav == NULL)
        return NULL;

    // Read the working memory song
    rvio->read(sav->working_memory_song_buffer.bytes, sizeof(sav->working_memory_song_buffer.bytes), rvio->user_data);
    
    // Read the header block, before we start processing each song
	header_t header;
    assert(sizeof(header) == BLOCK_SIZE);
	rvio->read(&header, sizeof(header), rvio->user_data);
    
    // Check the initialization characters. If they're not 'jk', we're
    // probably not dealing with an actual LSDJ sav format file.
    if (header.init[0] != 'j' || header.init[1] != 'k')
    {
        lsdj_sav_free(sav);
        lsdj_error_optional_new(error, "SRAM initialization check wasn't 'jk'");
        return NULL;
    }

    // Store the active project index
    sav->active_project_index = header.active_project;
    
    // Store the reserved empty memory at 0x8120
    // Not sure what's really in there, but might as well keep it intact
    memcpy(sav->reserved8120, header.empty, sizeof(sav->reserved8120));
    
    // Read the compressed projects
    if (decompress_blocks(rvio, &header, sav->projects, error) == false)
    {
        lsdj_sav_free(sav);
        return NULL;
    }
    
 //    // Read the working song
 //    const long end = rvio->tell(rvio->user_data);
 //    if (end == -1L)
 //    {
 //        lsdj_error_optional_new(error, "could not tell end of sav read");
 //        lsdj_sav_free(sav);
 //        return NULL;
 //    }
    
 //    rvio->seek(begin, SEEK_SET, rvio->user_data);
 //    lsdj_song_buffer_t song_data;
 //    if (rvio->read(song_data.bytes, sizeof(song_data.bytes), rvio->user_data) != sizeof(song_data.bytes))
 //    {
 //        lsdj_sav_free(sav);
 //        lsdj_error_optional_new(error, "could not read compressed song data");
 //        return NULL;
 //    }
    
 //    sav->song = lsdj_song_new(error);
 //    if (error && *error)
 //    {
 //        lsdj_sav_free(sav);
 //        return NULL;
 //    }
    
 //    sav->song = lsdj_song_read_from_memory(song_data.bytes, sizeof(song_data.bytes), error);
    
 //    rvio->seek(end, SEEK_SET, rvio->user_data);
    
    return sav;
}

lsdj_sav_t* lsdj_sav_read_from_file(const char* path, lsdj_error_t** error)
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

    lsdj_sav_t* sav = lsdj_sav_read(&rvio, error);
    
    fclose(file);
    return sav;
}

lsdj_sav_t* lsdj_sav_read_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error)
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
    
    return lsdj_sav_read(&rvio, error);
}

// int lsdj_sav_is_likely_valid(lsdj_vio_t* vio, lsdj_error_t** error)
// {
//     // Check for incorrect input
//     if (vio->read == NULL)
//     {
//         lsdj_error_optional_new(error, "vio->read is NULL");
//         return 0;
//     }
    
//     if (vio->seek == NULL)
//     {
//         lsdj_error_optional_new(error, "vio->seek is NULL");
//         return 0;
//     }
    
//     // Move to the initialization bytes
//     vio->seek(LSDJ_SAV_HEADER_POSITION + 0x13E, SEEK_CUR, vio->user_data);
    
//     // Ensure these bytes are 'jk', that's what LSDJ sets them to on RAM init
//     char buffer[2];
//     vio->read(buffer, sizeof(buffer), vio->user_data);
    
//     if (buffer[0] != 'j' || buffer[1] != 'k')
//     {
//         lsdj_error_optional_new(error, "Memory 0x813E isn't 'jk'");
//         return 0;
//     }
    
//     return 1;
// }

// int lsdj_sav_is_likely_valid_file(const char* path, lsdj_error_t** error)
// {
//     if (path == NULL)
//     {
//         lsdj_error_optional_new(error, "path is NULL");
//         return 0;
//     }
    
//     FILE* file = fopen(path, "rb");
//     if (file == NULL)
//     {
//         char message[512];
//         snprintf(message, 512, "could not open %s for reading", path);
//         lsdj_error_optional_new(error, message);
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
//         lsdj_error_optional_new(error, "data is NULL");
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
//         return lsdj_error_optional_new(error, "could not write compressed song data");

//     // Create the header for writing
//     header_t header;
//     memset(&header, 0, sizeof(header));
//     header.init[0] = 'j';
//     header.init[1] = 'k';
//     header.active_project = sav->active_project_index;
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
//         return lsdj_error_optional_new(error, "could not write header");
//     if (vio->write(&block_alloc_table, sizeof(block_alloc_table), vio->user_data) != sizeof(block_alloc_table))
//         return lsdj_error_optional_new(error, "could not write block allocation table");
//     if (vio->write(blocks, sizeof(blocks), vio->user_data) != sizeof(blocks))
//         return lsdj_error_optional_new(error, "could not write blocks");
// }

// void lsdj_sav_write_to_file(const lsdj_sav_t* sav, const char* path, lsdj_error_t** error)
// {
//     if (path == NULL)
//         return lsdj_error_optional_new(error, "path is NULL");
    
//     if (sav == NULL)
//         return lsdj_error_optional_new(error, "sav is NULL");
    
//     FILE* file = fopen(path, "wb");
//     if (file == NULL)
//     {
//         char message[512];
//         snprintf(message, 512, "could not open %s for writing", path);
//         return lsdj_error_optional_new(error, message);
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
//         return lsdj_error_optional_new(error, "sav is NULL");
    
//     if (data == NULL)
//         return lsdj_error_optional_new(error, "data is NULL");
    
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
