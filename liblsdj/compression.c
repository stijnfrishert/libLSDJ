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

void decompress_rle_byte(lsdj_vio_t* rvio, lsdj_vio_t* wvio)
{
    unsigned char byte;
    rvio->read(&byte, 1, rvio->user_data);
    if (byte == RUN_LENGTH_ENCODING_BYTE)
    {
        wvio->write(&byte, 1, wvio->user_data);
    }
    else
    {
        unsigned char count = 0;
        rvio->read(&count, 1, rvio->user_data);
        for (int i = 0; i < count; ++i)
            wvio->write(&byte, 1, wvio->user_data);
    }
}

void decompress_default_wave_byte(lsdj_vio_t* rvio, lsdj_vio_t* wvio)
{
    unsigned char count = 0;
    rvio->read(&count, 1, rvio->user_data);
    for (int i = 0; i < count; ++i)
        wvio->write(DEFAULT_WAVE, sizeof(DEFAULT_WAVE), wvio->user_data);
}

void decompress_default_instrument_byte(lsdj_vio_t* rvio, lsdj_vio_t* wvio)
{
    unsigned char count = 0;
    rvio->read(&count, 1, rvio->user_data);
    
    for (int i = 0; i < count; ++i)
        wvio->write(DEFAULT_INSTRUMENT, sizeof(DEFAULT_INSTRUMENT), wvio->user_data);
}

void decompress_sa_byte(lsdj_vio_t* rvio, long firstBlockOffset, size_t blockSize, lsdj_vio_t* wvio, int* reading)
{
    unsigned char byte = 0;
    rvio->read(&byte, 1, rvio->user_data);
    switch (byte)
    {
        case SPECIAL_ACTION_BYTE:
            wvio->write(&byte, 1, wvio->user_data);
            break;
        case DEFAULT_WAVE_BYTE:
            decompress_default_wave_byte(rvio, wvio);
            break;
        case DEFAULT_INSTRUMENT_BYTE:
            decompress_default_instrument_byte(rvio, wvio);
            break;
        case END_OF_FILE_BYTE:
            *reading = 0;
            break;
        default:
            rvio->seek(firstBlockOffset + (long)(byte * blockSize), SEEK_SET, rvio->user_data);
            break;
    }
}

void lsdj_decompress(lsdj_vio_t* rvio, lsdj_vio_t* wvio, long firstBlockOffset, size_t blockSize)
{
    long start = wvio->tell(wvio->user_data);
    
    unsigned char byte = 0;
    
    int reading = 1;
    while (reading == 1)
    {
//        int cur = (int)(wvio->tell(wvio->user_data) - start);
//        printf("%#04x\n", cur);
        
        rvio->read(&byte, 1, rvio->user_data);
        
        switch (byte)
        {
            case RUN_LENGTH_ENCODING_BYTE:
                decompress_rle_byte(rvio, wvio);
                break;
                
            case SPECIAL_ACTION_BYTE:
                decompress_sa_byte(rvio, firstBlockOffset, blockSize, wvio, &reading);
                break;
                
            default:
                wvio->write(&byte, 1, wvio->user_data);
                break;
        }
    }

    assert((wvio->tell(wvio->user_data) - start) == SONG_DECOMPRESSED_SIZE);
}

unsigned int lsdj_compress(const unsigned char* data, unsigned int blockSize, unsigned char startBlock, unsigned int blockCount, lsdj_vio_t* wvio)
{
    unsigned char nextEvent[3] = { 0, 0, 0 };
    unsigned short eventSize = 0;
    
    unsigned char currentBlock = startBlock;
    unsigned int currentBlockSize = 0;
    
    unsigned char byte = 0;
    
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
            byte = SPECIAL_ACTION_BYTE;
            wvio->write(&byte, 1, wvio->user_data);
            byte = DEFAULT_WAVE_BYTE;
            wvio->write(&byte, 1, wvio->user_data);
            wvio->write(&defaultWaveLengthCount, 1, wvio->user_data);
            currentBlockSize += 3;
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
            byte = SPECIAL_ACTION_BYTE;
            wvio->write(&byte, 1, wvio->user_data);
            byte = DEFAULT_INSTRUMENT_BYTE;
            wvio->write(&byte, 1, wvio->user_data);
            wvio->write(&defaultInstrumentLengthCount, 1, wvio->user_data);
            currentBlockSize += 3;
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
        
        if (currentBlockSize >= blockSize - eventSize - 1)
        {
            byte = SPECIAL_ACTION_BYTE;
            wvio->write(&byte, 1, wvio->user_data);
            byte = currentBlock + 1;
            wvio->write(&byte, 1, wvio->user_data);
            
            byte = 0;
            for (currentBlockSize += 2; currentBlockSize < blockSize; currentBlockSize++)
                wvio->write(&byte, 1, wvio->user_data);
            
            currentBlock += 1;
            currentBlockSize = 0;
            continue;
        }
        
        wvio->write(nextEvent, eventSize, wvio->user_data);
        currentBlockSize += eventSize;
        nextEvent[0] = nextEvent[1] = nextEvent[2] = 0;
        eventSize = 0;
    }
    
    byte = SPECIAL_ACTION_BYTE;
    wvio->write(&byte, 1, wvio->user_data);
    byte = END_OF_FILE_BYTE;
    wvio->write(&byte, 1, wvio->user_data);
    
    if (currentBlockSize > 0)
    {
        byte = 0;
        for (currentBlockSize += 2; currentBlockSize < blockSize; currentBlockSize++)
            wvio->write(&byte, 1, wvio->user_data);
    }
    
    return currentBlock - startBlock;
}
