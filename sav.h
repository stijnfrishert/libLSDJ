#ifndef LSDJ_SAV_H
#define LSDJ_SAV_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct lsdj_error_t lsdj_error_t;

typedef struct
{
	const char* name;
} lsdj_project_t;

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

//! Provide liblsdj with a callback to trigger in case of an error
// void lsdj_set_error_callback(void(*callback)(const char*));

//! Retrieve a string description of an error
const char* lsdj_get_error_c_str(lsdj_error_t* error);

//! Free error data returned from an lsdj function call
void lsdj_free_error(lsdj_error_t* error);

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