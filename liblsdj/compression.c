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

bool decompress_rle_byte(lsdj_vio_t* rvio, lsdj_vio_t* wvio, size_t* writeCount, lsdj_error_t** error)
{
    unsigned char byte;
    if (rvio->read(&byte, 1, rvio->userData) != 1)
    {
        lsdj_error_optional_new(error, "could not read RLE byte");
        return false;
    }
    
    if (byte == RUN_LENGTH_ENCODING_BYTE)
    {
        if (wvio->write(&byte, 1, wvio->userData) != 1)
        {
            lsdj_error_optional_new(error, "could not write RLE byte");
            return false;
        }
        if (writeCount) *writeCount += 1;
    }
    else
    {
        unsigned char count = 0;
        if (rvio->read(&count, 1, rvio->userData) != 1)
        {
            lsdj_error_optional_new(error, "could not read RLE count byte");
            return false;
        }
        
        for (int i = 0; i < count; ++i)
        {
            if (wvio->write(&byte, 1, wvio->userData) != 1)
            {
                lsdj_error_optional_new(error, "could not write byte for RLE expansion");
                return false;
            }
            if (writeCount) *writeCount += 1;
        }
    }

    return true;
}

bool decompress_default_wave_byte(lsdj_vio_t* rvio, lsdj_vio_t* wvio, size_t* writeCount, lsdj_error_t** error)
{
    unsigned char count = 0;
    if (rvio->read(&count, 1, rvio->userData) != 1)
    {
        lsdj_error_optional_new(error, "could not read default wave count byte");
        return false;
    }
    
    for (int i = 0; i < count; ++i)
    {
        size_t written_bytes_count = wvio->write(LSDJ_DEFAULT_WAVE, sizeof(LSDJ_DEFAULT_WAVE), wvio->userData);
        if (writeCount) *writeCount += written_bytes_count;
        if (written_bytes_count != sizeof(LSDJ_DEFAULT_WAVE))
        {
            lsdj_error_optional_new(error, "could not write default wave byte");
            return false;
        }
    }

    return true;
}

bool decompress_default_instrument_byte(lsdj_vio_t* rvio, lsdj_vio_t* wvio, size_t* writeCount, lsdj_error_t** error)
{
    unsigned char count = 0;
    if (rvio->read(&count, 1, rvio->userData) != 1)
    {
        lsdj_error_optional_new(error, "could not read default instrument count byte");
        return false;
    }
    
    for (int i = 0; i < count; ++i)
    {
        size_t written_bytes_count = wvio->write(LSDJ_DEFAULT_INSTRUMENT_COMPRESSION, sizeof(LSDJ_DEFAULT_INSTRUMENT_COMPRESSION), wvio->userData);
        if (writeCount) *writeCount += written_bytes_count;
        if (written_bytes_count != sizeof(LSDJ_DEFAULT_INSTRUMENT_COMPRESSION))
        {
            lsdj_error_optional_new(error, "could not write default instrument byte");
            return false;
        }
    }

    return true;
}

bool decompress_sa_byte(lsdj_vio_t* rvio, long* currentBlockPosition, bool followBlockSwitches, lsdj_vio_t* wvio, bool* reading, size_t* writeCount, lsdj_error_t** error)
{
    unsigned char byte = 0;
    if (rvio->read(&byte, 1, rvio->userData) != 1)
    {
        lsdj_error_optional_new(error, "could not read SA byte");
        return false;
    }
    
    switch (byte)
    {
        case SPECIAL_ACTION_BYTE:
            if (wvio->write(&byte, 1, wvio->userData) != 1)
            {
                lsdj_error_optional_new(error, "could not write SA byte");
                return false;
            }
            if (writeCount) *writeCount += 1;
            break;
        case LSDJ_DEFAULT_WAVE_BYTE:
            if (!decompress_default_wave_byte(rvio, wvio, writeCount, error))
                return false;
            break;
        case LSDJ_DEFAULT_INSTRUMENT_BYTE:
            if (!decompress_default_instrument_byte(rvio, wvio, writeCount, error))
                return false;
            break;
        case END_OF_FILE_BYTE:
            *reading = false;
            break;
        default:
            if (followBlockSwitches)
                *currentBlockPosition = LSDJ_SAV_HEADER_POSITION + LSDJ_BLOCK_SIZE + (long)((byte - 1) * LSDJ_BLOCK_SIZE);
            else
                *currentBlockPosition += LSDJ_BLOCK_SIZE;
            
            if (rvio->seek(*currentBlockPosition, SEEK_SET, rvio->userData) != 0)
            {
                lsdj_error_optional_new(error, "could not seek to new block position");
                return false;
            }
            break;
    }

    return true;
}

