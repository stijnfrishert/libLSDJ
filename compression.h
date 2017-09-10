//
//  compression.h
//  lsdj
//
//  Created by Stijn Frishert on 9/10/17.
//
//

#ifndef LSDJ_COMPRESSION_H
#define LSDJ_COMPRESSION_H

//! The size of a block in the LSDJ file system
static const unsigned int BLOCK_SIZE = 0x200;

//! The amount of blocks in the LSDJ file system
static const unsigned int BLOCK_COUNT = 191;

//! Decompress blocks to a song buffer
void decompress(const unsigned char blocks[BLOCK_COUNT][BLOCK_SIZE], unsigned char start_block, unsigned char* write);

#endif /* LSDJ_COMPRESSION_H */
