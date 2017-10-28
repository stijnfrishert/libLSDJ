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

lsdj_project_t* lsdj_read_lsdsng(lsdj_vio_t* vio, lsdj_error_t** error)
{
    lsdj_project_t* project = alloc_project(error);
    
    vio->read(project->name, PROJECT_NAME_LENGTH, vio->user_data);
    vio->read(&project->version, 1, vio->user_data);

    // Decompress the data
    const long firstBlockOffset = vio->tell(vio->user_data);
    unsigned char decompressed[SONG_DECOMPRESSED_SIZE];
    memset(decompressed, 0, sizeof(decompressed));
    
    lsdj_memory_data_t mem;
    mem.begin = mem.cur = decompressed;
    mem.size = sizeof(decompressed);
    
    lsdj_vio_t wvio;
    wvio.write = lsdj_fwrite;
    wvio.tell = lsdj_ftell;
    wvio.seek = lsdj_fseek;
    wvio.user_data = &mem;
    
    lsdj_decompress(vio, &wvio, firstBlockOffset, BLOCK_SIZE);
    
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
    
    lsdj_vio_t vio;
    vio.read = lsdj_fread;
    vio.tell = lsdj_ftell;
    vio.seek = lsdj_fseek;
    vio.user_data = file;
    
    lsdj_project_t* project = lsdj_read_lsdsng(&vio, error);
    
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
    
    lsdj_vio_t vio;
    vio.read = lsdj_mread;
    vio.tell = lsdj_mtell;
    vio.seek = lsdj_mseek;
    vio.user_data = &mem;
    
    return lsdj_read_lsdsng(&vio, error);
}

void lsdj_write_lsdsng(const lsdj_project_t* project, lsdj_vio_t* vio, lsdj_error_t** error)
{
    if (project->song == NULL)
        return lsdj_create_error(error, "project does not contain a song");
    
    vio->write(project->name, PROJECT_NAME_LENGTH, vio->user_data);
    vio->write(&project->version, 1, vio->user_data);
    
    // Write the song to memory
    unsigned char decompressed[SONG_DECOMPRESSED_SIZE];
    lsdj_write_song_to_memory(project->song, decompressed, SONG_DECOMPRESSED_SIZE, error);
    
    // Compress the song
    lsdj_compress(decompressed, BLOCK_SIZE, 0, BLOCK_COUNT, vio);
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
    
    lsdj_vio_t vio;
    vio.write = lsdj_fwrite;
    vio.tell = lsdj_ftell;
    vio.seek = lsdj_fseek;
    vio.user_data = file;
    
    lsdj_write_lsdsng(project, &vio, error);
    
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
    
    lsdj_vio_t vio;
    vio.write = lsdj_mwrite;
    vio.tell = lsdj_mtell;
    vio.seek = lsdj_mseek;
    vio.user_data = &mem;
    
    lsdj_write_lsdsng(project, &vio, error);
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
