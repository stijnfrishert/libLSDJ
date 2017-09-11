//
//  compression.c
//  lsdj
//
//  Created by Stijn Frishert on 9/10/17.
//
//

#include <assert.h>
#include <string.h>

#include "compression.h"

static const unsigned char RUN_LENGTH_ENCODING_BYTE = 0xC0;
static const unsigned char SPECIAL_ACTION_BYTE = 0xE0;
static const unsigned char END_OF_FILE_BYTE = 0xFF;
static const unsigned char DEFAULT_WAVE_BYTE = 0xF0;
static const unsigned char DEFAULT_INSTRUMENT_BYTE = 0xF1;

static const unsigned char DEFAULT_WAVE[16] = { 0x8E, 0xCD, 0xCC, 0xBB, 0xAA, 0xA9, 0x99, 0x88, 0x87, 0x76, 0x66, 0x55, 0x54, 0x43, 0x32, 0x31 };
static const unsigned char DEFAULT_INSTRUMENT[16] = { 0xA8, 0, 0, 0xFF, 0, 0, 3, 0, 0, 0xD0, 0, 0, 0, 0xF3, 0, 0 };

void lsdj_decompress(const unsigned char* blocks, unsigned char start_block, unsigned int block_size, unsigned char* write)
{
    for (const unsigned char* read = blocks + start_block * block_size; *read != END_OF_FILE_BYTE; )
    {
        switch (*read)
        {
            case RUN_LENGTH_ENCODING_BYTE:
            {
                unsigned char c = *++read;
                if (c == RUN_LENGTH_ENCODING_BYTE)
                {
                    *write++ = RUN_LENGTH_ENCODING_BYTE;
                } else {
                    unsigned char count = *++read;
                    for (int i = 0; i < count; ++i)
                        *write++ = c;
                }
                read++;
                break;
            }
            case SPECIAL_ACTION_BYTE:
            {
                unsigned char c = *++read;
                switch (c)
                {
                    case SPECIAL_ACTION_BYTE:
                        *write++ = SPECIAL_ACTION_BYTE;
                        read++;
                        break;
                    case DEFAULT_WAVE_BYTE:
                    {
                        unsigned char count = *++read;
                        for (int i = 0; i < count; ++i)
                        {
                            for (int j = 0; j < 16; ++j)
                                *write++ = DEFAULT_WAVE[j];
                        }
                        read++;
                        break;
                    }
                    case DEFAULT_INSTRUMENT_BYTE:
                    {
                        unsigned char count = *++read;
                        for (int i = 0; i < count; ++i)
                        {
                            for (int j = 0; j < 16; ++j)
                                *write++ = DEFAULT_INSTRUMENT[j];
                        }
                        read++;
                        break;
                    }
                    case END_OF_FILE_BYTE:
                        break;
                    default:
                        read = blocks + (c - 1) * block_size;
                        break;
                }
                break;
            }
            default:
                *write++ = *read++;
                break;
        }
    }
}

unsigned int lsdj_compress(const unsigned char* data, unsigned char* blocks, unsigned int block_size, unsigned int start_block, unsigned int block_count)
{
    unsigned int current_block = start_block;
    unsigned char* block = blocks + current_block * block_size;
    unsigned char* write = block;
    
    for (const unsigned char* read = data; read < data + SONG_DECOMPRESSED_SIZE; )
    {
        if (write - block >= block_size - 4)
        {
            current_block += 1;
            *write++ = SPECIAL_ACTION_BYTE;
            *write++ = (unsigned char)(current_block + 1);
            
            write = block = blocks + current_block * block_size;
            continue;
        }
        
        switch (*read)
        {
            case RUN_LENGTH_ENCODING_BYTE:
                *write++ = RUN_LENGTH_ENCODING_BYTE;
                *write++ = RUN_LENGTH_ENCODING_BYTE;
                read++;
                break;
                
            case SPECIAL_ACTION_BYTE:
                *write++ = SPECIAL_ACTION_BYTE;
                *write++ = SPECIAL_ACTION_BYTE;
                read++;
                break;
                
            default:
            {
                unsigned char c = *read;
                
                // See if we can do run-length encoding
                if ((read + 3 < data + SONG_DECOMPRESSED_SIZE) &&
                    *(read + 1) == c &&
                    *(read + 2) == c &&
                    *(read + 3) == c)
                {
                    *write++ = RUN_LENGTH_ENCODING_BYTE;
                    *write++ = c;
                    
                    *write = 0;
                    for ( ; read < data + SONG_DECOMPRESSED_SIZE; ++read)
                    {
                        if (*read != c || *write == 0xFF)
                            break;
                        
                        *write += 1;
                    }
                    write++;
                } else {
                    *write++ = *read++;
                }
                
                break;
            }
        }
    }
    
    *write++ = SPECIAL_ACTION_BYTE;
    *write++ = END_OF_FILE_BYTE;
    
    return current_block - start_block + 1;
}
