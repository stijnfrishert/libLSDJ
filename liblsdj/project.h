#ifndef LSDJ_PROJECT_H
#define LSDJ_PROJECT_H

#include <stddef.h>

#include "error.h"
#include "song.h"
#include "vio.h"

// The length of project names
static const unsigned short PROJECT_NAME_LENGTH = 8;

// Representation of a project within an LSDJ sav file
struct lsdj_project_t;
typedef struct lsdj_project_t lsdj_project_t;

// Create/free projects
lsdj_project_t* lsdj_new_project(lsdj_error_t** error);
void lsdj_free_project(lsdj_project_t* project);
    
// Deserialize a project from LSDSNG
lsdj_project_t* lsdj_read_lsdsng(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_error_t** error);
lsdj_project_t* lsdj_read_lsdsng_from_file(const char* path, lsdj_error_t** error);
lsdj_project_t* lsdj_read_lsdsng_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error);
    
// Write a project to an lsdsng file
void lsdj_write_lsdsng(const lsdj_project_t* project, lsdj_vio_write_t write, void* user_data, lsdj_error_t** error);
void lsdj_write_lsdsng_to_file(const lsdj_project_t* project, const char* path, lsdj_error_t** error);
void lsdj_write_lsdsng_to_memory(const lsdj_project_t* project, unsigned char* data, size_t size, lsdj_error_t** error);

// Change data in a project
void lsdj_project_set_name(lsdj_project_t* project, const char* data, size_t size);
void lsdj_project_get_name(lsdj_project_t* project, char* data, size_t size);
void lsdj_project_set_version(lsdj_project_t* project, unsigned char version);
unsigned char lsdj_project_get_version(lsdj_project_t* project);
void lsdj_project_set_song(lsdj_project_t* project, lsdj_song_t* song);
lsdj_song_t* lsdj_project_get_song(lsdj_project_t* project);

#endif
