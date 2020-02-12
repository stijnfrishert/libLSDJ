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

// --- Offsets --- //

#define PHRASE_NOTES_OFFSET (0x0000)
#define BOOKMARKS_OFFSET (0x0FF0)
#define CHAIN_ASSIGNMENTS_OFFSET (0x1290)

#define CHAIN_PHRASES_OFFSET (0x2080)
#define CHAIN_TRANSPOSITIONS_OFFSET (0x2880)
#define PHRASE_ALLOCATIONS_OFFSET (0x3E82)
#define CHAIN_ALLOCATIONS_OFFSET (0x3EA2)

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
#define SYNC_MODE_OFFSET (0x3FBD)
#define COLOR_PALETTE_OFFSET (0x3FBE)
#define CLONE_MODE_OFFSET (0x3FC0)
#define FILE_CHANGED_OFFSET (0x3FC1)
#define PRELISTEN_OFFSET (0x3FC3)
#define PHRASE_COMMANDS_OFFSET (0x4000)
#define PHRASE_COMMAND_VALUES_OFFSET (0x4FF0)

#define PHRASE_INSTRUMENTS_OFFSET (0x7000)
#define FORMAT_VERSION_OFFSET (0x7FFF)

// --- Other macros --- //

#define BOOKMARK_PER_CHANNEL_COUNT (16)
#define NO_BOOKMARK_VALUE (0xFF)

// --- Convenience Macros --- //

#define PHRASE_SETTER(OFFSET, LENGTH, VALUE) \
const size_t index = phrase * LSDJ_PHRASE_LENGTH + row; \
assert(index <= LENGTH); \
song->bytes[OFFSET + index] = VALUE;

#define PHRASE_GETTER(OFFSET, LENGTH) \
const size_t index = phrase * LSDJ_PHRASE_LENGTH + row; \
assert(index <= LENGTH); \
return song->bytes[OFFSET + index];

// --- Song Settings --- //

uint8_t lsdj_song_get_format_version(const lsdj_song_t* song)
{
    return song->bytes[FORMAT_VERSION_OFFSET];
}

bool lsdj_song_has_changed(const lsdj_song_t* song)
{
	return song->bytes[FILE_CHANGED_OFFSET] == 1;
}

void lsdj_song_set_tempo(lsdj_song_t* song, uint8_t bpm)
{
	song->bytes[TEMPO_OFFSET] = bpm;
}

uint8_t lsdj_song_get_tempo(const lsdj_song_t* song)
{
	return song->bytes[TEMPO_OFFSET];
}

void lsdj_song_set_transposition(lsdj_song_t* song, uint8_t semitones)
{
	song->bytes[TRANSPOSITION_OFFSET] = semitones;
}

uint8_t lsdj_song_get_transposition(const lsdj_song_t* song)
{
	return song->bytes[TRANSPOSITION_OFFSET];
}

void lsdj_song_set_sync_mode(lsdj_song_t* song, lsdj_sync_mode mode)
{
	song->bytes[SYNC_MODE_OFFSET] = (uint8_t)mode;
}

lsdj_sync_mode lsdj_song_get_sync_mode(const lsdj_song_t* song)
{
	return (lsdj_sync_mode)song->bytes[SYNC_MODE_OFFSET];
}


// --- Editor Settings --- //

void lsdj_song_set_clone_mode(lsdj_song_t* song, lsdj_clone_mode clone)
{
	song->bytes[CLONE_MODE_OFFSET] = (uint8_t)clone;
}

lsdj_clone_mode lsdj_song_get_clone_mode(const lsdj_song_t* song)
{
	return (lsdj_clone_mode)song->bytes[CLONE_MODE_OFFSET];
}

void lsdj_song_set_font(lsdj_song_t* song, uint8_t font)
{
	song->bytes[FONT_OFFSET] = font;
}

uint8_t lsdj_song_get_font(const lsdj_song_t* song)
{
	return song->bytes[FONT_OFFSET];
}

void lsdj_song_set_color_palette(lsdj_song_t* song, uint8_t palette)
{
	song->bytes[COLOR_PALETTE_OFFSET] = palette;
}

