#ifndef LSDJ_PROJECT_H
#define LSDJ_PROJECT_H

#include <stddef.h>

#include "error.h"
#include "song.h"
#include "vio.h"

#ifdef __cplusplus
extern "C" {
#endif
    
// The length of project names
static const unsigned short PROJECT_NAME_LENGTH = 8;

// Representation of a project within an LSDJ sav file
typedef struct
{
    // The name of the project
	char name[PROJECT_NAME_LENGTH];
    
    // The version of the project
    unsigned char version;
    
    // The project decompressed in memory
    lsdj_song_t* song;
} lsdj_project_t;
    
// Deserialize a project from LSDSNG
void lsdj_read_lsdsng(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_project_t* project, lsdj_error_t** error);
void lsdj_read_lsdsng_from_file(const char* path, lsdj_project_t* project, lsdj_error_t** error);
void lsdj_read_lsdsng_from_memory(const unsigned char* data, size_t size, lsdj_project_t* project, lsdj_error_t** error);
    
// Write a project to an lsdsng file
void lsdj_write_lsdsng(const lsdj_project_t* project, lsdj_vio_write_t write, void* user_data, lsdj_error_t** error);
void lsdj_write_lsdsng_to_file(const lsdj_project_t* project, const char* path, lsdj_error_t** error);
void lsdj_write_lsdsng_to_memory(const lsdj_project_t* project, unsigned char* data, size_t size, lsdj_error_t** error);
    
// Clear an LSDJ project
void lsdj_clear_project(lsdj_project_t* project);

#ifdef __cplusplus
}
#endif

#endif
