#ifndef LSDJ_COMPRESSION_H
#define LSDJ_COMPRESSION_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus */

#include "error.h"
#include "vio.h"

// Decompress blocks to a song buffer
void lsdj_decompress(lsdj_vio_t* rvio, lsdj_vio_t* wvio, long firstBlockOffset, size_t blockSize);
void lsdj_decompress_from_file(const char* path, lsdj_vio_t* wvio, long firstBlockOffset, size_t blockSize, lsdj_error_t** error);

// Compress a song buffer to a set of blocks
/*! Returns the amount of blocks written */
unsigned int lsdj_compress(const unsigned char* data, unsigned int blockSize, unsigned char startBlock, unsigned int blockCount, lsdj_vio_t* wvio);
unsigned int lsdj_compress_to_file(const unsigned char* data, unsigned int blockSize, unsigned char startBlock, unsigned int blockCount, const char* path, lsdj_error_t** error);
    
#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif
