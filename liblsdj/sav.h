#ifndef LSDJ_SAV_H
#define LSDJ_SAV_H

#include "error.h"
#include "project.h"
#include "song.h"
#include "vio.h"
    
// The amount of projects in a sav
static const unsigned short PROJECT_COUNT = 32;
    
// Representation of an entire LSDJ save file
typedef struct
{
	// The projects
	lsdj_project_t projects[PROJECT_COUNT];

	// Index of the project that is currently being edited
	/*! Indices start at 0, a value of 0xFF means there is no active project */
	unsigned char activeProject;
    
    // The song in active working memory
    lsdj_song_t song;
} lsdj_sav_t;

// Create/free saves
void lsdj_init_sav(lsdj_sav_t* sav);
    
// Deserialize a sav
void lsdj_read_sav(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_sav_t* sav, lsdj_error_t** error);
void lsdj_read_sav_from_file(const char* path, lsdj_sav_t* sav, lsdj_error_t** error);
void lsdj_read_sav_from_memory(const unsigned char* data, size_t size, lsdj_sav_t* sav, lsdj_error_t** error);
    
// Serialize a sav
void lsdj_write_sav(const lsdj_sav_t* sav, lsdj_vio_write_t write, void* user_data, lsdj_error_t** error);
void lsdj_write_sav_to_file(const lsdj_sav_t* sav, const char* path, lsdj_error_t** error);
void lsdj_write_sav_to_memory(const lsdj_sav_t* sav, unsigned char* data, size_t size, lsdj_error_t** error);
    
// Clear all sav data to factory settings
void lsdj_clear_sav(lsdj_sav_t* sav);

#endif
