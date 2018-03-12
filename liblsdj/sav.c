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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compression.h"
#include "sav.h"

#define LSDJ_SAV_PROJECT_COUNT 32
#define HEADER_START SONG_DECOMPRESSED_SIZE
#define BLOCK_COUNT 191
#define BLOCK_SIZE 0x200

// Representation of an entire LSDJ save file
struct lsdj_sav_t
{
    // The projects
    lsdj_project_t* projects[LSDJ_SAV_PROJECT_COUNT];
    
    // Index of the project that is currently being edited
    /*! Indices start at 0, a value of 0xFF means there is no active project */
    unsigned char activeProject;
    
    // The song in active working memory
    lsdj_song_t* song;
    
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

lsdj_sav_t* alloc_sav(lsdj_error_t** error)
{
    lsdj_sav_t* sav = (lsdj_sav_t*)calloc(sizeof(lsdj_sav_t), 1);
    if (sav == NULL)
    {
        lsdj_create_error(error, "could not allocate sav");
        return NULL;
    }
    
    return sav;
}

lsdj_sav_t* lsdj_new_sav(lsdj_error_t** error)
{
    lsdj_sav_t* sav = alloc_sav(error);
    if (sav == NULL)
        return NULL;
    
    for (int i = 0; i < LSDJ_SAV_PROJECT_COUNT; ++i)
    {
        sav->projects[i] = lsdj_new_project(error);
        if (error && *error)
        {
            lsdj_free_sav(sav);
            return NULL;
        }
    }
    
    sav->activeProject = 0xFF;
    
    sav->song = lsdj_new_song(error);
    if (error && *error)
    {
        lsdj_free_sav(sav);
        return NULL;
    }
    
    return sav;
}

void lsdj_free_sav(lsdj_sav_t* sav)
{
    if (sav)
    {
        for (int i = 0; i < LSDJ_SAV_PROJECT_COUNT; ++i)
            lsdj_free_project(sav->projects[i]);
        
        lsdj_free_song(sav->song);
        
        free(sav);
    }
}

void lsdj_sav_set_song(lsdj_sav_t* sav, lsdj_song_t* song)
{
    if (sav->song)
        lsdj_free_song(sav->song);
    
    sav->song = song;
}

lsdj_song_t* lsdj_sav_get_song(const lsdj_sav_t* sav)
{
    return sav->song;
}

void lsdj_sav_copy_active_project(lsdj_sav_t* sav, unsigned char index, lsdj_error_t** error)
{
    lsdj_song_t* song = lsdj_project_get_song(sav->projects[index]);
    if (song == NULL)
        return lsdj_create_error(error, "no song at given index");
    
    lsdj_song_t* copy = lsdj_copy_song(song, error);
    if (*error)
        return;
    
    lsdj_sav_set_song(sav, copy);
    sav->activeProject = index;
}

unsigned char lsdj_sav_get_active_project(const lsdj_sav_t* sav)
{
    return sav->activeProject;
}

unsigned int lsdj_sav_get_project_count(const lsdj_sav_t* sav)
{
    return LSDJ_SAV_PROJECT_COUNT;
}

void lsdj_sav_set_project(lsdj_sav_t* sav, unsigned char index, lsdj_project_t* project, lsdj_error_t** error)
{
    if (project == NULL)
        return lsdj_create_error(error, "project is NULL");
    
    lsdj_free_project(sav->projects[index]);
    sav->projects[index] = project;
}

lsdj_project_t* lsdj_sav_get_project(const lsdj_sav_t* sav, unsigned char project)
{
    return sav->projects[project];
}

// Read compressed project data from memory sav file
void read_compressed_blocks(lsdj_vio_t* vio, lsdj_project_t** projects, lsdj_error_t** error)
{
    // Read the block allocation table
    unsigned char blocks_alloc_table[BLOCK_COUNT];
    vio->read(blocks_alloc_table, sizeof(blocks_alloc_table), vio->user_data);
    
    // Pointers for storing decompressed song data
    // Handle decompression
    for (int i = 0; i < BLOCK_COUNT; ++i)
    {
        unsigned char p = blocks_alloc_table[i];
        if (p == 0xFF)
            continue;
        
        lsdj_project_t* project = projects[p];
        if (lsdj_project_get_song(project) != NULL)
            continue;
        
        unsigned char data[SONG_DECOMPRESSED_SIZE];
        memset(data, 0x00, sizeof(data));
        
        vio->seek(HEADER_START + (i + 1) * BLOCK_SIZE, SEEK_SET, vio->user_data);
                
        lsdj_memory_data_t mem;
        mem.cur = mem.begin = data;
        mem.size = sizeof(data);
        
        long block1position = HEADER_START + BLOCK_SIZE;
        
        lsdj_vio_t wvio;
        wvio.write = lsdj_mwrite;
        wvio.tell = lsdj_mtell;
        wvio.seek = lsdj_mseek;
        wvio.user_data = &mem;
        lsdj_decompress(vio, &wvio, &block1position, BLOCK_SIZE);
        
        // Read the song from memory
        lsdj_song_t* song = lsdj_read_song_from_memory(data, sizeof(data), error);
        if (error && *error)
        {
            free(song);
            return;
        }
        
        lsdj_project_set_song(project, song);
    }
}

lsdj_sav_t* lsdj_read_sav(lsdj_vio_t* vio, lsdj_error_t** error)
{
    // Check for incorrect input
    if (vio->read == NULL)
    {
        lsdj_create_error(error, "read is NULL");
        return NULL;
    }
    
    if (vio->seek == NULL)
    {
        lsdj_create_error(error, "seek is NULL");
        return NULL;
    }
    
    lsdj_sav_t* sav = alloc_sav(error);
    if (sav == NULL)
        return NULL;
    
    // Skip memory representing the working song (we'll get to that)
    const long begin = vio->tell(vio->user_data);
    vio->seek(begin + HEADER_START, SEEK_SET, vio->user_data);
    
    // Read the header block, before we start processing each song
	header_t header;
	vio->read(&header, sizeof(header), vio->user_data);
    
    // Check the initialization characters. If they're not 'jk', we're
    // probably not dealing with an actual LSDJ sav format file.
    if (header.init[0] != 'j' || header.init[1] != 'k')
    {
        lsdj_create_error(error, "SRAM initialization check wasn't 'jk'");
        return NULL;
    }
    
    // Allocate data for all the projects and store their names
    for (int i = 0; i < LSDJ_SAV_PROJECT_COUNT; ++i)
    {
        lsdj_project_t* project = lsdj_new_project(error);
        if (error && *error)
        {
            lsdj_free_sav(sav);
            return NULL;
        }
        
        lsdj_project_set_name(project, &header.project_names[i * 8], 8);
        lsdj_project_set_version(project, header.versions[i]);
        
        sav->projects[i] = project;
    }
    
    // Store the active project index
    sav->activeProject = header.active_project;
    
    // Store the reserved empty memory at 0x8120
    // Not sure what's really in there, but might as well keep it intact
    memcpy(sav->reserved8120, header.empty, sizeof(sav->reserved8120));
    
    // Read the compressed projects
    read_compressed_blocks(vio, sav->projects, error);
    if (error && *error)
        return NULL;
    
    // Read the working song
    const long end = vio->tell(vio->user_data);
    vio->seek(begin, SEEK_SET, vio->user_data);
    unsigned char song_data[SONG_DECOMPRESSED_SIZE];
    vio->read(song_data, sizeof(song_data), vio->user_data);
    sav->song = lsdj_new_song(error);
    if (error && *error)
    {
        lsdj_free_sav(sav);
        return NULL;
    }
    
    sav->song = lsdj_read_song_from_memory(song_data, sizeof(song_data), error);
    
    vio->seek(end, SEEK_SET, vio->user_data);
    
    return sav;
}

lsdj_sav_t* lsdj_read_sav_from_file(const char* path, lsdj_error_t** error)
{
    if (path == NULL)
    {
        lsdj_create_error(error, "path is NULL");
        return NULL;
    }
        
    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        char message[512];
        snprintf(message, 512, "could not open %s for reading", path);
        lsdj_create_error(error, message);
        return NULL;
    }
    
