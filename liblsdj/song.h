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

#ifndef LSDJ_SONG_H
#define LSDJ_SONG_H

/* Song buffers are the representation of an LSDJ song uncompressed in memory.
   It hasn't been parsed yet into meaningful data. This is raw song data, and
   for example how the song in working memory within your LSDJ sav is repre-
   sented.

   From here, you can either compress song buffers into memory blocks, which is
   how all other projects in LSDJ are represented. You can also go the other
   way around, and parse them into an lsdj_song_t structure, providing you with
   detailed information about elements within your song. */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "channel.h"
#include "error.h"

//! The size of a decompressed song in memory
#define LSDJ_SONG_BYTE_COUNT (0x8000)

//! The value representing an empty row + channel slot
#define LSDJ_NO_CHAIN (0xFF)

//! A structure that can hold one entire decompressed song in memory
typedef struct
{
	uint8_t bytes[LSDJ_SONG_BYTE_COUNT];
} lsdj_song_t;

typedef enum
{
   LSDJ_CLONE_DEEP = 0,
   LSDJ_CLONE_SLIM = 1
} lsdj_clone_mode;

typedef enum
{
   LSDJ_SYNC_NONE = 0,
   LSDJ_SYNC_LSDJ = 1,
   LSDJ_SYNC_MIDI = 2,
   LSDJ_SYNC_KEYBOARD = 3,
   LSDJ_SYNC_ANALOG_IN = 4,
   LSDJ_SYNC_ANALOG_OUT = 5,
} lsdj_sync_mode;


// --- Song Settings --- //

//! Retrieve the format version of the song
/*! @note This is not the same as the project version in the song save/load screen,
    nor the version of LSDj itself. This relates to the internal format version
    that is increased everytime the song format changes. */
uint8_t lsdj_song_get_format_version(const lsdj_song_t* song);

//! Has the song been changed since the last save?
bool lsdj_song_has_changed(const lsdj_song_t* song);

void lsdj_song_set_tempo(lsdj_song_t* song, uint8_t bpm);
uint8_t lsdj_song_get_tempo(const lsdj_song_t* song);

void lsdj_song_set_transposition(lsdj_song_t* song, uint8_t semitones);
uint8_t lsdj_song_get_transposition(const lsdj_song_t* song);

void lsdj_song_set_sync_mode(lsdj_song_t* song, lsdj_sync_mode mode);
lsdj_sync_mode lsdj_song_get_sync_mode(const lsdj_song_t* song);


// --- Editor Settings --- //

void lsdj_song_set_clone_mode(lsdj_song_t* song, lsdj_clone_mode clone);
lsdj_clone_mode lsdj_song_get_clone_mode(const lsdj_song_t* song);

void lsdj_song_set_font(lsdj_song_t* song, uint8_t font);
uint8_t lsdj_song_get_font(const lsdj_song_t* song);

void lsdj_song_set_color_set(lsdj_song_t* song, uint8_t colorSet);
uint8_t lsdj_song_get_color_set(const lsdj_song_t* song);

void lsdj_song_set_key_delay(lsdj_song_t* song, uint8_t delay);
uint8_t lsdj_song_get_key_delay(const lsdj_song_t* song);

void lsdj_song_set_key_repeat(lsdj_song_t* song, uint8_t repeat);
uint8_t lsdj_song_get_key_repeat(const lsdj_song_t* song);

void lsdj_song_set_prelisten(lsdj_song_t* song, bool prelisten);
bool lsdj_song_get_prelisten(const lsdj_song_t* song);


// --- Clocks --- //

void lsdj_song_set_total_days(lsdj_song_t* song, uint8_t days);
uint8_t lsdj_song_get_total_days(const lsdj_song_t* song);

void lsdj_song_set_total_hours(lsdj_song_t* song, uint8_t hours);
uint8_t lsdj_song_get_total_hours(const lsdj_song_t* song);

void lsdj_song_set_total_minutes(lsdj_song_t* song, uint8_t minutes);
uint8_t lsdj_song_get_total_minutes(const lsdj_song_t* song);

void lsdj_song_set_work_hours(lsdj_song_t* song, uint8_t hours);
uint8_t lsdj_song_get_work_hours(const lsdj_song_t* song);

void lsdj_song_set_work_minutes(lsdj_song_t* song, uint8_t minutes);
uint8_t lsdj_song_get_work_minutes(const lsdj_song_t* song);


// --- Musical data --- //

//! Change the chain assigned to a row + channel slot
/*! @param chain The chain number to assign, or LSDJ_NO_CHAIN to set it to empty */
void lsdj_song_set_chain_assignment(lsdj_song_t* song, uint8_t row, lsdj_channel channel, uint8_t chain);

//! Retrieve the chain assigned to a row + channel slot
/*! @return The chain number of LSDJ_NO_CHAIN if empty */
uint8_t lsdj_song_get_chain_assignment(const lsdj_song_t* song, uint8_t row, lsdj_channel channel);
    
#ifdef __cplusplus
}
#endif

#endif
