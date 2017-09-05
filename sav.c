#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sav.h"

const unsigned int HEADER_START = 0x8000;
const unsigned int BLOCK_SIZE = 0x200;
const unsigned int BLOCKS_TABLE_SIZE = 191;

typedef struct
{
	char project_names[32 * 8];
	unsigned char versions[32 * 1];
	unsigned char empty[30];
	char init[2];
	unsigned char active_project;
} header_t;

// Read compressed project data from memory sav file
void read_compressed_blocks(lsdj_project_t* projects, FILE* file)
{
    unsigned char blocks_table[BLOCKS_TABLE_SIZE];
    fread(blocks_table, sizeof(blocks_table), 1, file);
    
    for (int i = 0; i < 32; ++i)
    {
        if (projects[i].compressed_data.size > 0)
        {
            free(projects[i].compressed_data.data);
            projects[i].compressed_data.size = 0;
        }
        
        projects[i].compressed_data.data = NULL;
    }
    
    // Gather the size of each project in blocks
    for (int i = 0; i < BLOCKS_TABLE_SIZE; ++i)
    {
        unsigned char project = blocks_table[i];
        if (project == 0xFF)
            continue;
        
        projects[project].compressed_data.size += BLOCK_SIZE;
    }
    
    // Allocate space to store the compressed data
    void* ptrs[32];
    for (int i = 0; i < 32; ++i)
    {
        lsdj_project_t* project = projects + i;
        
        size_t size = project->compressed_data.size;
        
        if (size > 0)
        {
            project->compressed_data.data = malloc(size);
            memset(project->compressed_data.data, 0, size);
            ptrs[i] = project->compressed_data.data;
        } else {
            project->compressed_data.data = NULL;
            ptrs[i] = NULL;
        }
    }
    
    // Store each block
    for (int i = 0; i < BLOCKS_TABLE_SIZE; ++i)
    {
        unsigned char project = blocks_table[i];
        if (project == 0xFF)
            continue;
        
        assert(ptrs[project] != NULL);
        fread(ptrs[project], BLOCK_SIZE, 1, file);
        ptrs[project] += BLOCK_SIZE;
    }
}

void read_working_project(lsdj_project_decompressed_t* project, FILE* file)
{
    fread(project->data, sizeof(project->data), 1, file);
}

void write_working_project(const lsdj_project_decompressed_t* project, FILE* file)
{
    fwrite(project->data, sizeof(project->data), 1, file);
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
    for (int i = 0; i < 32; ++i)
    {
        memcpy(sav->projects[i].name, &header.project_names[i * 8], 8);
        sav->projects[i].version = header.versions[i];
    }

	// Store the active project index
	sav->active_project = header.active_project;
    
    // Read the compressed projects
    read_compressed_blocks(sav->projects, file);
    
    // Read the working song
    fseek(file, 0, SEEK_SET);
    read_working_project(&sav->working_project, file);

    // Clean-up and close the file
	fclose(file);

    // Return the save structure
	return sav;
}

void lsdj_write_sav(const lsdj_sav_t* sav, const char* path, lsdj_error_t** error)
{
    FILE* file = fopen(path, "w");
    if (file == NULL)
        return lsdj_create_error(error, "could not open file for writing");
    
    // Write the working project
    write_working_project(&sav->working_project, file);
    
    // Create the header for writing
    header_t header;
    memset(&header, 0, sizeof(header));
    header.init[0] = 'j';
    header.init[1] = 'k';
    header.active_project = sav->active_project;
    
    // Create the block allocation table for writing
    unsigned char block_alloc_table[BLOCKS_TABLE_SIZE];
    memset(&block_alloc_table, 0xFF, sizeof(block_alloc_table));
    unsigned char* table_ptr = block_alloc_table;
    
    // Write project specific data
    for (int i = 0; i < 32; ++i)
    {
        // Write project name
        memcpy(&header.project_names[i * 8], sav->projects[i].name, 8);
        
        // Write project version
        header.versions[i] = sav->projects[i].version;
        
        // Write in the block allocation table
        for (int j = 0; j < sav->projects[i].compressed_data.size / BLOCK_SIZE; ++j)
            *table_ptr++ = (unsigned char)i;
    }
    
    // Write the header and block allocation table
    fwrite(&header, sizeof(header), 1, file);
    fwrite(&block_alloc_table, sizeof(block_alloc_table), 1, file);
    
    // Write the actual blocks
    for (int i = 0; i < 32; ++i)
    {
        fwrite(sav->projects[i].compressed_data.data, sav->projects[i].compressed_data.size, 1, file);
    }
    
    // Close the file
    fclose(file);
}

void lsdj_free_sav(lsdj_sav_t* sav)
{
    for (int i = 0; i < 32; ++i)
    {
        if (sav->projects[i].compressed_data.size > 0)
            free(sav->projects[i].compressed_data.data);
    }
    
	free(sav);
}
