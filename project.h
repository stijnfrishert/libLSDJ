#ifndef LSDJ_PROJECT_H
#define LSDJ_PROJECT_H

#include <stddef.h>

#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif
    
//! Structure containing a compressed LSDJ project
typedef struct
{
    size_t size;
    void* data;
} lsdj_project_compressed_t;

//! Representation of a project within an LSDJ sav file
typedef struct
{
    //! The name of the project
	char name[8];
    
    //! The version of the project
    unsigned char version;
    
    //! The project compressed in memory
    lsdj_project_compressed_t compressed_data;
} lsdj_project_t;
    
//! Write a project to file as an .lsdsng
void lsdj_write_lsdsng(lsdj_project_t* project, const char* path, lsdj_error_t** error);

#ifdef __cplusplus
}
#endif

#endif
