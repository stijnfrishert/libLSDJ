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


// --- SONG --- //


//! Retrieve the format version of the song
/*! @note This is not the same as the project version in the song save/load screen,
    nor the version of LSDj itself. This relates to the internal format version
    that is increased everytime the song format changes. */
uint8_t lsdj_song_get_format_version(const lsdj_song_t* song);

//! Change the tempo (beats-per-minute) at which this song runs
void lsdj_song_set_tempo(lsdj_song_t* song, uint8_t bpm);

//! Retrieve the tempo (beats-per-minute) at which this song runs
uint8_t lsdj_song_get_tempo(const lsdj_song_t* song);

//! Change the global transposition (in semitones) at which this song runs
void lsdj_song_set_transposition(lsdj_song_t* song, int8_t semitones);

//! Retrieve the tempo (beats-per-minute) at which this song runs
int8_t lsdj_song_get_transposition(const lsdj_song_t* song);

//! Retrieve the total work time days stored in the song
void lsdj_song_set_total_days(lsdj_song_t* song, uint8_t days);

//! Retrieve the total work time days stored in the song
uint8_t lsdj_song_get_total_days(const lsdj_song_t* song);

//! Retrieve the total work time hours stored in the song
void lsdj_song_set_total_hours(lsdj_song_t* song, uint8_t hours);

//! Retrieve the total work time hours stored in the song
uint8_t lsdj_song_get_total_hours(const lsdj_song_t* song);

//! Retrieve the total work time minutes stored in the song
void lsdj_song_set_total_minutes(lsdj_song_t* song, uint8_t minutes);

//! Retrieve the total work time minutes stored in the song
uint8_t lsdj_song_get_total_minutes(const lsdj_song_t* song);

//! Retrieve the work time hours stored in the song
void lsdj_song_set_work_hours(lsdj_song_t* song, uint8_t hours);

//! Retrieve the work time hours stored in the song
uint8_t lsdj_song_get_work_hours(const lsdj_song_t* song);

//! Retrieve the work time minutes stored in the song
void lsdj_song_set_work_minutes(lsdj_song_t* song, uint8_t minutes);

//! Retrieve the work time minutes stored in the song
uint8_t lsdj_song_get_work_minutes(const lsdj_song_t* song);

//! Change the key repeat delay
void lsdj_song_set_key_delay(lsdj_song_t* song, uint8_t delay);

//! Retrieve the key repeat delay
uint8_t lsdj_song_get_key_delay(const lsdj_song_t* song);

//! Change the key repeat rate
void lsdj_song_set_key_repeat(lsdj_song_t* song, uint8_t repeat);

//! Retrieve the key repeat rate
uint8_t lsdj_song_get_key_repeat(const lsdj_song_t* song);

//! Change the font used for this song
void lsdj_song_set_font(lsdj_song_t* song, uint8_t repeat);

//! Retrieve the font used for this song
uint8_t lsdj_song_get_font(const lsdj_song_t* song);

//! Change the sync setting used for this song
void lsdj_song_set_sync_setting(lsdj_song_t* song, uint8_t repeat);

//! Retrieve the sync setting used for this song
uint8_t lsdj_song_get_sync_setting(const lsdj_song_t* song);

//! Change the color set used for this song
void lsdj_song_set_color_set(lsdj_song_t* song, uint8_t repeat);

//! Retrieve the color set used for this song
uint8_t lsdj_song_get_color_set(const lsdj_song_t* song);

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
