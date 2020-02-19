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

#include "bytes.h"

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

void set_instrument_bits(lsdj_song_t* song, uint8_t instrument, uint8_t byte, uint8_t position, uint8_t count, uint8_t value)
{
	const size_t index = instrument * LSDJ_INSTRUMENT_BYTE_COUNT + byte;
	assert(index < 1024);

	copy_bits_in_place(
		&song->bytes[INSTRUMENTS_OFFSET + index],
		position, count, value
	);
}

const uint8_t get_instrument_byte(const lsdj_song_t* song, uint8_t instrument, uint8_t byte)
{
	const size_t index = instrument * LSDJ_INSTRUMENT_BYTE_COUNT + byte;
	assert(index < 1024);

	return song->bytes[INSTRUMENTS_OFFSET + index];
}

uint8_t get_instrument_bits(const lsdj_song_t* song, uint8_t instrument, uint8_t byte, uint8_t position, uint8_t count)
{
	const size_t index = instrument * LSDJ_INSTRUMENT_BYTE_COUNT + byte;
	assert(index < 1024);

	return (uint8_t)(get_bits(song->bytes[INSTRUMENTS_OFFSET + index], position, count) >> position);
}

void lsdj_instrument_set_type(lsdj_song_t* song, uint8_t instrument, lsdj_instrument_type type)
{
	set_instrument_bits(song, instrument, 0, 0, 8, (uint8_t)type);
}

lsdj_instrument_type lsdj_instrument_get_type(const lsdj_song_t* song, uint8_t instrument)
{
	return (uint8_t)get_instrument_byte(song, instrument, 0);
}

void lsdj_instrument_set_envelope(lsdj_song_t* song, uint8_t instrument, uint8_t envelope)
{
	set_instrument_bits(song, instrument, 1, 0, 8, envelope);
}

uint8_t lsdj_instrument_get_envelope(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 1, 0, 8);
}

void lsdj_instrument_set_panning(lsdj_song_t* song, uint8_t instrument, lsdj_panning panning)
{
	set_instrument_bits(song, instrument, 7, 0, 2, (uint8_t)panning);
}

lsdj_panning lsdj_instrument_get_panning(const lsdj_song_t* song, uint8_t instrument)
{
	return (lsdj_panning)get_instrument_bits(song, instrument, 7, 0, 2);
}

void lsdj_instrument_set_transpose(lsdj_song_t* song, uint8_t instrument, bool transpose)
{
	set_instrument_bits(song, instrument, 5, 5, 1, transpose ? 0 : 1);
}

bool lsdj_instrument_get_transpose(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 5, 5, 1) == 0;
}

void lsdj_instrument_enable_table(lsdj_song_t* song, uint8_t instrument, bool enabled)
{
	set_instrument_bits(song, instrument, 6, 5, 1, enabled ? 1 : 0);
}

bool lsdj_instrument_is_table_enabled(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 6, 5, 1) == 1;
}

void lsdj_instrument_set_table(lsdj_song_t* song, uint8_t instrument, uint8_t table)
{
	set_instrument_bits(song, instrument, 6, 0, 4, table);
}

uint8_t lsdj_instrument_get_table(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 6, 0, 4);
}

void lsdj_instrument_automate_table(lsdj_song_t* song, uint8_t instrument, bool automate)
{
	set_instrument_bits(song, instrument, 5, 3, 1, automate ? 1 : 0);
}

uint8_t lsdj_instrument_is_table_automated(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 5, 3, 1) == 1;
}

void lsdj_instrument_set_vibrato_direction(lsdj_song_t* song, uint8_t instrument, lsdj_vibrato_direction direction)
{
	set_instrument_bits(song, instrument, 5, 0, 1, (uint8_t)direction);
}

lsdj_vibrato_direction lsdj_instrument_get_vibrato_direction(const lsdj_song_t* song, uint8_t instrument)
{
	return (lsdj_vibrato_direction)get_instrument_bits(song, instrument, 5, 0, 1);
}

