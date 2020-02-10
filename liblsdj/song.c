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

#define TEMPO_OFFSET (0x3FB4)
#define TRANSPOSITION_OFFSET (0x3FB5)
#define TOTAL_DAYS_OFFSET (0x3FB6)
#define TOTAL_HOURS_OFFSET (0x3FB7)
#define TOTAL_MINUTES_OFFSET (0x3FB8)
#define TOTAL_TIME_CHECKSUM_OFFSET (0x3FB9)
#define KEY_DELAY_OFFSET (0x3FBA)
#define KEY_REPEAT_OFFSET (0x3FBB)
#define FONT_OFFSET (0x3FBC)
#define SYNC_SETTING_OFFSET (0x3FBD)
#define COLOR_SET_OFFSET (0x3FBE)

#define FORMAT_VERSION_OFFSET (0x7FFF)

uint8_t lsdj_song_get_format_version(const lsdj_song_t* song)
{
    return song->bytes[FORMAT_VERSION_OFFSET];
}

void lsdj_song_set_tempo(lsdj_song_t* song, uint8_t bpm)
{
	song->bytes[TEMPO_OFFSET] = bpm;
}

uint8_t lsdj_song_get_tempo(const lsdj_song_t* song)
{
	return song->bytes[TEMPO_OFFSET];
}

void lsdj_song_set_transposition(lsdj_song_t* song, int8_t semitones)
{
	song->bytes[TRANSPOSITION_OFFSET] = (uint8_t)(semitones);
}

int8_t lsdj_song_get_transposition(const lsdj_song_t* song)
{
	return (int8_t)(song->bytes[TRANSPOSITION_OFFSET]);
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

void lsdj_song_set_key_delay(lsdj_song_t* song, uint8_t delay)
{
	song->bytes[KEY_DELAY_OFFSET] = delay;
}

uint8_t lsdj_song_get_key_delay(const lsdj_song_t* song)
{
	return song->bytes[KEY_DELAY_OFFSET];
}

void lsdj_song_set_key_repeat(lsdj_song_t* song, uint8_t repeat)
{
	song->bytes[KEY_REPEAT_OFFSET] = repeat;
}

uint8_t lsdj_song_get_key_repeat(const lsdj_song_t* song)
{
	return song->bytes[KEY_REPEAT_OFFSET];
}

void lsdj_song_set_font(lsdj_song_t* song, uint8_t repeat)
{
	song->bytes[FONT_OFFSET] = repeat;
}

uint8_t lsdj_song_get_font(const lsdj_song_t* song)
{
	return song->bytes[FONT_OFFSET];
}

void lsdj_song_set_sync_setting(lsdj_song_t* song, uint8_t repeat)
{
	song->bytes[SYNC_SETTING_OFFSET] = repeat;
}

uint8_t lsdj_song_get_sync_setting(const lsdj_song_t* song)
{
	return song->bytes[SYNC_SETTING_OFFSET];
}

void lsdj_song_set_color_set(lsdj_song_t* song, uint8_t repeat)
{
	song->bytes[COLOR_SET_OFFSET] = repeat;
}

uint8_t lsdj_song_get_color_set(const lsdj_song_t* song)
{
	return song->bytes[COLOR_SET_OFFSET];
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
