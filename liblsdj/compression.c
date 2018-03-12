/*
 
 This file is a part of liblsdj, a C library for managing everything
 that has to do with LSDJ, software for writing music (chiptune) with
 your gameboy. For more information, see:
 
 * https://github.com/stijnfrishert/liblsdj
 * http://www.littlesounddj.com
 
 --------------------------------------------------------------------------------
 
 MIT License
 
 Copyright (c) 2018 Stijn Frishert
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 */

#include <assert.h>
#include <string.h>

#include "compression.h"
#include "song.h"

#define RUN_LENGTH_ENCODING_BYTE 0xC0
#define SPECIAL_ACTION_BYTE 0xE0
#define END_OF_FILE_BYTE 0xFF
#define DEFAULT_WAVE_BYTE 0xF0
#define DEFAULT_INSTRUMENT_BYTE 0xF1

static const unsigned char DEFAULT_INSTRUMENT_COMPRESSION[DEFAULT_INSTRUMENT_LENGTH] = { 0xA8, 0, 0, 0xFF, 0, 0, 3, 0, 0, 0xD0, 0, 0, 0, 0xF3, 0, 0 };

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
        wvio->write(DEFAULT_INSTRUMENT_COMPRESSION, sizeof(DEFAULT_INSTRUMENT_COMPRESSION), wvio->user_data);
}

void decompress_sa_byte(lsdj_vio_t* rvio, long* currentBlockPosition, long* block1position, size_t blockSize, lsdj_vio_t* wvio, int* reading)
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
            if (block1position)
                *currentBlockPosition = *block1position + (long)((byte - 1) * blockSize);
            else
                *currentBlockPosition += blockSize;
            
            rvio->seek(*currentBlockPosition, SEEK_SET, rvio->user_data);
            break;
    }
}

void lsdj_decompress(lsdj_vio_t* rvio, lsdj_vio_t* wvio, long* block1position, size_t blockSize)
{
    long wstart = wvio->tell(wvio->user_data);
    long currentBlockPosition = rvio->tell(rvio->user_data);
    
    unsigned char byte = 0;
    
    int reading = 1;
    while (reading == 1)
    {
//        int rcur = (int)(rvio->tell(rvio->user_data));
//        int wcur = (int)(wvio->tell(wvio->user_data) - wstart);
//        printf("%#06x\t%#04x\n", rcur, wcur);
        
        rvio->read(&byte, 1, rvio->user_data);
        
        switch (byte)
        {
            case RUN_LENGTH_ENCODING_BYTE:
                decompress_rle_byte(rvio, wvio);
                break;
                
            case SPECIAL_ACTION_BYTE:
                decompress_sa_byte(rvio, &currentBlockPosition, block1position, blockSize, wvio, &reading);
                break;
                
            default:
                wvio->write(&byte, 1, wvio->user_data);
                break;
        }
    }

    const long readSize = wvio->tell(wvio->user_data) - wstart;
    assert(readSize == SONG_DECOMPRESSED_SIZE);
}

void lsdj_decompress_from_file(const char* path, lsdj_vio_t* wvio, long* firstBlockOffset, size_t blockSize, lsdj_error_t** error)
{
    if (path == NULL)
    {
        lsdj_create_error(error, "path is NULL");
        return;
    }
    
    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        char message[512];
        snprintf(message, 512, "could not open %s for reading", path);
        return lsdj_create_error(error, message);
    }
    
    lsdj_vio_t vio;
    vio.read = lsdj_fread;
    vio.tell = lsdj_ftell;
    vio.seek = lsdj_fseek;
    vio.user_data = file;
    
    lsdj_decompress(&vio, wvio, firstBlockOffset, blockSize);
    
    fclose(file);
}

unsigned int lsdj_compress(const unsigned char* data, unsigned int blockSize, unsigned char startBlock, unsigned int blockCount, lsdj_vio_t* wvio)
{
    if (startBlock == blockCount + 1)
        return 0;
    
    unsigned char nextEvent[3] = { 0, 0, 0 };
    unsigned short eventSize = 0;
    
    unsigned char currentBlock = startBlock;
    unsigned int currentBlockSize = 0;
    
    unsigned char byte = 0;
    
    long wstart = wvio->tell(wvio->user_data);
    
    const unsigned char* end = data + SONG_DECOMPRESSED_SIZE;
    for (const unsigned char* read = data; read < end; )
    {
//        int wcur = (int)(wvio->tell(wvio->user_data) - wstart);
//        printf("read: %#06x\twrite: %#04x\n", (int)(read - data), wcur);
        
        // Are we reading a default wave? If so, we can compress these!
        unsigned char defaultWaveLengthCount = 0;
        while (read + WAVE_LENGTH < end && memcmp(read, DEFAULT_WAVE, WAVE_LENGTH) == 0 && defaultWaveLengthCount != 0xFF)
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
        while (read + DEFAULT_INSTRUMENT_LENGTH < end && memcmp(read, DEFAULT_INSTRUMENT_COMPRESSION, DEFAULT_INSTRUMENT_LENGTH) == 0 && defaultInstrumentLengthCount != 0xFF)
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
        
        // See if the event would still fit in this block
        // If not, move to a new block
        if (currentBlockSize + eventSize >= blockSize - 2)
        {            
            byte = SPECIAL_ACTION_BYTE;
            wvio->write(&byte, 1, wvio->user_data);
            byte = currentBlock + 1;
            wvio->write(&byte, 1, wvio->user_data);
            
            assert(currentBlockSize <= blockSize);
            
            byte = 0;
            for (currentBlockSize += 2; currentBlockSize < blockSize; currentBlockSize++)
                wvio->write(&byte, 1, wvio->user_data);
            
            assert(currentBlockSize == blockSize);
            
            currentBlock += 1;
            currentBlockSize = 0;
            
            // Have we reached the maximum block count?
            // If so, roll back
            if (currentBlock == blockCount + 1)
            {
                long pos = wvio->tell(wvio->user_data);
                wvio->seek(wstart, SEEK_SET, wvio->user_data);
                
                byte = 0;
                for (long i = 0; i < pos - wstart; ++i)
                    wvio->write(&byte, 1, wvio->user_data);
                
                wvio->seek(wstart, SEEK_SET, wvio->user_data);
                
                return 0;
            }
            
            // Don't "continue;" but fall through. We still need to write the event
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
    
    return currentBlock - startBlock + 1;
}

unsigned int lsdj_compress_to_file(const unsigned char* data, unsigned int blockSize, unsigned char startBlock, unsigned int blockCount, const char* path, lsdj_error_t** error)
{
    if (path == NULL)
    {
        lsdj_create_error(error, "path is NULL");
        return 0;
    }
    
    if (data == NULL)
    {
        lsdj_create_error(error, "data is NULL");
        return 0;
    }
    
    FILE* file = fopen(path, "wb");
    if (file == NULL)
    {
        char message[512];
        snprintf(message, 512, "could not open %s for writing", path);
        lsdj_create_error(error, message);
        return 0;
    }
    
    lsdj_vio_t vio;
    vio.write = lsdj_fwrite;
    vio.tell = lsdj_ftell;
    vio.seek = lsdj_fseek;
    vio.user_data = file;
    
    unsigned int result = lsdj_compress(data, blockSize, startBlock, blockCount, &vio);
    
    fclose(file);
    
    return result;
}