bool lsdj_instrument_set_vibrato_shape_and_plv_speed(lsdj_song_t* song, uint8_t instrument, lsdj_vibrato_shape shape, lsdj_plv_speed speed)
{
	if (lsdj_song_get_format_version(song) >= 4)
	{
		set_instrument_bits(song, instrument, 5, 1, 2, (uint8_t)shape);

		set_instrument_bits(song, instrument, 5, 7, 1, speed == LSDJ_INSTRUMENT_PLV_STEP ? 1 : 0);
		set_instrument_bits(song, instrument, 5, 4, 1, speed == LSDJ_INSTRUMENT_PLV_TICK ? 1 : 0);

		return true;
	} else {

		switch (speed)
		{
		case LSDJ_INSTRUMENT_PLV_FAST:
			if (shape == LSDJ_INSTRUMENT_VIBRATO_TRIANGLE)
			{
				set_instrument_bits(song, instrument, 5, 1, 2, 0x0);
				return true;
			} else {
				return false;
			}

		case LSDJ_INSTRUMENT_PLV_TICK:
			switch (shape)
			{
				case LSDJ_INSTRUMENT_VIBRATO_SAWTOOTH: set_instrument_bits(song, instrument, 5, 1, 2, 0x1); return true;
				case LSDJ_INSTRUMENT_VIBRATO_TRIANGLE: set_instrument_bits(song, instrument, 5, 1, 2, 0x2); return true;
				case LSDJ_INSTRUMENT_VIBRATO_SQUARE: set_instrument_bits(song, instrument, 5, 1, 2, 0x3); return true;
				default: return false;
			}

		default:
			return false;
		}

	}
}

lsdj_vibrato_shape lsdj_instrument_get_vibrato_shape(const lsdj_song_t* song, uint8_t instrument)
{
	const uint8_t byte = get_instrument_byte(song, instrument, 5);

	if (lsdj_song_get_format_version(song) >= 4)
	{
		switch (get_instrument_bits(song, instrument, 5, 1, 2))
        {
            case 0: return LSDJ_INSTRUMENT_VIBRATO_TRIANGLE;
            case 1: return LSDJ_INSTRUMENT_VIBRATO_SAWTOOTH;
            case 2: return LSDJ_INSTRUMENT_VIBRATO_SQUARE;
            default: assert(false);
        }
    } else {
		switch (get_instrument_bits(song, instrument, 5, 1, 2))
        {
            case 0: return LSDJ_INSTRUMENT_VIBRATO_TRIANGLE;
            case 1: return LSDJ_INSTRUMENT_VIBRATO_SAWTOOTH;
            case 2: return LSDJ_INSTRUMENT_VIBRATO_TRIANGLE;
            case 3: return LSDJ_INSTRUMENT_VIBRATO_SQUARE;
            default: assert(false);
        }
	}
}

lsdj_plv_speed lsdj_instrument_get_plv_speed(const lsdj_song_t* song, uint8_t instrument)
{
	if (lsdj_song_get_format_version(song) >= 4)
	{
		const uint8_t byte = get_instrument_bits(song, instrument, 5, 0, 8);

		if (byte & 0x80)
			return LSDJ_INSTRUMENT_PLV_STEP;
		else if (byte & 0x10)
			return LSDJ_INSTRUMENT_PLV_TICK;
		else
			return LSDJ_INSTRUMENT_PLV_FAST;

	} else {
		switch (get_instrument_bits(song, instrument, 5, 1, 2))
        {
            case 0: return LSDJ_INSTRUMENT_PLV_FAST;

            case 1:
            case 2:
            case 3: return LSDJ_INSTRUMENT_PLV_TICK;
            default: assert(false);
        }
	}
}


// --- Pulse --- //

void lsdj_instrument_pulse_set_pulse_width(lsdj_song_t* song, uint8_t instrument, lsdj_instrument_pulse_width pulseWidth)
{
	set_instrument_bits(song, instrument, 7, 6, 2, (uint8_t)pulseWidth);
}

lsdj_instrument_pulse_width lsdj_instrument_pulse_get_pulse_width(const lsdj_song_t* song, uint8_t instrument)
{
	return (lsdj_instrument_pulse_width)get_instrument_bits(song, instrument, 7, 6, 2);
}

void lsdj_instrument_pulse_set_length(lsdj_song_t* song, uint8_t instrument, uint8_t length)
{
	const bool unlimited = length == LSDJ_INSTRUMENT_PULSE_LENGTH_INFINITE;
	set_instrument_bits(song, instrument, 3, 6, 1, unlimited ? 0 : 1);

	if (unlimited)
		set_instrument_bits(song, instrument, 3, 0, 5, length);
}

uint8_t lsdj_instrument_pulse_get_length(const lsdj_song_t* song, uint8_t instrument)
{
	if (get_instrument_bits(song, instrument, 3, 6, 1) == 0)
		return LSDJ_INSTRUMENT_PULSE_LENGTH_INFINITE;
	else
		return (~get_instrument_bits(song, instrument, 3, 0, 5)) & 0x3F;
}

void lsdj_instrument_pulse_set_sweep(lsdj_song_t* song, uint8_t instrument, uint8_t sweep)
{
	set_instrument_bits(song, instrument, 4, 0, 8, sweep);
}

