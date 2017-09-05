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
void read_compressed_blocks(lsdj_project_t* projects, unsigned short project_count, FILE* file)
{
    unsigned char blocks_table[BLOCKS_TABLE_SIZE];
    fread(blocks_table, sizeof(blocks_table), 1, file);
    
    for (int i = 0; i < project_count; ++i)
    {
        projects[i].compressed_data.size = 0;
        projects[i].compressed_data.data = NULL;
    }
    
    // Gather the size of each project in blocks
    for (int i = 0; i < BLOCKS_TABLE_SIZE; ++i)
    {
        unsigned char project = blocks_table[i];
        if (project >= project_count)
            continue;
        
        projects[project].compressed_data.size += BLOCK_SIZE;
    }
    
    // Allocate space to store the compressed data
    void** ptrs = malloc(sizeof(void*) * project_count);
    for (int i = 0; i < project_count; ++i)
    {
        size_t size = projects[i].compressed_data.size;
        lsdj_project_t* project = projects + i;
        
        if (size > 0)
        {
            project->compressed_data.data = malloc(size);
            memset(project->compressed_data.data, 0, size);
        } else {
            project->compressed_data.data = NULL;
        }
        
        ptrs[i] = project->compressed_data.data;
    }
    
    // Store each block
    for (int i = 0; i < BLOCKS_TABLE_SIZE; ++i)
    {
        unsigned char project = blocks_table[i];
        if (project >= project_count)
            continue;
        
        fread(ptrs[project], BLOCK_SIZE, 1, file);
        ptrs[project] += BLOCK_SIZE;
    }
    
    for (int i = 0; i < project_count; ++i)
    {
        assert(ptrs[i] == projects[i].compressed_data.data + projects[i].compressed_data.size);
    }
    
    free(ptrs);
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

    // Skip memory representing the currently open song
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

	// Find out how many projects there are
	sav->project_count = 0;
	char* ptr = header.project_names;
	while (1)
	{
		size_t length = strlen(ptr);
        if (length > 8)
            length = 8;
        
		if (length == 0)
			break;

		sav->project_count += 1;
        ptr += length;
        if (*ptr == '\0')
            ptr += 1;
	}
    
    // Allocate data for all the projects and store their names
    sav->projects = malloc(sizeof(lsdj_project_t) * sav->project_count);
    ptr = header.project_names;
    for (int i = 0; i < sav->project_count; ++i)
    {
        // Store the project name
    	strncpy(sav->projects[i].name, ptr, 8);
        size_t length = strlen(ptr);
        ptr += (length < 8 ? length + 1 : 8);
        
        // Store the project version
        sav->projects[i].version = header.versions[i];
    }

	// Store the active project index
	sav->active_project = header.active_project == 0xFF ? -1 : header.active_project;
    
    // Read the compressed projects
    read_compressed_blocks(sav->projects, sav->project_count, file);

    // Clean-up and close the file
	fclose(file);

    // Return the save structure
	return sav;
}

void lsdj_close_sav(lsdj_sav_t* sav)
{
    for (int i = 0; i < sav->project_count; ++i)
    {
        if (sav->projects[i].compressed_data.size > 0)
            free(sav->projects[i].compressed_data.data);
    }
    
    free(sav->projects);
	free(sav);
}
