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

#include "instrument.h"

#include <assert.h>
#include <string.h>

#define NAMES_OFFSET (0x1E7A)
#define NAMES_LENGTH (320)

#define ALLOCATION_TABLE_OFFSET (0x2040)
#define ALLOCATION_TABLE_LENGTH (64)

bool lsdj_instrument_is_allocated(const lsdj_song_t* song, uint8_t instrument)
{
    const size_t index = ALLOCATION_TABLE_OFFSET + instrument;
    assert(index <= ALLOCATION_TABLE_OFFSET + ALLOCATION_TABLE_LENGTH);

    return song->bytes[index];
}

void lsdj_instrument_set_name(lsdj_song_t* song, uint8_t instrument, const char* name)
{
    const size_t index = NAMES_OFFSET + instrument * LSDJ_INSTRUMENT_NAME_LENGTH;
    assert(index < NAMES_OFFSET + NAMES_LENGTH);

    strncpy((char*)(&song->bytes[index]), name, LSDJ_INSTRUMENT_NAME_LENGTH);
}

const char* lsdj_instrument_get_name(const lsdj_song_t* song, uint8_t instrument)
{
    const size_t index = NAMES_OFFSET + instrument * LSDJ_INSTRUMENT_NAME_LENGTH;
    assert(index < NAMES_OFFSET + NAMES_LENGTH);

    return (const char*)(&song->bytes[index]);
}