uint8_t lsdj_instrument_pulse_get_sweep(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 4, 0, 8);
}

void lsdj_instrument_pulse_set_pulse2_tune(lsdj_song_t* song, uint8_t instrument, uint8_t tune)
{
	set_instrument_bits(song, instrument, 2, 0, 8, tune);
}

uint8_t lsdj_instrument_pulse_get_pulse2_tune(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 2, 0, 8);
}

void lsdj_instrument_pulse_set_finetune(lsdj_song_t* song, uint8_t instrument, uint8_t finetune)
{
	set_instrument_bits(song, instrument, 7, 2, 4, finetune);
}

uint8_t lsdj_instrument_pulse_get_finetune(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 7, 2, 4);
}


// --- Wave --- //

void lsdj_instrument_wave_set_synth(lsdj_song_t* song, uint8_t instrument, uint8_t synth)
{
	set_instrument_bits(song, instrument, 2, 4, 4, synth);
}

uint8_t lsdj_instrument_wave_get_synth(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 2, 4, 4);
}

void lsdj_instrument_wave_set_play_mode(lsdj_song_t* song, uint8_t instrument, lsdj_wave_play_mode mode)
{
	set_instrument_bits(song, instrument, 9, 0, 2, (uint8_t)mode);
}

lsdj_wave_play_mode lsdj_instrument_wave_get_play_mode(const lsdj_song_t* song, uint8_t instrument)
{
	return (lsdj_wave_play_mode)get_instrument_bits(song, instrument, 9, 0, 2);
}

void lsdj_instrument_wave_set_length(lsdj_song_t* song, uint8_t instrument, uint8_t length)
{
	const uint8_t version = lsdj_song_get_format_version(song);
	if (version >= 6)
		set_instrument_bits(song, instrument, 10, 0, 4, 0xF - length);
	else if (version == 6)
		set_instrument_bits(song, instrument, 10, 0, 4, length);
	else
		set_instrument_bits(song, instrument, 14, 4, 4, length);

}

uint8_t lsdj_instrument_wave_get_length(const lsdj_song_t* song, uint8_t instrument)
{
	const uint8_t version = lsdj_song_get_format_version(song);

	if (version >= 6)
		return 0xF - get_instrument_bits(song, instrument, 10, 0, 4);
	else if (version == 6)
		return get_instrument_bits(song, instrument, 10, 0, 4);
	else
		return get_instrument_bits(song, instrument, 14, 4, 4);
}

void lsdj_instrument_wave_set_repeat(lsdj_song_t* song, uint8_t instrument, uint8_t repeat)
{
	set_instrument_bits(song, instrument, 2, 0, 4, repeat);
}

uint8_t lsdj_instrument_wave_get_repeat(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 2, 0, 4);
}

bool lsdj_instrument_wave_set_speed(lsdj_song_t* song, uint8_t instrument, uint8_t speed)
{
    const uint8_t version = lsdj_song_get_format_version(song);
    
    // Speed is stored as starting at 0, but displayed as starting at 1, so subtract 1
    speed -= 1;
    
    if (version >= 6)
        set_instrument_bits(song, instrument, 11, 0, 8, speed - 3);
    else if (version == 6)
        set_instrument_bits(song, instrument, 11, 0, 8, speed);
    else {
    	if (speed > 0x0F)
    		return false;

        set_instrument_bits(song, instrument, 14, 0, 4, speed);
    }

    return true;
}

uint8_t lsdj_instrument_wave_get_speed(const lsdj_song_t* song, uint8_t instrument)
{
	const uint8_t version = lsdj_song_get_format_version(song);
    
    // Read the speed value
    uint8_t speed = 0;
	if (version >= 6)
		speed = get_instrument_bits(song, instrument, 11, 0, 8) + 3;
	else if (version == 6)
		speed = get_instrument_bits(song, instrument, 11, 0, 8);
	else
		speed = get_instrument_bits(song, instrument, 14, 0, 4);
    
    // Speed is stored as starting at 0, but displayed as starting at 1, so add 1
    return speed + 1;
}


// --- Kit --- //

void lsdj_instrument_kit_set_pitch(lsdj_song_t* song, uint8_t instrument, uint8_t pitch)
{
	set_instrument_bits(song, instrument, 8, 0, 8, pitch);
}

uint8_t lsdj_instrument_kit_get_pitch(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 8, 0, 8);
}

void lsdj_instrument_kit_set_half_speed(lsdj_song_t* song, uint8_t instrument, bool halfSpeed)
{
	set_instrument_bits(song, instrument, 2, 6, 1, halfSpeed ? 1 : 0);
}

