#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compression.h"
#include "project.h"

static const unsigned int BLOCK_SIZE = 0x200;
static const unsigned int BLOCK_COUNT = 191;

void lsdj_write_lsdsng(const lsdj_project_t* project, const char* path, lsdj_error_t** error)
{
    if (project->song == NULL)
        return lsdj_create_error(error, "project does not contain a song");
    
    FILE* file = fopen(path, "w");
    if (file == NULL)
        return lsdj_create_error(error, "could not open file for writing");
    
    fwrite(project->name, PROJECT_NAME_LENGTH, 1, file);
    fwrite(&project->version, 1, 1, file);
    
    // Write the song to memory
    unsigned char decompressed[SONG_DECOMPRESSED_SIZE];
    lsdj_write_song_to_memory(project->song, decompressed, SONG_DECOMPRESSED_SIZE, error);
    
    // Compress the song
    unsigned char compressed[SONG_DECOMPRESSED_SIZE];
    memset(&compressed, 0, SONG_DECOMPRESSED_SIZE);
    unsigned int block_write_count = lsdj_compress(decompressed, compressed, BLOCK_SIZE, 0, BLOCK_COUNT);
    
    fwrite(compressed, BLOCK_SIZE * block_write_count, 1, file);
    
    fclose(file);
}

void lsdj_read_lsdsng(const char* path, lsdj_project_t* project, lsdj_error_t** error)
{
	FILE* file = fopen(path, "r");
    if (file == NULL)
        return lsdj_create_error(error, "could not open file for reading");

    fread(project->name, PROJECT_NAME_LENGTH, 1, file);
    fread(&project->version, 1, 1, file);

    // Read the compressed data
    fseek(file, 0, SEEK_END);
    const size_t size = (size_t)(ftell(file) - 9);
    unsigned char* compressed = malloc(size);
    fseek(file, 0, SEEK_SET);
    fread(compressed, size, 1, file);
    
    // Decompressed the data
    unsigned char decompressed[SONG_DECOMPRESSED_SIZE];
    lsdj_decompress(compressed, 0, BLOCK_SIZE, decompressed);
    
    // Read in the song
    if (project->song == NULL)
        project->song = malloc(sizeof(lsdj_song_t));
    lsdj_read_song_from_memory(decompressed, sizeof(lsdj_song_t), project->song, error);
}

void lsdj_clear_project(lsdj_project_t* project)
{
    memset(project->name, 0, PROJECT_NAME_LENGTH);
    project->version = 0;
    
    if (project->song)
    {
        free(project->song);
        project->song = NULL;
    }
}
