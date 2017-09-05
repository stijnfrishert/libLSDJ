#ifndef LSDJ_SAV_H
#define LSDJ_SAV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "error.h"
#include "project.h"

//! Representation of an entire LSDJ sav file
typedef struct
{
	//! The number of projects there are in the sav file
	unsigned short project_count;

	//! The projects in the sav file
	lsdj_project_t* projects;

	//! The project in the SAV that's currently being edited
	/*! Indices start at 0, a value of -1 means there is no active project */
	int active_project;
} lsdj_sav_t;

//! Open an LSDJ sav file
/*! Each successful call to lsdj_open() should be paired with a call to lsdj_close() */
lsdj_sav_t* lsdj_open(const char* path, lsdj_error_t** error);

//! Close an LSDJ sav file
/*! Each successful call to lsdj_open() should be paired with a call to lsdj_close() */
void lsdj_close(lsdj_sav_t* sav);

#ifdef __cplusplus
}
#endif

#endif