bool lsdj_instrument_kit_get_half_speed(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 2, 6, 1) == 1;
}

void lsdj_instrument_kit_set_distortion_mode(lsdj_song_t* song, uint8_t instrument, lsdj_kit_distortion_mode distortion)
{
	set_instrument_bits(song, instrument, 10, 0, 2, (uint8_t)distortion);
}

lsdj_kit_distortion_mode lsdj_instrument_kit_get_distortion_mode(const lsdj_song_t* song, uint8_t instrument)
{
	return (lsdj_kit_distortion_mode)get_instrument_bits(song, instrument, 10, 0, 2);
}

void lsdj_instrument_kit_set_kit1(lsdj_song_t* song, uint8_t instrument, uint8_t kit)
{
	set_instrument_bits(song, instrument, 2, 0, 5, kit);
}

uint8_t lsdj_instrument_kit_get_kit1(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 2, 0, 5);
}

void lsdj_instrument_kit_set_kit2(lsdj_song_t* song, uint8_t instrument, uint8_t kit)
{
	set_instrument_bits(song, instrument, 9, 0, 5, kit);
}

uint8_t lsdj_instrument_kit_get_kit2(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 9, 0, 5);
}

void lsdj_instrument_kit_set_offset1(lsdj_song_t* song, uint8_t instrument, uint8_t offset)
{
	set_instrument_bits(song, instrument, 12, 0, 8, offset);
}

uint8_t lsdj_instrument_kit_get_offset1(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 12, 0, 8);
}

void lsdj_instrument_kit_set_offset2(lsdj_song_t* song, uint8_t instrument, uint8_t offset)
{
	set_instrument_bits(song, instrument, 13, 0, 8, offset);
}

uint8_t lsdj_instrument_kit_get_offset2(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 13, 0, 8);
}

void lsdj_instrument_kit_set_length1(lsdj_song_t* song, uint8_t instrument, uint8_t length)
{
	set_instrument_bits(song, instrument, 3, 0, 8, length);
}

uint8_t lsdj_instrument_kit_get_length1(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 3, 0, 8);
}

void lsdj_instrument_kit_set_length2(lsdj_song_t* song, uint8_t instrument, uint8_t length)
{
	set_instrument_bits(song, instrument, 13, 0, 8, length);
}

uint8_t lsdj_instrument_kit_get_length2(const lsdj_song_t* song, uint8_t instrument)
{
	return get_instrument_bits(song, instrument, 13, 0, 8);
}

void lsdj_instrument_kit_set_loop1(lsdj_song_t* song, uint8_t instrument, lsdj_kit_loop_mode loop)
{
	set_instrument_bits(song, instrument, 2, 7, 1, loop == LSDJ_INSTRUMENT_KIT_LOOP_ATTACK ? 1 : 0);
	set_instrument_bits(song, instrument, 5, 6, 1, loop == LSDJ_INSTRUMENT_KIT_LOOP_ON ? 1 : 0);
}

lsdj_kit_loop_mode lsdj_instrument_kit_get_loop1(const lsdj_song_t* song, uint8_t instrument)
{
	if (get_instrument_bits(song, instrument, 2, 7, 1) == 1)
		return LSDJ_INSTRUMENT_KIT_LOOP_ATTACK;
	else
		return get_instrument_bits(song, instrument, 5, 6, 1);
}

void lsdj_instrument_kit_set_loop2(lsdj_song_t* song, uint8_t instrument, lsdj_kit_loop_mode loop)
{
	set_instrument_bits(song, instrument, 9, 7, 1, loop == LSDJ_INSTRUMENT_KIT_LOOP_ATTACK ? 1 : 0);
	set_instrument_bits(song, instrument, 5, 5, 1, loop == LSDJ_INSTRUMENT_KIT_LOOP_ON ? 1 : 0);
}

lsdj_kit_loop_mode lsdj_instrument_kit_get_loop2(const lsdj_song_t* song, uint8_t instrument)
{
	if (get_instrument_bits(song, instrument, 9, 7, 1) == 1)
		return LSDJ_INSTRUMENT_KIT_LOOP_ATTACK;
	else
		return get_instrument_bits(song, instrument, 5, 5, 1);
}


// --- Noise --- //

void lsdj_instrument_noise_set_stability(lsdj_song_t* song, uint8_t instrument, lsdj_noise_stability stability)
{
	set_instrument_bits(song, instrument, 2, 0, 1, (uint8_t)stability);
}

lsdj_noise_stability lsdj_instrument_noise_get_stability(const lsdj_song_t* song, uint8_t instrument)
{
	return (lsdj_noise_stability)get_instrument_bits(song, instrument, 2, 0, 1);
}