uint8_t lsdj_song_get_color_palette(const lsdj_song_t* song)
{
	return song->bytes[COLOR_PALETTE_OFFSET];
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

void lsdj_song_set_prelisten(lsdj_song_t* song, bool prelisten)
{
	song->bytes[PRELISTEN_OFFSET] = prelisten ? 1 : 0;
}

bool lsdj_song_get_prelisten(const lsdj_song_t* song)
{
	return song->bytes[PRELISTEN_OFFSET] == 1;
}


// --- Clocks --- //

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


// --- Chains, Phrases //

void lsdj_row_set_chain(lsdj_song_t* song, uint8_t row, lsdj_channel channel, uint8_t chain)
{
	const size_t index = CHAIN_ASSIGNMENTS_OFFSET + row * LSDJ_CHANNEL_COUNT + channel;
	assert(index < CHAIN_ASSIGNMENTS_OFFSET + 1024);

    song->bytes[index] = chain;   
}

uint8_t lsdj_row_get_chain(const lsdj_song_t* song, uint8_t row, lsdj_channel channel)
{
	const size_t index = CHAIN_ASSIGNMENTS_OFFSET + row * LSDJ_CHANNEL_COUNT + channel;
	assert(index < CHAIN_ASSIGNMENTS_OFFSET + 1024);

    return song->bytes[CHAIN_ASSIGNMENTS_OFFSET + row * LSDJ_CHANNEL_COUNT + channel];
}

bool lsdj_song_set_row_bookmarked(lsdj_song_t* song, uint8_t row, lsdj_channel channel, bool bookmarked)
{
	if (lsdj_song_is_row_bookmarked(song, row, channel) == bookmarked)
		return true;

	for (size_t i = 0; i < BOOKMARK_PER_CHANNEL_COUNT; i++)
	{
		const size_t index = channel * BOOKMARK_PER_CHANNEL_COUNT + i;
		assert(index < 64);

		uint8_t* slot = &song->bytes[BOOKMARKS_OFFSET + index];

		if (bookmarked && *slot == NO_BOOKMARK_VALUE)
		{
			*slot = row;
			return true;
		} else if (!bookmarked && *slot == row) {
			*slot = NO_BOOKMARK_VALUE;
			return true;
		}
	}

	return false;
}

bool lsdj_song_is_row_bookmarked(const lsdj_song_t* song, uint8_t row, lsdj_channel channel)
{
	for (size_t i = 0; i < BOOKMARK_PER_CHANNEL_COUNT; i++)
	{
		const size_t index = channel * BOOKMARK_PER_CHANNEL_COUNT + i;
		assert(index < 64);

		if (song->bytes[BOOKMARKS_OFFSET + index] == row)
			return true;
	}

	return false;
}


bool lsdj_chain_is_allocated(const lsdj_song_t* song, uint8_t chain)
{
	const size_t index = chain / 8;
	assert(index < 16);

	const size_t mask = 1 << (chain - (index * 8));
    
	return (song->bytes[CHAIN_ALLOCATIONS_OFFSET + index] & mask) != 0;
}

void lsdj_chain_set_phrase(lsdj_song_t* song, uint8_t chain, uint8_t row, uint8_t phrase)
{
	const size_t index = chain * LSDJ_CHAIN_LENGTH + row;
	assert(index < 2048);

	song->bytes[CHAIN_PHRASES_OFFSET + index] = phrase;
}

uint8_t lsdj_chain_get_phrase(const lsdj_song_t* song, uint8_t chain, uint8_t row)
{
	const size_t index = chain * LSDJ_CHAIN_LENGTH + row;
	assert(index < 2048);

	return song->bytes[CHAIN_PHRASES_OFFSET + index];
}

void lsdj_chain_set_transposition(lsdj_song_t* song, uint8_t chain, uint8_t row, uint8_t transposition)
{
	const size_t index = chain * LSDJ_CHAIN_LENGTH + row;
	assert(index < 2048);

	song->bytes[CHAIN_TRANSPOSITIONS_OFFSET + index] = transposition;
}

uint8_t lsdj_chain_get_transposition(const lsdj_song_t* song, uint8_t chain, uint8_t row)
{
	const size_t index = chain * LSDJ_CHAIN_LENGTH + row;
	assert(index < 2048);

	return song->bytes[CHAIN_TRANSPOSITIONS_OFFSET + index];
}

bool lsdj_phrase_is_allocated(const lsdj_song_t* song, uint8_t phrase)
{
	const size_t index = phrase / 8;
	assert(index < 32);

	const size_t mask = 1 << (phrase - (index * 8));
    
	return (song->bytes[PHRASE_ALLOCATIONS_OFFSET + index] & mask) != 0;
}

void lsdj_phrase_set_note(lsdj_song_t* song, uint8_t phrase, uint8_t row, uint8_t note)
{
	PHRASE_SETTER(PHRASE_NOTES_OFFSET, 4080, note)
}

uint8_t lsdj_phrase_get_note(const lsdj_song_t* song, uint8_t phrase, uint8_t row)
{
	PHRASE_GETTER(PHRASE_NOTES_OFFSET, 4080)
}

void lsdj_phrase_set_instrument(lsdj_song_t* song, uint8_t phrase, uint8_t row, uint8_t instrument)
{
	PHRASE_SETTER(PHRASE_INSTRUMENTS_OFFSET, 4080, instrument)
}

uint8_t lsdj_phrase_get_instrument(const lsdj_song_t* song, uint8_t phrase, uint8_t row)
{
	PHRASE_GETTER(PHRASE_INSTRUMENTS_OFFSET, 4080)
}

void lsdj_phrase_set_command(lsdj_song_t* song, uint8_t phrase, uint8_t row, lsdj_command command)
{
	PHRASE_SETTER(PHRASE_COMMANDS_OFFSET, 4080, (uint8_t)command)
}

lsdj_command lsdj_phrase_get_command(const lsdj_song_t* song, uint8_t phrase, uint8_t row)
{
	PHRASE_GETTER(PHRASE_COMMANDS_OFFSET, 4080)
}

void lsdj_phrase_set_command_value(lsdj_song_t* song, uint8_t phrase, uint8_t row, uint8_t value)
{
	PHRASE_SETTER(PHRASE_COMMAND_VALUES_OFFSET, 4080, value)
}

uint8_t lsdj_phrase_get_command_value(const lsdj_song_t* song, uint8_t phrase, uint8_t row)
{
	PHRASE_GETTER(PHRASE_COMMAND_VALUES_OFFSET, 4080)
}
