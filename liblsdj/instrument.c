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
#define ALLOCATION_TABLE_OFFSET (0x2040)
#define INSTRUMENTS_OFFSET (0x3080)

bool lsdj_instrument_is_allocated(const lsdj_song_t* song, uint8_t instrument)
{
    const size_t index = ALLOCATION_TABLE_OFFSET + instrument;
    assert(index <= ALLOCATION_TABLE_OFFSET + 64);

    return song->bytes[index];
}

void lsdj_instrument_set_name(lsdj_song_t* song, uint8_t instrument, const char* name)
{
    const size_t index = NAMES_OFFSET + instrument * LSDJ_INSTRUMENT_NAME_LENGTH;
    assert(index < NAMES_OFFSET + 320);

    strncpy((char*)(&song->bytes[index]), name, LSDJ_INSTRUMENT_NAME_LENGTH);
}

const char* lsdj_instrument_get_name(const lsdj_song_t* song, uint8_t instrument)
{
    const size_t index = NAMES_OFFSET + instrument * LSDJ_INSTRUMENT_NAME_LENGTH;
    assert(index < NAMES_OFFSET + 320);

    return (const char*)(&song->bytes[index]);
}

void set_instrument_byte(lsdj_song_t* song, uint8_t instrument, uint8_t byte, uint8_t value)
{
	const size_t index = instrument * LSDJ_INSTRUMENT_BYTE_COUNT + byte;
	assert(index < 1024);

	song->bytes[INSTRUMENTS_OFFSET + index] = value;
}

const uint8_t get_instrument_byte(const lsdj_song_t* song, uint8_t instrument, uint8_t byte)
{
	const size_t index = instrument * LSDJ_INSTRUMENT_BYTE_COUNT + byte;
	assert(index < 1024);

	return song->bytes[INSTRUMENTS_OFFSET + index];
}

void lsdj_instrument_set_type(lsdj_song_t* song, uint8_t instrument, lsdj_instrument_type type)
{
	set_instrument_byte(song, instrument, 0, (uint8_t)type);
}

lsdj_instrument_type lsdj_instrument_get_type(const lsdj_song_t* song, uint8_t instrument)
{
	return (uint8_t)get_instrument_byte(song, instrument, 0);
}

void lsdj_instrument_set_envelope(lsdj_song_t* song, uint8_t instrument, uint8_t envelope)
{
	set_instrument_byte(song, instrument, 1, envelope);
}

uint8_t lsdj_instrument_get_envelope(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_byte(song, instrument, 1);
}

void lsdj_instrument_set_panning(lsdj_song_t* song, uint8_t instrument, lsdj_panning panning)
{
	const unsigned int byte = (get_instrument_byte(song, instrument, 7) & 0xFC) | (panning & 0x3);
	set_instrument_byte(song, instrument, 7, (uint8_t)byte);
}

lsdj_panning lsdj_instrument_get_panning(const lsdj_song_t* song, uint8_t instrument)
{
	return (lsdj_panning)get_instrument_byte(song, instrument, 7) & 0x3;
}

void lsdj_instrument_enable_table(lsdj_song_t* song, uint8_t instrument, bool enabled)
{
	uint8_t byte = get_instrument_byte(song, instrument, 6) & 0xDF;
	if (enabled)
		byte |= 0x20;

	set_instrument_byte(song, instrument, 6, byte);	
}

bool lsdj_instrument_is_table_enabled(const lsdj_song_t* song, uint8_t instrument)
{
	return (get_instrument_byte(song, instrument, 6) & 0x20) != 0;
}

void lsdj_instrument_set_table(lsdj_song_t* song, uint8_t instrument, uint8_t table)
{
	const unsigned int byte = (get_instrument_byte(song, instrument, 6) & 0xE0) | (table & 0x1F);
}

uint8_t lsdj_instrument_get_table(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_byte(song, instrument, 6) & 0x1F;
}

void lsdj_instrument_automate_table(lsdj_song_t* song, uint8_t instrument, bool automate)
{
	uint8_t byte = get_instrument_byte(song, instrument, 5) & 0xF7;
	if (automate)
		byte |= 0x8;

	set_instrument_byte(song, instrument, 5, byte);	
}

uint8_t lsdj_instrument_is_table_automated(const lsdj_song_t* song, uint8_t instrument)
{
	return (get_instrument_byte(song, instrument, 6) & 0x8) != 0;
}

void lsdj_instrument_set_pulse_width(lsdj_song_t* song, uint8_t instrument, lsdj_instrument_pulse_width pulseWidth)
{
	const unsigned int byte = (get_instrument_byte(song, instrument, 7) & 0x3F) | ((pulseWidth & 0x3) << 6);
	set_instrument_byte(song, instrument, 7, (uint8_t)byte);
}

lsdj_instrument_pulse_width lsdj_instrument_get_pulse_width(const lsdj_song_t* song, uint8_t instrument)
{
	return (lsdj_instrument_pulse_width)get_instrument_byte(song, instrument, 7) >> 6 & 0x3;
}

void lsdj_instrument_set_pulse_length(lsdj_song_t* song, uint8_t instrument, uint8_t length)
{
	uint8_t byte = get_instrument_byte(song, instrument, 3);
	if (length == LSDJ_INSTRUMENT_PULSE_LENGTH_INFINITE)
		byte = (byte & 0xC0) | LSDJ_INSTRUMENT_PULSE_LENGTH_INFINITE;
	else
		byte = (byte & 0xC0) | ~(length & 0x3F);
	set_instrument_byte(song, instrument, 3, byte);
}

uint8_t lsdj_instrument_get_pulse_length(const lsdj_song_t* song, uint8_t instrument)
{
	const uint8_t byte = get_instrument_byte(song, instrument, 3);
	if (byte & LSDJ_INSTRUMENT_PULSE_LENGTH_INFINITE)
		return ~(byte & 0x3F);
	else
		return LSDJ_INSTRUMENT_PULSE_LENGTH_INFINITE;
}

void lsdj_instrument_set_pulse_sweep(lsdj_song_t* song, uint8_t instrument, uint8_t sweep)
{
	set_instrument_byte(song, instrument, 4, sweep);
}

uint8_t lsdj_instrument_get_pulse_sweep(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_byte(song, instrument, 4);
}

void lsdj_instrument_set_pulse_transpose(lsdj_song_t* song, uint8_t instrument, bool transpose)
{
	uint8_t byte = get_instrument_byte(song, instrument, 5) & 0xDF;
	if (transpose)
		byte |= 0x20;

	set_instrument_byte(song, instrument, 5, byte);
}

bool lsdj_instrument_get_pulse_transpose(const lsdj_song_t* song, uint8_t instrument)
{
	return (get_instrument_byte(song, instrument, 5) & 0x20) != 0;
}

void lsdj_instrument_set_pulse2_tune(lsdj_song_t* song, uint8_t instrument, uint8_t tune)
{
	set_instrument_byte(song, instrument, 2, tune);
}

uint8_t lsdj_instrument_get_pulse2_tune(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_byte(song, instrument, 2);
}

void lsdj_instrument_set_pulse_finetune(lsdj_song_t* song, uint8_t instrument, bool finetune)
{
	const int byte = (get_instrument_byte(song, instrument, 7) & 0xC3) | ((finetune & 0xF) << 2);
	set_instrument_byte(song, instrument, 7, (uint8_t)byte);
}

bool lsdj_instrument_get_pulse_finetune(const lsdj_song_t* song, uint8_t instrument)
{
	return (get_instrument_byte(song, instrument, 7) >> 2) & 0xF;
}