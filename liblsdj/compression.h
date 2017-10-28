#ifndef LSDJ_COMPRESSION_H
#define LSDJ_COMPRESSION_H

#include "vio.h"

// Decompress blocks to a song buffer
void lsdj_decompress(lsdj_vio_t* rvio, lsdj_vio_t* wvio, long firstBlockOffset, size_t blockSize);

// Compress a song buffer to a set of blocks
/*! Returns the amount of blocks written */
unsigned int lsdj_compress(const unsigned char* data, unsigned int blockSize, unsigned char startBlock, unsigned int blockCount, lsdj_vio_t* wvio);

#endif
