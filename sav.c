#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compression.h"
#include "sav.h"

//! The memory place of the header
const unsigned int HEADER_START = SONG_DECRYPTED_SIZE;

//! The amount of blocks in the LSDJ file system
const unsigned int BLOCK_COUNT = 191;

//! The size of a block in the LSDJ file system
const unsigned int BLOCK_SIZE = 0x200;

typedef struct
{
	char project_names[SAV_PROJECT_COUNT * 8];
	unsigned char versions[SAV_PROJECT_COUNT * 1];
	unsigned char empty[30];
	char init[2];
	unsigned char active_project;
} header_t;

// Read compressed project data from memory sav file
void read_compressed_blocks(lsdj_project_t* projects, FILE* file, lsdj_error_t** error)
{
    // Read the block allocation table
    unsigned char blocks_alloc_table[BLOCK_COUNT];
    fread(blocks_alloc_table, sizeof(blocks_alloc_table), 1, file);
    
    // Read the blocks
    unsigned char blocks[BLOCK_COUNT][BLOCK_SIZE];
    fread(blocks, sizeof(blocks), 1, file);
    
    // Pointers for storing decompressed song data
    // Handle decompression
    for (unsigned char i = 0; i < BLOCK_COUNT; ++i)
    {
        unsigned char project = blocks_alloc_table[i];
        if (projects[project].song)
            continue;
        
        unsigned char data[SONG_DECRYPTED_SIZE];
        decompress(&blocks[0][0], i, BLOCK_SIZE, data);
        
        // Read the song from memory
        projects[project].song = malloc(sizeof(lsdj_song_t));
        lsdj_read_song_from_memory(data, projects[project].song, error);
        if (error)
            return;
    }
}

lsdj_sav_t* lsdj_open_sav(const char* path, lsdj_error_t** error)
{
    // Try to open the sav file at the given path
	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		lsdj_create_error(error, "could not open file for reading");
		return NULL;
	}

    // Skip memory representing the working song (we'll get to that)
	fseek(file, HEADER_START, SEEK_SET);

	// Read the header block, before we start processing each song
	header_t header;
	fread(&header, sizeof(header), 1, file);

	// Check the initialization characters. If they're not 'jk', we're
	// probably not dealing with an actual LSDJ sav format file.
	if (header.init[0] != 'j' || header.init[1] != 'k')
	{
		lsdj_create_error(error, "SRAM initialization check wasn't 'jk'");
		fclose(file);
		return NULL;
	}

	// Create the sav file structure to return from this function
	lsdj_sav_t* sav = malloc(sizeof(lsdj_sav_t));
    
    // Allocate data for all the projects and store their names
    for (int i = 0; i < SAV_PROJECT_COUNT; ++i)
    {
        memcpy(sav->projects[i].name, &header.project_names[i * 8], 8);
        sav->projects[i].version = header.versions[i];
    }

	// Store the active project index
	sav->active_project = header.active_project;
    
    // Read the compressed projects
    read_compressed_blocks(sav->projects, file, error);
    if (error)
    {
        fclose(file);
        return NULL;
    }
    
    // Read the working song
    fseek(file, 0, SEEK_SET);
    unsigned char song_data[SONG_DECRYPTED_SIZE];
    fread(song_data, sizeof(song_data), 1, file);
    lsdj_read_song_from_memory(song_data, &sav->song, error);
    
    // Clean-up and close the file anyway
    fclose(file);
    
    // If the last song read threw an error, propagate it upward
    if (error)
        return NULL;

    // Return the save structure
	return sav;
}

void lsdj_write_sav(const lsdj_sav_t* sav, const char* path, lsdj_error_t** error)
{
    FILE* file = fopen(path, "w");
    if (file == NULL)
        return lsdj_create_error(error, "could not open file for writing");
    
    // Write the working project
    unsigned char song_data[SONG_DECRYPTED_SIZE];
    lsdj_write_song_to_memory(&sav->song, song_data);
    fwrite(song_data, sizeof(song_data), 1, file);
    
    // Create the header for writing
    header_t header;
    memset(&header, 0, sizeof(header));
    header.init[0] = 'j';
    header.init[1] = 'k';
    header.active_project = sav->active_project;
    
    // Create the block allocation table for writing
    unsigned char block_alloc_table[BLOCK_COUNT];
    memset(&block_alloc_table, 0xFF, sizeof(block_alloc_table));
    unsigned char* table_ptr = block_alloc_table;
    
    // Write project specific data
    unsigned char blocks[BLOCK_SIZE][BLOCK_COUNT];
    unsigned char current_block = 0;
    memset(blocks, 0, sizeof(blocks));
    for (int i = 0; i < SAV_PROJECT_COUNT; ++i)
    {
        // Write project name
        memcpy(&header.project_names[i * 8], sav->projects[i].name, 8);
        
        // Write project version
        header.versions[i] = sav->projects[i].version;
        
        if (sav->projects[i].song)
        {
            // Compress the song to memory
            unsigned char song_data[SONG_DECRYPTED_SIZE];
            lsdj_write_song_to_memory(sav->projects[i].song, song_data);
            unsigned int written_block_count = compress(song_data, &blocks[0][0], BLOCK_SIZE, current_block, BLOCK_COUNT);
            
            current_block += written_block_count;
            for (int j = 0; j < written_block_count; ++j)
                *table_ptr++ = (unsigned char)i;
        }
    }
    
    // Write the header and blocks
    fwrite(&header, sizeof(header), 1, file);
    fwrite(&block_alloc_table, sizeof(block_alloc_table), 1, file);
    fwrite(blocks, sizeof(blocks), 1, file);
    
    // Close the file
    fclose(file);
}

void lsdj_clear_sav(lsdj_sav_t* sav)
{
    for (int i = 0; i < SAV_PROJECT_COUNT; ++i)
        lsdj_clear_project(&sav->projects[i]);
    
    sav->active_project = 0;
}

void lsdj_free_sav(lsdj_sav_t* sav)
{
    for (int i = 0; i < SAV_PROJECT_COUNT; ++i)
    {
        if (sav->projects[i].song)
            free(sav->projects[i].song);
        
        if (sav->projects[i].compressed.size > 0)
            free(sav->projects[i].compressed.data);
    }
    
	free(sav);
}
