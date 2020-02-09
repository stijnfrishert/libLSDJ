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

#include "song.h"

#include <assert.h>

#define CHAIN_ASSIGNMENTS_OFFSET (0x1290)
#define CHAIN_ASSIGNMENTS_LENGTH (1024)

#define WORK_HOURS_OFFSET (0x3FB2)
#define WORK_MINUTES_OFFSET (0x3FB3)

#define TOTAL_DAYS_OFFSET (0x3FB6)
#define TOTAL_HOURS_OFFSET (0x3FB7)
#define TOTAL_MINUTES_OFFSET (0x3FB8)
#define TOTAL_TIME_CHECKSUM_OFFSET (0x3FB9)

#define FORMAT_VERSION_OFFSET (0x7FFF)

void lsdj_song_set_format_version(lsdj_song_t* song, uint8_t version)
{
    song->bytes[FORMAT_VERSION_OFFSET] = version;
}

uint8_t lsdj_song_get_format_version(const lsdj_song_t* song)
{
    return song->bytes[FORMAT_VERSION_OFFSET];
}

void lsdj_song_set_total_days(lsdj_song_t* song, uint8_t days)
{
	song->bytes[TOTAL_DAYS_OFFSET] = days;
}

uint8_t lsdj_song_get_total_days(const lsdj_song_t* song)
{
	return song->bytes[TOTAL_DAYS_OFFSET];
}

void lsdj_song_set_total_hours(lsdj_song_t* song, uint8_t hours)
{
	song->bytes[TOTAL_HOURS_OFFSET] = hours;
}

uint8_t lsdj_song_get_total_hours(const lsdj_song_t* song)
{
	return song->bytes[TOTAL_HOURS_OFFSET];
}

void lsdj_song_set_total_minutes(lsdj_song_t* song, uint8_t minutes)
{
	song->bytes[TOTAL_MINUTES_OFFSET] = minutes;
}

uint8_t lsdj_song_get_total_minutes(const lsdj_song_t* song)
{
	return song->bytes[TOTAL_MINUTES_OFFSET];
}

void lsdj_song_set_work_hours(lsdj_song_t* song, uint8_t hours)
{
	song->bytes[WORK_HOURS_OFFSET] = hours;
}

uint8_t lsdj_song_get_work_hours(const lsdj_song_t* song)
{
	return song->bytes[WORK_HOURS_OFFSET];
}

void lsdj_song_set_work_minutes(lsdj_song_t* song, uint8_t minutes)
{
	song->bytes[WORK_MINUTES_OFFSET] = minutes;
}

uint8_t lsdj_song_get_work_minutes(const lsdj_song_t* song)
{
	return song->bytes[WORK_MINUTES_OFFSET];
}

void lsdj_song_set_chain_assignment(lsdj_song_t* song, uint8_t row, lsdj_channel channel, uint8_t chain)
{
	const size_t index = CHAIN_ASSIGNMENTS_OFFSET + row * LSDJ_CHANNEL_COUNT + channel;
	assert(index < CHAIN_ASSIGNMENTS_OFFSET + CHAIN_ASSIGNMENTS_LENGTH);

    song->bytes[index] = chain;   
}

uint8_t lsdj_song_get_chain_assignment(const lsdj_song_t* song, uint8_t row, lsdj_channel channel)
{
	const size_t index = CHAIN_ASSIGNMENTS_OFFSET + row * LSDJ_CHANNEL_COUNT + channel;
	assert(index < CHAIN_ASSIGNMENTS_OFFSET + CHAIN_ASSIGNMENTS_LENGTH);

    return song->bytes[CHAIN_ASSIGNMENTS_OFFSET + row * LSDJ_CHANNEL_COUNT + channel];
}
