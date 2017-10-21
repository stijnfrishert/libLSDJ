//
//  compression.h
//  lsdj
//
//  Created by Stijn Frishert on 9/10/17.
//
//

#ifndef LSDJ_COMPRESSION_H
#define LSDJ_COMPRESSION_H

//! Decompress blocks to a song buffer
void lsdj_decompress(const unsigned char* blocks, unsigned char start_block, unsigned int block_size, unsigned char* destination);

//! Compress a song buffer to a set of blocks
/*! Returns the amount of blocks written */
unsigned int lsdj_compress(const unsigned char* data, unsigned char* blocks, unsigned int block_size, unsigned int start_block, unsigned int block_count);

#endif /* LSDJ_COMPRESSION_H */
