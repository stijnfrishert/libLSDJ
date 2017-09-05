#ifndef LSDJ_PROJECT_H
#define LSDJ_PROJECT_H

#ifdef __cplusplus
extern "C" {
#endif
    
const unsigned int BLOCK_SIZE = 0x200;
    
typedef struct
{
    void* data;
} lsdj_block_t;
    
typedef struct
{
    unsigned int block_count;
    lsdj_block_t* blocks;
} lsdj_project_compressed_t;

//! Representation of a project within an LSDJ sav file
typedef struct
{
	char name[8];
    unsigned char version;
    
    lsdj_project_compressed_t compressed_data;
} lsdj_project_t;

#ifdef __cplusplus
}
#endif

#endif
