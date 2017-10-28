#ifndef LSDJ_COMPRESSION_H
#define LSDJ_COMPRESSION_H

#include "vio.h"

// Decompress blocks to a song buffer
void lsdj_decompress(lsdj_vio_read_t read, lsdj_vio_seek_t seek, lsdj_vio_tell_t tell, void* read_data, long firstBlockOffset, size_t blockSize, unsigned char* write);

// Compress a song buffer to a set of blocks
/*! Returns the amount of blocks written */
unsigned int lsdj_compress(const unsigned char* data, unsigned int blockSize, unsigned char startBlock, unsigned int blockCount, lsdj_vio_write_t write, void* user_data);

#endif
