#ifndef LSDJ_PROJECT_H
#define LSDJ_PROJECT_H

#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif
    
static const unsigned int BLOCK_SIZE = 0x200;
    
typedef struct
{
    unsigned char block_count;
    void* data;
} lsdj_project_compressed_t;

//! Representation of a project within an LSDJ sav file
typedef struct
{
	char name[8];
    unsigned char version;
    
    lsdj_project_compressed_t compressed_data;
} lsdj_project_t;
    
void lsdj_write_lsdsng(lsdj_project_t* project, const char* path, lsdj_error_t** error);

#ifdef __cplusplus
}
#endif

#endif
