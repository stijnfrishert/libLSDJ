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

lsdj_song_t* lsdj_sav_get_song(const lsdj_sav_t* sav)
{
    return sav->song;
}

int lsdj_sav_get_active_project(const lsdj_sav_t* sav)
{
    return sav->activeProject == 0xFF ? -1 : sav->activeProject;
}

unsigned int lsdj_sav_get_project_count(const lsdj_sav_t* sav)
{
    return LSDJ_SAV_PROJECT_COUNT;
}

lsdj_project_t* lsdj_sav_get_project(const lsdj_sav_t* sav, unsigned char project)
{
    return sav->projects[project];
}

// Read compressed project data from memory sav file
void read_compressed_blocks(lsdj_vio_read_t read, lsdj_vio_seek_t seek, lsdj_vio_tell_t tell, void* user_data, lsdj_project_t** projects, lsdj_error_t** error)
{
    // Read the block allocation table
    unsigned char blocks_alloc_table[BLOCK_COUNT];
    read(blocks_alloc_table, sizeof(blocks_alloc_table), user_data);
    
    const long firstBlockOffset = tell(user_data);
    
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
        
        seek(firstBlockOffset + i * BLOCK_SIZE, SEEK_SET, user_data);
        lsdj_decompress(read, seek, tell, user_data, firstBlockOffset, BLOCK_SIZE, data);
        
//        FILE* file = fopen("/Users/stijnfrishert/Desktop/uncompr.hex", "wb");
//        fwrite(data, SONG_DECOMPRESSED_SIZE, 1, file);
//        fclose(file);
        
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

lsdj_sav_t* lsdj_read_sav(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_error_t** error)
{
    // Check for incorrect input
    if (read == NULL)
    {
        lsdj_create_error(error, "read is NULL");
        return NULL;
    }
    
    if (seek == NULL)
    {
        lsdj_create_error(error, "seek is NULL");
        return NULL;
    }
    
    lsdj_sav_t* sav = alloc_sav(error);
    if (sav == NULL)
        return NULL;
    
    // Skip memory representing the working song (we'll get to that)
    const long begin = tell(user_data);
    seek(begin + HEADER_START, SEEK_SET, user_data);
    
    // Read the header block, before we start processing each song
	header_t header;
	read(&header, sizeof(header), user_data);
    
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
    
    // Read the compressed projects
    read_compressed_blocks(read, seek, tell, user_data, sav->projects, error);
    if (error && *error)
        return NULL;
    
    // Read the working song
    const long end = tell(user_data);
    seek(begin, SEEK_SET, user_data);
    unsigned char song_data[SONG_DECOMPRESSED_SIZE];
    read(song_data, sizeof(song_data), user_data);
    sav->song = lsdj_new_song(error);
    if (error && *error)
    {
        lsdj_free_sav(sav);
        return NULL;
    }
    
    sav->song = lsdj_read_song_from_memory(song_data, sizeof(song_data), error);
    
    seek(end, SEEK_SET, user_data);
    
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
        lsdj_create_error(error, "could not open file for reading");
        return NULL;
    }

    lsdj_sav_t* sav = lsdj_read_sav(lsdj_fread, lsdj_ftell, lsdj_fseek, file, error);
    
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
    
    return lsdj_read_sav(lsdj_mread, lsdj_mtell, lsdj_mseek, &mem, error);
}

void lsdj_write_sav(const lsdj_sav_t* sav, lsdj_vio_write_t write, void* user_data, lsdj_error_t** error)
{
    // Write the working project
    unsigned char song_data[SONG_DECOMPRESSED_SIZE];
    lsdj_write_song_to_memory(sav->song, song_data, SONG_DECOMPRESSED_SIZE, error);
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
    
    FILE* file = fopen(path, "wb");
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
