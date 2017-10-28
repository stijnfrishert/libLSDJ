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

#define RUN_LENGTH_ENCODING_BYTE 0xC0
#define SPECIAL_ACTION_BYTE 0xE0
#define END_OF_FILE_BYTE 0xFF
#define DEFAULT_WAVE_BYTE 0xF0
#define DEFAULT_INSTRUMENT_BYTE 0xF1

void lsdj_decompress(lsdj_vio_read_t read, lsdj_vio_seek_t seek, lsdj_vio_tell_t tell, void* user_data, long begin, size_t blockSize, unsigned char* write)
{
    unsigned char* start = write;
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
    
    assert((write - start) == SONG_DECOMPRESSED_SIZE);
}

unsigned int lsdj_compress(const unsigned char* data, unsigned char* blocks, unsigned int block_size, unsigned int start_block, unsigned int block_count)
{
    unsigned int current_block = start_block;
    unsigned char* block = blocks + current_block * block_size;
    unsigned char* write = block;
    
    
    unsigned char nextEvent[3] = { 0, 0, 0 };
    unsigned short eventSize = 0;
    
    const unsigned char* end = data + SONG_DECOMPRESSED_SIZE;
    for (const unsigned char* read = data; read < end; )
    {
        // Are we reading a default wave? If so, we can compress these!
        unsigned char defaultWaveLengthCount = 0;
        while (read + WAVE_LENGTH < end && memcmp(read, DEFAULT_WAVE, WAVE_LENGTH) == 0)
        {
            read += WAVE_LENGTH;
            ++defaultWaveLengthCount;
        }
        
        if (defaultWaveLengthCount > 0)
        {
            *write++ = SPECIAL_ACTION_BYTE;
            *write++ = DEFAULT_WAVE_BYTE;
            *write++ = defaultWaveLengthCount;
            continue;
        }
        
        // Are we reading a default instrument? If so, we can compress these!
        unsigned char defaultInstrumentLengthCount = 0;
        while (read + DEFAULT_INSTRUMENT_LENGTH < end && memcmp(read, DEFAULT_INSTRUMENT, DEFAULT_INSTRUMENT_LENGTH) == 0)
        {
            read += DEFAULT_INSTRUMENT_LENGTH;
            ++defaultInstrumentLengthCount;
        }
        
        if (defaultInstrumentLengthCount > 0)
        {
            *write++ = SPECIAL_ACTION_BYTE;
            *write++ = DEFAULT_INSTRUMENT_BYTE;
            *write++ = defaultInstrumentLengthCount;
            continue;
        }
        
        // Not a default wave, time to do "normal" compression
        switch (*read)
        {
            case RUN_LENGTH_ENCODING_BYTE:
                nextEvent[0] = RUN_LENGTH_ENCODING_BYTE;
                nextEvent[1] = RUN_LENGTH_ENCODING_BYTE;
                eventSize = 2;
                read++;
                break;
                
            case SPECIAL_ACTION_BYTE:
                nextEvent[0] = SPECIAL_ACTION_BYTE;
                nextEvent[1] = SPECIAL_ACTION_BYTE;
                eventSize = 2;
                read++;
                break;
                
            default:
            {
                const unsigned char* beg = read;
                
                unsigned char c = *read;
                
                // See if we can do run-length encoding
                if ((read + 3 < end) &&
                    *(read + 1) == c &&
                    *(read + 2) == c &&
                    *(read + 3) == c)
                {
                    unsigned char count = 0;
                    
                    while (read < end && *read == c && count != 0xFF)
                    {
                        ++count;
                        ++read;
                    }
                    
                    assert((read - beg) == count);
                    
                    nextEvent[0] = RUN_LENGTH_ENCODING_BYTE;
                    nextEvent[1] = c;
                    nextEvent[2] = count;
                    
                    eventSize = 3;
                } else {
                    nextEvent[0] = *read++;
                    eventSize = 1;
                }
                
                break;
            }
        }
        
        if (write - block >= block_size - eventSize - 1)
        {
            *write++ = SPECIAL_ACTION_BYTE;
            *write++ = (unsigned char)(current_block + 1);
            
            current_block += 1;
            write = block = blocks + current_block * block_size;
        }
        
        for (int i = 0; i < eventSize; ++i)
            *write++ = nextEvent[i];
        
        nextEvent[0] = nextEvent[1] = nextEvent[2] = 0;
        eventSize = 0;
    }
    
    *write++ = SPECIAL_ACTION_BYTE;
    *write++ = END_OF_FILE_BYTE;
    
    return current_block - start_block + 1;
}
