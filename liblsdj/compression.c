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
#include "song.h"

static const unsigned char RUN_LENGTH_ENCODING_BYTE = 0xC0;
static const unsigned char SPECIAL_ACTION_BYTE = 0xE0;
static const unsigned char END_OF_FILE_BYTE = 0xFF;
static const unsigned char DEFAULT_WAVE_BYTE = 0xF0;
static const unsigned char DEFAULT_INSTRUMENT_BYTE = 0xF1;

void lsdj_decompress(lsdj_vio_read_t read, lsdj_vio_seek_t seek, lsdj_vio_tell_t tell, void* user_data, long begin, size_t blockSize, unsigned char* write)
{    
    unsigned char byte = 0;
    
    int reading = 1;
    while (reading == 1)
    {
        read(&byte, 1, user_data);
        switch (byte)
        {
            case RUN_LENGTH_ENCODING_BYTE:
            {
                read(&byte, 1, user_data);
                if (byte == RUN_LENGTH_ENCODING_BYTE)
                {
                    *write ++ = RUN_LENGTH_ENCODING_BYTE;
                }
                else
                {
                    unsigned char count = 0;
                    read(&count, 1, user_data);
                    for (int i = 0; i < count; ++i)
                        *write++ = byte;
                }
                break;
            }
                
            case SPECIAL_ACTION_BYTE:
            {
                read(&byte, 1, user_data);
                switch (byte)
                {
                    case SPECIAL_ACTION_BYTE:
                        *write++ = SPECIAL_ACTION_BYTE;
                        break;
                    case DEFAULT_WAVE_BYTE:
                    {
                        unsigned char count = 0;
                        read(&count, 1, user_data);
                        for (int i = 0; i < count; ++i)
                        {
                            for (int j = 0; j < 16; ++j)
                                *write++ = DEFAULT_WAVE[j];
                        }
                        break;
                    }
                    case DEFAULT_INSTRUMENT_BYTE:
                    {
                        unsigned char count = 0;
                        read(&count, 1, user_data);
                        for (int i = 0; i < count; ++i)
                        {
                            for (int j = 0; j < 16; ++j)
                                *write++ = DEFAULT_INSTRUMENT[j];
                        }
                        break;
                    }
                    case END_OF_FILE_BYTE:
                        reading = 0;
                        break;
                    default:
                        seek(begin + ((long)byte - 1) * (long)blockSize, SEEK_SET, user_data);
                        break;
                }
                break;
            }
                
            default:
                *write++ = byte;
                break;
        }
    }
}

//void lsdj_decompress(const unsigned char* blocks, unsigned char start_block, unsigned int block_size, unsigned char* write)
//{
//    for (const unsigned char* read = blocks + start_block * block_size; *read != END_OF_FILE_BYTE; )
//    {
//        switch (*read)
//        {
//            case RUN_LENGTH_ENCODING_BYTE:
//            {
//                unsigned char c = *++read;
//                if (c == RUN_LENGTH_ENCODING_BYTE)
//                {
//                    *write++ = RUN_LENGTH_ENCODING_BYTE;
//                } else {
//                    unsigned char count = *++read;
//                    for (int i = 0; i < count; ++i)
//                        *write++ = c;
//                }
//                read++;
//                break;
//            }
//            case SPECIAL_ACTION_BYTE:
//            {
//                unsigned char c = *++read;
//                switch (c)
//                {
//                    case SPECIAL_ACTION_BYTE:
//                        *write++ = SPECIAL_ACTION_BYTE;
//                        read++;
//                        break;
//                    case DEFAULT_WAVE_BYTE:
//                    {
//                        unsigned char count = *++read;
//                        for (int i = 0; i < count; ++i)
//                        {
//                            for (int j = 0; j < 16; ++j)
//                                *write++ = DEFAULT_WAVE[j];
//                        }
//                        read++;
//                        break;
//                    }
//                    case DEFAULT_INSTRUMENT_BYTE:
//                    {
//                        unsigned char count = *++read;
//                        for (int i = 0; i < count; ++i)
//                        {
//                            for (int j = 0; j < 16; ++j)
//                                *write++ = DEFAULT_INSTRUMENT[j];
//                        }
//                        read++;
//                        break;
//                    }
//                    case END_OF_FILE_BYTE:
//                        break;
//                    default:
//                        read = blocks + (c - 1) * block_size;
//                        break;
//                }
//                break;
//            }
//            default:
//                *write++ = *read++;
//                break;
//        }
//    }
//}

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
