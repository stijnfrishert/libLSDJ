#ifndef LSDJ_COMPRESSION_H
#define LSDJ_COMPRESSION_H

#include "vio.h"

// Decompress blocks to a song buffer
void lsdj_decompress(lsdj_vio_read_t read, lsdj_vio_seek_t seek, lsdj_vio_tell_t tell, void* user_data, long begin, size_t blockSize, unsigned char* write);
//void lsdj_decompress(const unsigned char* blocks, unsigned char start_block, unsigned int block_size, unsigned char* destination);

// Compress a song buffer to a set of blocks
/*! Returns the amount of blocks written */
unsigned int lsdj_compress(const unsigned char* data, unsigned char* blocks, unsigned int block_size, unsigned int start_block, unsigned int block_count);

#endif
