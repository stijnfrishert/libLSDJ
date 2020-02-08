/*
 
 This file is a part of liblsdj, a C library for managing everything
 that has to do with LSDJ, software for writing music (chiptune) with
 your gameboy. For more information, see:
 
 * https://github.com/stijnfrishert/liblsdj
 * http://www.littlesounddj.com
 
 --------------------------------------------------------------------------------
 
 MIT License
 
 Copyright (c) 2018 - 2020 Stijn Frishert
 
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

#include "compression.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "instrument.h"
#include "sav.h"
#include "song_buffer.h"
#include "wave.h"

#define RUN_LENGTH_ENCODING_BYTE 0xC0
#define SPECIAL_ACTION_BYTE 0xE0
#define END_OF_FILE_BYTE 0xFF
#define LSDJ_DEFAULT_WAVE_BYTE 0xF0
#define LSDJ_DEFAULT_INSTRUMENT_BYTE 0xF1

static const unsigned char LSDJ_DEFAULT_INSTRUMENT_COMPRESSION[LSDJ_LSDJ_DEFAULT_INSTRUMENT_LENGTH] = { 0xA8, 0, 0, 0xFF, 0, 0, 3, 0, 0, 0xD0, 0, 0, 0, 0xF3, 0, 0 };

bool decompress_rle_byte(lsdj_vio_t* rvio, size_t* readCounter, lsdj_vio_t* wvio, size_t* writeCounter, lsdj_error_t** error)
{
    unsigned char byte = 0;
    if (!lsdj_vio_read_byte(rvio, &byte, readCounter))
    {
        lsdj_error_optional_new(error, "could not read RLE byte");
        return false;
    }
    
    if (byte == RUN_LENGTH_ENCODING_BYTE)
    {
        if (!lsdj_vio_write_byte(wvio, byte, writeCounter))
        {
            lsdj_error_optional_new(error, "could not write RLE byte");
            return false;
        }
    }
    else
    {
        unsigned char count = 0;
        if (!lsdj_vio_read_byte(rvio, &count, readCounter))
        {
            lsdj_error_optional_new(error, "could not read RLE count byte");
            return false;
        }
        
        if (!lsdj_vio_write_repeat(wvio, &byte, 1, count, writeCounter))
        {
            lsdj_error_optional_new(error, "could not write byte for RLE expansion");
            return false;
        }
    }

    return true;
}

bool decompress_default_wave_byte(lsdj_vio_t* rvio, lsdj_vio_t* wvio, size_t* writeCounter, lsdj_error_t** error)
{
    unsigned char count = 0;
    if (!lsdj_vio_read_byte(rvio, &count, NULL))
    {
        lsdj_error_optional_new(error, "could not read default wave count byte");
        return false;
    }
    
    if (!lsdj_vio_write_repeat(wvio, LSDJ_DEFAULT_WAVE, sizeof(LSDJ_DEFAULT_WAVE), count, writeCounter))
    {
        lsdj_error_optional_new(error, "could not write default wave byte");
        return false;
    }

    return true;
}

bool decompress_default_instrument_byte(lsdj_vio_t* rvio, lsdj_vio_t* wvio, size_t* writeCounter, lsdj_error_t** error)
{
    unsigned char count = 0;
    if (!lsdj_vio_read_byte(rvio, &count, NULL))
    {
        lsdj_error_optional_new(error, "could not read default instrument count byte");
        return false;
    }
    
    if (!lsdj_vio_write_repeat(wvio, LSDJ_DEFAULT_INSTRUMENT_COMPRESSION, sizeof(LSDJ_DEFAULT_INSTRUMENT_COMPRESSION), count, writeCounter))
    {
        lsdj_error_optional_new(error, "could not write default instrument byte");
        return false;
    }

    return true;
}

bool decompress_sa_byte(lsdj_vio_t* rvio, size_t* readCounter, long firstBlockPosition, bool followBlockJumps, lsdj_vio_t* wvio, size_t* writeCounter, bool* reading, lsdj_error_t** error)
{
    unsigned char byte = 0;
    if (!lsdj_vio_read_byte(rvio, &byte, readCounter))
    {
        lsdj_error_optional_new(error, "could not read SA byte");
        return false;
    }
    
    switch (byte)
    {
        case SPECIAL_ACTION_BYTE:
            if (!lsdj_vio_write_byte(wvio, byte, writeCounter))
            {
                lsdj_error_optional_new(error, "could not write SA byte");
                return false;
            }
            return true;
        case LSDJ_DEFAULT_WAVE_BYTE:
            return decompress_default_wave_byte(rvio, wvio, writeCounter, error);
        case LSDJ_DEFAULT_INSTRUMENT_BYTE:
            return decompress_default_instrument_byte(rvio, wvio, writeCounter, error);
        case END_OF_FILE_BYTE:
            *reading = false;
            return true;
        default:
        {
            if (followBlockJumps)
            {
                const long newPosition = firstBlockPosition + (long)((byte - 1) * LSDJ_BLOCK_SIZE);
                if (!lsdj_vio_seek(rvio, newPosition, SEEK_SET))
                {
                    lsdj_error_optional_new(error, "could not jump to new block position");
                    return false;
                }
            } else {
                const long position = lsdj_vio_tell(rvio) - firstBlockPosition;
                const long offset = LSDJ_BLOCK_SIZE - (position % LSDJ_BLOCK_SIZE);
                if (!lsdj_vio_seek(rvio, offset, SEEK_CUR))
                {
                    lsdj_error_optional_new(error, "could not jump to new block position");
                    return false;
                }
            }
            
            assert((lsdj_vio_tell(rvio) - firstBlockPosition) % LSDJ_BLOCK_SIZE == 0);
            
            return true;
        }
    }
}

bool lsdj_decompress(lsdj_vio_t* rvio, size_t* readCounter,
                     lsdj_vio_t* wvio, size_t* writeCounter,
                     long firstBlockPosition,
                     bool followBlockJumps,
                     lsdj_error_t** error)
{
    // Store our starting write position
    const long readStart = lsdj_vio_tell(rvio);
    const long writeStart = lsdj_vio_tell(wvio);
    
    unsigned char byte = 0;
    
    bool reading = true;
    while (reading == true)
    {
        // Uncomment this to see every read and corresponding write position
//        const long rcur = lsdj_vio_tell(rvio)/* - readStart*/;
//        const long wcur = lsdj_vio_tell(wvio)/* - writeStart*/;
//        printf("read: 0x%lx ->\twrite: 0x%lx\n", rcur, wcur);
        
        if (!lsdj_decompress_step(rvio, readCounter,
                                  wvio, writeCounter,
                                  firstBlockPosition,
                                  followBlockJumps,
                                  &reading,
                                  error)) {
            return false;
        }
    }

    const long writeEnd = lsdj_vio_tell(wvio);
    if (writeEnd == -1L)
    {
        lsdj_error_optional_new(error, "could not tell compression end");
        return false;
    }
    
    const long readSize = writeEnd - writeStart;
    if (readSize != LSDJ_SONG_BUFFER_BYTE_COUNT)
    {
        char buffer[100];
        memset(buffer, '\0', sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "decompressed size does not line up with 0x8000 bytes (but 0x%lx)", writeEnd - writeStart);
        lsdj_error_optional_new(error, buffer);
        return false;
    }

    return true;
}