    lsdj_vio_t vio;
    vio.read = lsdj_fread;
    vio.tell = lsdj_ftell;
    vio.seek = lsdj_fseek;
    vio.user_data = file;

    lsdj_sav_t* sav = lsdj_read_sav(&vio, error);
    
    fclose(file);
    return sav;
}

lsdj_sav_t* lsdj_read_sav_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error)
{
    if (data == NULL)
    {
        lsdj_create_error(error, "data is NULL");
        return NULL;
    }

    lsdj_memory_data_t mem;
    mem.begin = (unsigned char*)data;
    mem.cur = mem.begin;
    mem.size = size;
    
    lsdj_vio_t vio;
    vio.read = lsdj_mread;
    vio.tell = lsdj_mtell;
    vio.seek = lsdj_mseek;
    vio.user_data = &mem;
    
    return lsdj_read_sav(&vio, error);
}

void lsdj_write_sav(const lsdj_sav_t* sav, lsdj_vio_t* vio, lsdj_error_t** error)
{
    // Write the working project
    unsigned char song_data[SONG_DECOMPRESSED_SIZE];
    lsdj_write_song_to_memory(sav->song, song_data, SONG_DECOMPRESSED_SIZE, error);
    vio->write(song_data, sizeof(song_data), vio->user_data);

    // Create the header for writing
    header_t header;
    memset(&header, 0, sizeof(header));
    header.init[0] = 'j';
    header.init[1] = 'k';
    header.active_project = sav->activeProject;
    memcpy(header.empty, sav->reserved8120, sizeof(sav->reserved8120));

    // Create the block allocation table for writing
    unsigned char block_alloc_table[BLOCK_COUNT];
    memset(&block_alloc_table, 0xFF, sizeof(block_alloc_table));
    unsigned char* table_ptr = block_alloc_table;

    // Write project specific data
    unsigned char blocks[BLOCK_COUNT][BLOCK_SIZE];
    
    unsigned char current_block = 1;
    memset(blocks, 0, sizeof(blocks));
    for (int i = 0; i < LSDJ_SAV_PROJECT_COUNT; ++i)
    {
        lsdj_project_t* project = sav->projects[i];
        
        // Write project name
        char name[PROJECT_NAME_LENGTH];
        lsdj_project_get_name(project, name, PROJECT_NAME_LENGTH);
        strncpy(&header.project_names[i * 8], name, PROJECT_NAME_LENGTH < 8 ? PROJECT_NAME_LENGTH : 8);
        
        // Write project version
        header.versions[i] = lsdj_project_get_version(project);
        
        lsdj_song_t* song = lsdj_project_get_song(project);
        if (song)
        {
            // Compress the song to memory
            unsigned char song_data[SONG_DECOMPRESSED_SIZE];
            lsdj_write_song_to_memory(song, song_data, SONG_DECOMPRESSED_SIZE, error);
            
            lsdj_memory_data_t mem;
            mem.cur = mem.begin = blocks[current_block - 1];
            mem.size = sizeof(blocks);
            
            lsdj_vio_t wvio;
            wvio.write = lsdj_mwrite;
            wvio.seek = lsdj_mseek;
            wvio.tell = lsdj_mtell;
            wvio.user_data = &mem;
            
//            unsigned int written_block_count = lsdj_compress(song_data, &blocks[0][0], BLOCK_SIZE, current_block, BLOCK_COUNT);
            unsigned int written_block_count = lsdj_compress(song_data, BLOCK_SIZE, current_block, BLOCK_COUNT, &wvio);
            
            if (written_block_count == 0)
            {
                char message[45];
                memset(message, 0, sizeof(message));
                sprintf(message, "not enough space for compressing project %d", i);
                return lsdj_create_error(error, message);
            }
            
            current_block += written_block_count;
            for (int j = 0; j < written_block_count; ++j)
                *table_ptr++ = (unsigned char)i;
        }
    }
    
    // Write the header and blocks
    vio->write(&header, sizeof(header), vio->user_data);
    vio->write(&block_alloc_table, sizeof(block_alloc_table), vio->user_data);
    vio->write(blocks, sizeof(blocks), vio->user_data);
}

