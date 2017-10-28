//
//  instrument.h
//  lsdj
//
//  Created by Stijn Frishert on 9/11/17.
//
//

#ifndef LSDJ_INSTRUMENT_H
#define LSDJ_INSTRUMENT_H

#include "error.h"
#include "instrument_kit.h"
#include "instrument_noise.h"
#include "instrument_pulse.h"
#include "instrument_wave.h"
#include "vio.h"

// The default constant length of an instrument name
#define INSTRUMENT_NAME_LENGTH 5

#define DEFAULT_INSTRUMENT_LENGTH 16
static const unsigned char DEFAULT_INSTRUMENT[DEFAULT_INSTRUMENT_LENGTH] = { 0, 0xA8, 0, 0, 0xFF, 0, 0, 3, 0, 0, 0xD0, 0, 0, 0, 0xF3, 0 };

typedef enum
{
    INSTR_PULSE,
    INSTR_WAVE,
    INSTR_KIT,
    INSTR_NOISE
} instrument_type;

typedef unsigned char lsdj_panning;
static const lsdj_panning LSDJ_PAN_NONE = 0;
static const lsdj_panning LSDJ_PAN_RIGHT = 1;
static const lsdj_panning LSDJ_PAN_LEFT = 2;
static const lsdj_panning LSDJ_PAN_LEFT_RIGHT = 3;

static const unsigned char NO_TABLE = 0x20;
static const unsigned char UNLIMITED_LENGTH = 0x40;
static const unsigned char KIT_LENGTH_AUTO = 0x0;
    
// Structure representing one instrument
typedef struct
{
    char name[INSTRUMENT_NAME_LENGTH];

    instrument_type type;
    
    union { unsigned char envelope; unsigned char volume; };
    lsdj_panning panning;
    unsigned char table; // 0x20 or higher = NO_TABLE
    unsigned char automate;
    
    union
    {
        lsdj_instrument_pulse_t pulse;
        lsdj_instrument_wave_t wave;
        lsdj_instrument_kit_t kit;
        lsdj_instrument_noise_t noise;
    };
} lsdj_instrument_t;
    
// Clear all instrument data to factory settings
void lsdj_clear_instrument(lsdj_instrument_t* instrument);
void lsdj_clear_instrument_as_pulse(lsdj_instrument_t* instrument);
void lsdj_clear_instrument_as_wave(lsdj_instrument_t* instrument);
void lsdj_clear_instrument_as_kit(lsdj_instrument_t* instrument);
void lsdj_clear_instrument_as_noise(lsdj_instrument_t* instrument);

// Instrument I/O
void lsdj_read_instrument(lsdj_vio_t* vio, unsigned char version, lsdj_instrument_t* instrument, lsdj_error_t** error);
void lsdj_write_instrument(const lsdj_instrument_t* instrument, unsigned char version, lsdj_vio_t* vio, lsdj_error_t** error);

#endif