bool lsdj_decompress(lsdj_vio_t* rvio, lsdj_vio_t* wvio, bool followBlockSwitches, size_t* writeCount, lsdj_error_t** error)
{
    if (writeCount)
        *writeCount = 0;

    // Store the position of the current block
    // (This will be updated through the decompression algorithm)
    long currentBlockPosition = rvio->tell(rvio->userData);
    if (currentBlockPosition == -1L)
    {
        lsdj_error_optional_new(error, "could not tell current read block position");
        return false;
    }

    // Store our starting write position
    const long writeStart = wvio->tell(wvio->userData);
    
    unsigned char byte = 0;
    
    bool reading = true;
    while (reading == true)
    {
        // Uncomment this to see every read and corresponding write position
//        const long rcur = rvio->tell(rvio->userData) - 9;
//        const long wcur = wvio->tell(wvio->userData) - writeStart;
//        printf("read: 0x%lx\twrite: 0x%lx\n", rcur, wcur);
        
        if (rvio->read(&byte, 1, rvio->userData) != 1)
        {
            lsdj_error_optional_new(error, "could not read byte for decompression");
            return false;
        }
        
        switch (byte)
        {
            case RUN_LENGTH_ENCODING_BYTE:
                if (!decompress_rle_byte(rvio, wvio, writeCount, error))
                    return false;
                break;
            case SPECIAL_ACTION_BYTE:
                if (!decompress_sa_byte(rvio, &currentBlockPosition, followBlockSwitches, wvio, &reading, writeCount, error))
                    return false;
                break;
            default:
                if (wvio->write(&byte, 1, wvio->userData) != 1)
                {
                    lsdj_error_optional_new(error, "could not write decompression byte");
                    return false;
                }
                if (writeCount) *writeCount += 1;
                break;
        }
    }

    const long writeEnd = wvio->tell(wvio->userData);
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

bool lsdj_compress(const unsigned char* data, lsdj_vio_t* wvio, unsigned int blockOffset, size_t* writeCount, lsdj_error_t** error)
{
    //! @todo This function needs to be refactored because it is w-a-y too huge

    if (writeCount)
        *writeCount = 0;
     
    if (blockOffset == LSDJ_BLOCK_COUNT + 1)
        return false;
    
    unsigned char nextEvent[3] = { 0, 0, 0 };
    unsigned short eventSize = 0;
    
    unsigned int currentBlock = blockOffset;
    unsigned int currentBlockSize = 0;
    
    unsigned char byte = 0;
    
    long writeStart = wvio->tell(wvio->userData);
    if (writeStart == -1L)
    {
        lsdj_error_optional_new(error, "could not tell write position on compression");
        return false;
    }
    
    const unsigned char* end = data + LSDJ_SONG_BUFFER_BYTE_COUNT;
    for (const unsigned char* read = data; read < end; )
    {
        // Uncomment this to print the current read and write positions
        // long wcur = wvio->tell(wvio->userData) - writeStart;
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
            if (wvio->write(&byte, 1, wvio->userData) != 1)
            {
                lsdj_error_optional_new(error, "could not write SA byte for next block command");
                return false;
            }
            if (writeCount) *writeCount += 1;
            
            byte = (unsigned char)(currentBlock + 1);
            if (wvio->write(&byte, 1, wvio->userData) != 1)
            {
                lsdj_error_optional_new(error, "could not write next block byte for compression");
                return false;
            }
            if (writeCount) *writeCount += 1;
            
            currentBlockSize += 2;
            assert(currentBlockSize <= LSDJ_BLOCK_SIZE);
            
            // Fill the rest of the block with 0's
            byte = 0;
            for (; currentBlockSize < LSDJ_BLOCK_SIZE; currentBlockSize++)
            {
                if (wvio->write(&byte, 1, wvio->userData) != 1)
                {
                    lsdj_error_optional_new(error, "could not write 0 for block padding");
                    return false;
                }
                if (writeCount) *writeCount += 1;
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
                long pos = wvio->tell(wvio->userData);
                if (wvio->seek(writeStart, SEEK_SET, wvio->userData) != 0)
                {
                    lsdj_error_optional_new(error, "could not roll back after reaching max block count for compression");
                    return 0;
                }
                
                byte = 0;
                for (long i = 0; i < pos - writeStart; ++i)
                {
                    if (wvio->write(&byte, 1, wvio->userData) != 1)
                    {
                        lsdj_error_optional_new(error, "could not fill rolled back data with 0 for compression");
                        return false;
                    }
                    if (writeCount) *writeCount += 1;
                }
                
                if (wvio->seek(writeStart, SEEK_SET, wvio->userData) != 0)
                {
                    lsdj_error_optional_new(error, "could not fill roll back to start for compression roll back");
                    return false;
                }
                
                return false;
            }
            
            // Don't "continue;" but fall through. We still need to write the event *in the next block*
        }
        
        size_t count = wvio->write(nextEvent, eventSize, wvio->userData);
        if (writeCount) *writeCount += count;
        if (count != eventSize)
        {
            lsdj_error_optional_new(error, "could not write event for compression");
            return false;
        }
        
        currentBlockSize += eventSize;
        nextEvent[0] = nextEvent[1] = nextEvent[2] = 0;
        eventSize = 0;
    }
    
    byte = SPECIAL_ACTION_BYTE;
    if (wvio->write(&byte, 1, wvio->userData) != 1)
    {
        lsdj_error_optional_new(error, "could not write SA for EOF for compression");
        return 0;
    }
    if (writeCount) *writeCount += 1;
    
    byte = END_OF_FILE_BYTE;
    if (wvio->write(&byte, 1, wvio->userData) != 1)
    {
        lsdj_error_optional_new(error, "could not write EOF for compression");
        return false;
    }
    if (writeCount) *writeCount += 1;
    
    if (currentBlockSize > 0)
    {
        byte = 0;
        for (currentBlockSize += 2; currentBlockSize < LSDJ_BLOCK_SIZE; currentBlockSize++)
        {
            if (wvio->write(&byte, 1, wvio->userData) != 1)
            {
                lsdj_error_optional_new(error, "could not write 0 for block padding");
                return false;
            }
            if (writeCount) *writeCount += 1;
        }
    }
    
    return true;
}