void lsdj_write_sav_to_file(const lsdj_sav_t* sav, const char* path, lsdj_error_t** error)
{
    if (path == NULL)
        return lsdj_create_error(error, "path is NULL");
    
    if (sav == NULL)
        return lsdj_create_error(error, "sav is NULL");
    
    FILE* file = fopen(path, "wb");
    if (file == NULL)
    {
        char message[512];
        snprintf(message, 512, "could not open %s for writing", path);
        return lsdj_create_error(error, message);
    }
    
    lsdj_vio_t vio;
    vio.write = lsdj_fwrite;
    vio.tell = lsdj_ftell;
    vio.seek = lsdj_fseek;
    vio.user_data = file;
    
    lsdj_write_sav(sav, &vio, error);
    
    fclose(file);
}

void lsdj_write_sav_to_memory(const lsdj_sav_t* sav, unsigned char* data, size_t size, lsdj_error_t** error)
{
    if (sav == NULL)
        return lsdj_create_error(error, "sav is NULL");
    
    if (data == NULL)
        return lsdj_create_error(error, "data is NULL");
    
    lsdj_memory_data_t mem;
    mem.begin = data;
    mem.cur = mem.begin;
    mem.size = size;
    
    lsdj_vio_t vio;
    vio.write = lsdj_mwrite;
    vio.tell = lsdj_mtell;
    vio.seek = lsdj_mseek;
    vio.user_data = &mem;
    
    lsdj_write_sav(sav, &vio, error);
}
