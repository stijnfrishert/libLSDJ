#ifndef LSDJ_SAV_H
#define LSDJ_SAV_H

#include "error.h"
#include "project.h"

#ifdef __cplusplus
extern "C" {
#endif
    
//! Representation of an entire LSDJ save file
typedef struct
{
//	//! The number of projects on file
//	unsigned short project_count;

	//! The projects
	lsdj_project_t projects[32];

	//! Index of the project that is currently being edited
	/*! Indices start at 0, a value of 0xFF means there is no active project */
	unsigned char active_project;
    
    //! The project in active working memory
    lsdj_project_decompressed_t working_project;
} lsdj_sav_t;

//! Reads an LSDJ save file into memory
/*! Each successful call to lsdj_open() should be paired with a call to lsdj_free() */
lsdj_sav_t* lsdj_open_sav(const char* path, lsdj_error_t** error);
    
//! Write am LSDJ save file to file
void lsdj_write_sav(const lsdj_sav_t* sav, const char* path, lsdj_error_t** error);

//! Deallocate an LSDJ save file
void lsdj_free_sav(lsdj_sav_t* sav);

#ifdef __cplusplus
}
#endif

#endif
