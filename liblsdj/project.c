#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compression.h"
#include "project.h"

#define BLOCK_SIZE 0x200
#define BLOCK_COUNT 191

struct lsdj_project_t
{
    // The name of the project
    char name[PROJECT_NAME_LENGTH];
    
    // The version of the project
    unsigned char version;
    
    // The song belonging to this project
    /*! If this is NULL, the project isn't in use */
    lsdj_song_t* song;
};

lsdj_project_t* alloc_project(lsdj_error_t** error)
{
    lsdj_project_t* project = (lsdj_project_t*)calloc(sizeof(lsdj_project_t), 1);
    if (project == NULL)
    {
        lsdj_create_error(error, "could not allocate project");
        return NULL;
    }
    
    return project;
}

lsdj_project_t* lsdj_new_project(lsdj_error_t** error)
{
    lsdj_project_t* project = alloc_project(error);
    if (project == NULL)
        return NULL;
    
    memset(project->name, '\0', sizeof(project->name));
    project->version = 0;
    project->song = NULL;
    
    return project;
}

void lsdj_free_project(lsdj_project_t* project)
{
    free(project);
}

lsdj_project_t* lsdj_read_lsdsng(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_error_t** error)
{
    lsdj_project_t* project = alloc_project(error);
    
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
    
    // Read in the song
    if (project->song == NULL)
        project->song = lsdj_read_song_from_memory(decompressed, sizeof(decompressed), error);
    
    return project;
}

lsdj_project_t* lsdj_read_lsdsng_from_file(const char* path, lsdj_error_t** error)
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
    
    lsdj_project_t* project = lsdj_read_lsdsng(lsdj_fread, lsdj_ftell, lsdj_fseek, file, error);
    
    fclose(file);
    
    return project;
}

lsdj_project_t* lsdj_read_lsdsng_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error)
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
    
    return lsdj_read_lsdsng(lsdj_mread, lsdj_mtell, lsdj_mseek, &mem, error);
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
    
    FILE* file = fopen(path, "wb");
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

void lsdj_project_set_name(lsdj_project_t* project, const char* data, size_t size)
{
    strncpy(project->name, data, size < PROJECT_NAME_LENGTH ? size : PROJECT_NAME_LENGTH);
}

void lsdj_project_get_name(const lsdj_project_t* project, char* data, size_t size)
{
    const size_t len = strnlen(project->name, PROJECT_NAME_LENGTH);
    strncpy(data, project->name, len);
    if (len < size)
        data[len] = '\0';
}

void lsdj_project_set_version(lsdj_project_t* project, unsigned char version)
{
    project->version = version;
}

unsigned char lsdj_project_get_version(const lsdj_project_t* project)
{
    return project->version;
}

void lsdj_project_set_song(lsdj_project_t* project, lsdj_song_t* song)
{
    if (project->song)
        free(project->song);
    
    project->song = song;
}

lsdj_song_t* lsdj_project_get_song(const lsdj_project_t* project)
{
    return project->song;
}
