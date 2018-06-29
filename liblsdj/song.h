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

#ifndef LSDJ_SONG_H
#define LSDJ_SONG_H

#include "instrument.h"
#include "phrase.h"
#include "table.h"
#include "vio.h"

#define LSDJ_SONG_DECOMPRESSED_SIZE (0x8000)
#define LSDJ_ROW_COUNT (256)
#define LSDJ_CHAIN_COUNT (128)
#define LSDJ_PHRASE_COUNT (0xFF)
#define LSDJ_BOOKMARK_COUNT (64)
#define LSDJ_INSTRUMENT_COUNT (64)
#define LSDJ_SYNTH_COUNT (16)
#define LSDJ_TABLE_COUNT (32)
#define LSDJ_WAVE_COUNT (256)
#define LSDJ_GROOVE_COUNT (32)
#define LSDJ_WORD_COUNT (42)
    
static const unsigned char LSDJ_CLONE_DEEP = 0;
static const unsigned char LSDJ_CLONE_SLIM = 1;

// An LSDJ song
typedef struct lsdj_song_t lsdj_song_t;

// Create/free projects
lsdj_song_t* lsdj_song_new(lsdj_error_t** error);
lsdj_song_t* lsdj_song_copy(const lsdj_song_t* song, lsdj_error_t** error);
void lsdj_song_free(lsdj_song_t* song);

// Deserialize a song
lsdj_song_t* lsdj_song_read(lsdj_vio_t* vio, lsdj_error_t** error);
lsdj_song_t* lsdj_song_read_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error);
    
// Serialize a song
void lsdj_song_write(const lsdj_song_t* song, lsdj_vio_t* vio, lsdj_error_t** error);
void lsdj_song_write_to_memory(const lsdj_song_t* song, unsigned char* data, size_t size, lsdj_error_t** error);

// Change data in a song
void lsdj_song_set_format_version(lsdj_song_t* song, unsigned char version);
unsigned char lsdj_song_get_format_version(const lsdj_song_t* song);
void lsdj_song_set_tempo(lsdj_song_t* song, unsigned char tempo);
unsigned char lsdj_song_get_tempo(const lsdj_song_t* song);
void lsdj_song_set_transposition(lsdj_song_t* song, unsigned char transposition);
unsigned char lsdj_song_get_transposition(const lsdj_song_t* song);
unsigned char lsdj_song_get_file_changed_flag(const lsdj_song_t* song);
void lsdj_song_set_drum_max(lsdj_song_t* song, unsigned char drumMax);
unsigned char lsdj_song_get_drum_max(const lsdj_song_t* song);

lsdj_instrument_t* lsdj_song_get_instrument(lsdj_song_t* song, size_t index);
lsdj_table_t* lsdj_song_get_table(lsdj_song_t* song, size_t index);
lsdj_phrase_t* lsdj_song_get_phrase(lsdj_song_t* song, size_t index);

#endif
