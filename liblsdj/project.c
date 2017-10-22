#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compression.h"
#include "project.h"

static const unsigned int BLOCK_SIZE = 0x200;
static const unsigned int BLOCK_COUNT = 191;

void lsdj_init_project(lsdj_project_t* project)
{
    memset(project->name, 0, sizeof(project->name));
    project->version = 0;
    project->song = NULL;
}

void lsdj_read_lsdsng(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_project_t* project, lsdj_error_t** error)
{
    read(project->name, PROJECT_NAME_LENGTH, user_data);
    read(&project->version, 1, user_data);

    // Read the compressed data
    seek(0, SEEK_END, user_data);
    const size_t size = (size_t)(tell(user_data) - 9);
    unsigned char* compressed = (unsigned char*)malloc(size);
    seek(0, SEEK_SET, user_data);
    read(compressed, size, user_data);

    // Decompressed the data
    unsigned char decompressed[SONG_DECOMPRESSED_SIZE];
    lsdj_memory_data_t mem;
    mem.cur = mem.begin = compressed;
    mem.size = size;
    lsdj_decompress(lsdj_mread, lsdj_mseek, lsdj_mtell, &mem, 0, BLOCK_SIZE, decompressed);
    
//    lsdj_decompress(compressed, 0, BLOCK_SIZE, decompressed);

    // Read in the song
    if (project->song == NULL)
        project->song = (lsdj_song_t*)malloc(sizeof(lsdj_song_t));
    lsdj_read_song_from_memory(decompressed, sizeof(lsdj_song_t), project->song, error);
}

void lsdj_read_lsdsng_from_file(const char* path, lsdj_project_t* project, lsdj_error_t** error)
{
    if (path == NULL)
        return lsdj_create_error(error, "path is NULL");
    
    if (project == NULL)
        return lsdj_create_error(error, "project is NULL");
    
    FILE* file = fopen(path, "r");
    if (file == NULL)
        return lsdj_create_error(error, "could not open file for reading");
    
    lsdj_read_lsdsng(lsdj_fread, lsdj_ftell, lsdj_fseek, file, project, error);
    
    fclose(file);
}

void lsdj_read_lsdsng_from_memory(const unsigned char* data, size_t size, lsdj_project_t* project, lsdj_error_t** error)
{
    if (data == NULL)
        return lsdj_create_error(error, "data is NULL");
    
    if (project == NULL)
        return lsdj_create_error(error, "project is NULL");
    
    lsdj_memory_data_t mem;
    mem.begin = (unsigned char*)data;
    mem.cur = mem.begin;
    mem.size = size;
    
    lsdj_read_lsdsng(lsdj_mread, lsdj_mtell, lsdj_mseek, &mem, project, error);
}

void lsdj_write_lsdsng(const lsdj_project_t* project, lsdj_vio_write_t write, void* user_data, lsdj_error_t** error)
{
    if (project->song == NULL)
        return lsdj_create_error(error, "project does not contain a song");
    
    write(project->name, PROJECT_NAME_LENGTH, user_data);
    write(&project->version, 1, user_data);
    
    // Write the song to memory
    unsigned char decompressed[SONG_DECOMPRESSED_SIZE];
    lsdj_write_song_to_memory(project->song, decompressed, SONG_DECOMPRESSED_SIZE, error);
    
    // Compress the song
    unsigned char compressed[SONG_DECOMPRESSED_SIZE];
    memset(&compressed, 0, SONG_DECOMPRESSED_SIZE);
    unsigned int block_write_count = lsdj_compress(decompressed, compressed, BLOCK_SIZE, 0, BLOCK_COUNT);
    
    write(compressed, BLOCK_SIZE * block_write_count, user_data);
}

void lsdj_write_lsdsng_to_file(const lsdj_project_t* project, const char* path, lsdj_error_t** error)
{
    if (path == NULL)
        return lsdj_create_error(error, "path is NULL");
    
    if (project == NULL)
        return lsdj_create_error(error, "project is NULL");
    
    FILE* file = fopen(path, "w");
    if (file == NULL)
        return lsdj_create_error(error, "could not open file for writing");
    
    lsdj_write_lsdsng(project, lsdj_fwrite, file, error);
    
    fclose(file);
}

void lsdj_write_lsdsng_to_memory(const lsdj_project_t* project, unsigned char* data, size_t size, lsdj_error_t** error)
{
    if (project == NULL)
        return lsdj_create_error(error, "project is NULL");
    
    if (data == NULL)
        return lsdj_create_error(error, "data is NULL");
    
    lsdj_memory_data_t mem;
    mem.begin = data;
    mem.cur = mem.begin;
    mem.size = size;
    
    lsdj_write_lsdsng(project, lsdj_mwrite, &mem, error);
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
