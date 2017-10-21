#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compression.h"
#include "sav.h"

//! The memory place of the header
static const unsigned int HEADER_START = SONG_DECOMPRESSED_SIZE;
static const unsigned int BLOCK_COUNT = 191;
static const unsigned int BLOCK_SIZE = 0x200;

typedef struct
{
	char project_names[PROJECT_COUNT * 8];
	unsigned char versions[PROJECT_COUNT * 1];
	unsigned char empty[30];
	char init[2];
	unsigned char active_project;
} header_t;

// Read compressed project data from memory sav file
void read_compressed_blocks(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_project_t* projects, lsdj_error_t** error)
{
    // Read the block allocation table
    unsigned char blocks_alloc_table[BLOCK_COUNT];
    read(blocks_alloc_table, sizeof(blocks_alloc_table), user_data);
    
    // Read the blocks
    unsigned char blocks[BLOCK_COUNT][BLOCK_SIZE];
    read(blocks, sizeof(blocks), user_data);
    
    // Pointers for storing decompressed song data
    // Handle decompression
    for (unsigned char i = 0; i < BLOCK_COUNT; ++i)
    {
        unsigned char project = blocks_alloc_table[i];
        if (project == 0xFF || projects[project].song)
            continue;
        
        unsigned char data[SONG_DECOMPRESSED_SIZE];
        lsdj_decompress(&blocks[0][0], i, BLOCK_SIZE, data);
        
        // Read the song from memory
        projects[project].song = malloc(sizeof(lsdj_song_t));
        lsdj_read_song_from_memory(data, sizeof(lsdj_song_t), projects[project].song, error);
        if (*error)
            return;
    }
}

void lsdj_read_sav(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_sav_t* sav, lsdj_error_t** error)
{
    // Check for incorrect input
    if (read == NULL)
        return lsdj_create_error(error, "read is NULL");
    
    if (seek == NULL)
        return lsdj_create_error(error, "seek is NULL");
    
    if (sav == NULL)
        return lsdj_create_error(error, "sav is NULL");
    
    lsdj_clear_sav(sav);
    
    // Skip memory representing the working song (we'll get to that)
    const long begin = tell(user_data);
    seek(HEADER_START, SEEK_SET, user_data);
    
    // Read the header block, before we start processing each song
    header_t header;
    read(&header, sizeof(header), user_data);
    
    // Check the initialization characters. If they're not 'jk', we're
    // probably not dealing with an actual LSDJ sav format file.
    if (header.init[0] != 'j' || header.init[1] != 'k')
        return lsdj_create_error(error, "SRAM initialization check wasn't 'jk'");
    
    // Allocate data for all the projects and store their names
    for (int i = 0; i < PROJECT_COUNT; ++i)
    {
        memcpy(sav->projects[i].name, &header.project_names[i * 8], 8);
        sav->projects[i].version = header.versions[i];
    }
    
    // Store the active project index
    sav->activeProject = header.active_project;
    
    // Read the compressed projects
    read_compressed_blocks(read, seek, user_data, sav->projects, error);
    if (*error)
        return;
    
    // Read the working song
    const long end = tell(user_data);
    seek(begin, SEEK_SET, user_data);
    unsigned char song_data[SONG_DECOMPRESSED_SIZE];
    read(song_data, sizeof(song_data), user_data);
    lsdj_read_song_from_memory(song_data, sizeof(song_data), &sav->song, error);
    
    seek(end, SEEK_SET, user_data);
}

void lsdj_read_sav_from_file(const char* path, lsdj_sav_t* sav, lsdj_error_t** error)
{
    if (path == NULL)
        return lsdj_create_error(error, "path is NULL");
    
    if (sav == NULL)
        return lsdj_create_error(error, "sav is NULL");
        
    FILE* file = fopen("/Users/stijnfrishert/Desktop/LSDj/4ntler/Chipwrecked Set.sav", "r");
    if (file == NULL)
        return lsdj_create_error(error, "could not open file for reading");

    lsdj_read_sav(lsdj_fread, lsdj_ftell, lsdj_fseek, file, sav, error);
    
    fclose(file);
}

void lsdj_read_sav_from_memory(const unsigned char* data, size_t size, lsdj_sav_t* sav, lsdj_error_t** error)
{
    if (data == NULL)
        return lsdj_create_error(error, "data is NULL");
    
    if (sav == NULL)
        return lsdj_create_error(error, "sav is NULL");

    lsdj_memory_data_t mem;
    mem.begin = (unsigned char*)data;
    mem.cur = mem.begin;
    mem.size = size;
    
    lsdj_read_sav(lsdj_mread, lsdj_mtell, lsdj_mseek, &mem, sav, error);
}

void lsdj_write_sav(const lsdj_sav_t* sav, lsdj_vio_write_t write, void* user_data, lsdj_error_t** error)
{
    // Write the working project
    unsigned char song_data[SONG_DECOMPRESSED_SIZE];
    lsdj_write_song_to_memory(&sav->song, song_data, SONG_DECOMPRESSED_SIZE, error);
    write(song_data, sizeof(song_data), user_data);

    // Create the header for writing
    header_t header;
    memset(&header, 0, sizeof(header));
    header.init[0] = 'j';
    header.init[1] = 'k';
    header.active_project = sav->activeProject;

    // Create the block allocation table for writing
    unsigned char block_alloc_table[BLOCK_COUNT];
    memset(&block_alloc_table, 0xFF, sizeof(block_alloc_table));
    unsigned char* table_ptr = block_alloc_table;

    // Write project specific data
    unsigned char blocks[BLOCK_SIZE][BLOCK_COUNT];
    unsigned char current_block = 0;
    memset(blocks, 0, sizeof(blocks));
    for (int i = 0; i < PROJECT_COUNT; ++i)
    {
        // Write project name
        memcpy(&header.project_names[i * 8], sav->projects[i].name, 8);

        // Write project version
        header.versions[i] = sav->projects[i].version;

        if (sav->projects[i].song)
        {
            // Compress the song to memory
            unsigned char song_data[SONG_DECOMPRESSED_SIZE];
            lsdj_write_song_to_memory(sav->projects[i].song, song_data, SONG_DECOMPRESSED_SIZE, error);
            unsigned int written_block_count = lsdj_compress(song_data, &blocks[0][0], BLOCK_SIZE, current_block, BLOCK_COUNT);

            current_block += written_block_count;
            for (int j = 0; j < written_block_count; ++j)
                *table_ptr++ = (unsigned char)i;
        }
    }

    // Write the header and blocks
    write(&header, sizeof(header), user_data);
    write(&block_alloc_table, sizeof(block_alloc_table), user_data);
    write(blocks, sizeof(blocks), user_data);
}

void lsdj_write_sav_to_file(const lsdj_sav_t* sav, const char* path, lsdj_error_t** error)
{
    if (path == NULL)
        return lsdj_create_error(error, "path is NULL");
    
    if (sav == NULL)
        return lsdj_create_error(error, "sav is NULL");
    
    FILE* file = fopen(path, "w");
    if (file == NULL)
        return lsdj_create_error(error, "could not open file for writing");
    
    lsdj_write_sav(sav, lsdj_fwrite, file, error);
    
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
    
    lsdj_write_sav(sav, lsdj_mwrite, &mem, error);
}

void lsdj_clear_sav(lsdj_sav_t* sav)
{
    for (int i = 0; i < PROJECT_COUNT; ++i)
        lsdj_clear_project(&sav->projects[i]);
    
    sav->activeProject = 0xFF;
    
    lsdj_clear_song(&sav->song);
}