bool lsdj_decompress_step(lsdj_vio_t* rvio, size_t* readCounter,
                          lsdj_vio_t* wvio, size_t* writeCounter,
                          long firstBlockPosition,
                          bool followBlockJumps,
                          bool* reading,
                          lsdj_error_t** error)
{
    unsigned char byte = 0;
    
    if (!lsdj_vio_read_byte(rvio, &byte, readCounter))
    {
        lsdj_error_optional_new(error, "could not read byte for decompression step");
        return false;
    }
    
    switch (byte)
    {
        case RUN_LENGTH_ENCODING_BYTE:
            return decompress_rle_byte(rvio, readCounter, wvio, writeCounter, error);
            
        case SPECIAL_ACTION_BYTE:
            return decompress_sa_byte(rvio, readCounter, firstBlockPosition, followBlockJumps, wvio, writeCounter, reading, error);
            
        default:
            if (!lsdj_vio_write_byte(wvio, byte, writeCounter))
            {
                lsdj_error_optional_new(error, "could not write decompression byte");
                return false;
            } else {
                return true;
            }
    }
}

bool lsdj_compress(const unsigned char* data, lsdj_vio_t* wvio, unsigned int blockOffset, size_t* writeCounter, lsdj_error_t** error)
{
    //! @todo This function needs to be refactored because it is w-a-y too huge

    if (blockOffset == LSDJ_BLOCK_COUNT + 1)
        return false;
    
    unsigned char nextEvent[3] = { 0, 0, 0 };
    unsigned short eventSize = 0;
    
    unsigned int currentBlock = blockOffset;
    unsigned int currentBlockSize = 0;
    
    unsigned char byte = 0;
    
    long writeStart = lsdj_vio_tell(wvio);
    if (writeStart == -1L)
    {
        lsdj_error_optional_new(error, "could not tell write position on compression");
        return false;
    }
    
    const unsigned char* end = data + LSDJ_SONG_BUFFER_BYTE_COUNT;
    for (const unsigned char* read = data; read < end; )
    {
        // Uncomment this to print the current read and write positions
        // long wcur = lsdj_vio_tell(wvio) - writeStart;
        // printf("read: 0x%lx\twrite: 0x%lx\n", read - data, wcur);
        
        // Are we reading a default wave? If so, we can compress these!
        unsigned char defaultWaveLengthCount = 0;
        while (read + LSDJ_WAVE_LENGTH < end && memcmp(read, LSDJ_DEFAULT_WAVE, LSDJ_WAVE_LENGTH) == 0 && defaultWaveLengthCount != 0xFF)
        {
            read += LSDJ_WAVE_LENGTH;
            ++defaultWaveLengthCount;
        }
        
        if (defaultWaveLengthCount > 0)
        {
            nextEvent[0] = SPECIAL_ACTION_BYTE;
            nextEvent[1] = LSDJ_DEFAULT_WAVE_BYTE;
            nextEvent[2] = defaultWaveLengthCount;
            eventSize = 3;
        } else {
            // Are we reading a default instrument? If so, we can compress these!
            unsigned char defaultInstrumentLengthCount = 0;
            while (read + LSDJ_LSDJ_DEFAULT_INSTRUMENT_LENGTH < end && memcmp(read, LSDJ_DEFAULT_INSTRUMENT_COMPRESSION, LSDJ_LSDJ_DEFAULT_INSTRUMENT_LENGTH) == 0 && defaultInstrumentLengthCount != 0xFF)
            {
                read += LSDJ_LSDJ_DEFAULT_INSTRUMENT_LENGTH;
                ++defaultInstrumentLengthCount;
            }
            
            if (defaultInstrumentLengthCount > 0)
            {
                nextEvent[0] = SPECIAL_ACTION_BYTE;
                nextEvent[1] = LSDJ_DEFAULT_INSTRUMENT_BYTE;
                nextEvent[2] = defaultInstrumentLengthCount;
                eventSize = 3;
            } else {
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
            }
        }
         
        
        // See if the event would still fit in this block
        // If not, move to a new block
        if (currentBlockSize + eventSize + 2 >= LSDJ_BLOCK_SIZE)
        {
            // Write the "next block" command
            byte = SPECIAL_ACTION_BYTE;
            if (!lsdj_vio_write_byte(wvio, byte, writeCounter))
            {
                lsdj_error_optional_new(error, "could not write SA byte for next block command");
                return false;
            }
            
            byte = (unsigned char)(currentBlock + 1);
            if (!lsdj_vio_write_byte(wvio, byte, writeCounter))
            {
                lsdj_error_optional_new(error, "could not write next block byte for compression");
                return false;
            }
            
            currentBlockSize += 2;
            assert(currentBlockSize <= LSDJ_BLOCK_SIZE);
            
            // Fill the rest of the block with 0's
            byte = 0;
            for (; currentBlockSize < LSDJ_BLOCK_SIZE; currentBlockSize++)
            {
                if (!lsdj_vio_write_byte(wvio, byte, writeCounter))
                {
                    lsdj_error_optional_new(error, "could not write 0 for block padding");
                    return false;
                }
            }
            
            // Make sure we filled up the block entirely
            if (currentBlockSize != LSDJ_BLOCK_SIZE)
            {
                lsdj_error_optional_new(error, "block wasn't completely filled upon compression");
                return false;
            }
            
            // Move to the next block
            currentBlock += 1;
            currentBlockSize = 0;
            
            // Have we reached the maximum block count?
            // If so, roll back
            if (currentBlock == LSDJ_BLOCK_COUNT + 1)
            {
                long pos = lsdj_vio_tell(wvio);
                if (!lsdj_vio_seek(wvio, writeStart, SEEK_SET))
                {
                    lsdj_error_optional_new(error, "could not roll back after reaching max block count for compression");
                    return 0;
                }
                
                byte = 0;
                if (!lsdj_vio_write_repeat(wvio, &byte, 1, (size_t)(pos - writeStart), writeCounter))
                {
                    lsdj_error_optional_new(error, "could not fill rolled back data with 0 for compression");
                    return false;
                }
                
                if (!lsdj_vio_seek(wvio, writeStart, SEEK_SET))
                {
                    lsdj_error_optional_new(error, "could not fill roll back to start for compression roll back");
                    return false;
                }
                
                return false;
            }
            
            // Don't "continue;" but fall through. We still need to write the event *in the next block*
        }
        
        if (!lsdj_vio_write(wvio, nextEvent, eventSize, writeCounter))
        {
            lsdj_error_optional_new(error, "could not write event for compression");
            return false;
        }
        
        currentBlockSize += eventSize;
        nextEvent[0] = nextEvent[1] = nextEvent[2] = 0;
        eventSize = 0;
    }
    
    byte = SPECIAL_ACTION_BYTE;
    if (!lsdj_vio_write_byte(wvio, byte, writeCounter))
    {
        lsdj_error_optional_new(error, "could not write SA for EOF for compression");
        return 0;
    }
    
    byte = END_OF_FILE_BYTE;
    if (!lsdj_vio_write_byte(wvio, byte, writeCounter))
    {
        lsdj_error_optional_new(error, "could not write EOF for compression");
        return false;
    }
    
    // Pad 0's to the end of the block
    if (currentBlockSize > 0)
    {
        byte = 0;
        for (currentBlockSize += 2; currentBlockSize < LSDJ_BLOCK_SIZE; currentBlockSize++)
        {
            if (!lsdj_vio_write_byte(wvio, byte, writeCounter))
            {
                lsdj_error_optional_new(error, "could not write 0 for block padding");
                return false;
            }
        }
    }
    
    return true;
}
