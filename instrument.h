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
#include "vio.h"

// The default constant length of an instrument name
static const unsigned int INSTRUMENT_NAME_LENGTH = 5;
    
static const unsigned char DEFAULT_INSTRUMENT[16] = { 0, 0xA8, 0, 0, 0xFF, 0, 0, 3, 0, 0, 0xD0, 0, 0, 0, 0xF3, 0 };

typedef enum
{
    INSTR_PULSE,
    INSTR_WAVE,
    INSTR_KIT,
    INSTR_NOISE
} instrument_type;

typedef enum
{
    PAN_LEFT_RIGHT,
    PAN_LEFT,
    PAN_RIGHT,
    PAN_NONE
} panning;

typedef enum
{
    PULSE_WIDTH_125,
    PULSE_WIDTH_25,
    PULSE_WIDTH_50,
    PULSE_WIDTH_75
} pulse_width;

typedef enum
{
    PLVIB_HIGH_FREQUENCY,
    PLVIB_SAWTOOTH,
    PLVIB_TRIANGLE,
    PLVIB_SQUARE
} plvib_type;

typedef enum
{
    VIB_UP,
    VIB_DOWN
} vibrato_direction;

typedef enum
{
    TUNE_12_TONE,
    TUNE_FIXED,
    TUNE_DRUM
} tuning_mode;

typedef enum
{
    PLAY_ONCE,
    PLAY_LOOP,
    PLAY_PING_PONG,
    PLAY_MANUAL
} playback_mode;

typedef enum
{
    KIT_LOOP_OFF,
    KIT_LOOP_ON,
    KIT_LOOP_ATTACK
} kit_loop_mode;

typedef enum
{
    KIT_DIST_CLIP,
    KIT_DIST_SHAPE,
    KIT_DIST_SHAPE2,
    KIT_DIST_WRAP,
} kit_distortion;

typedef enum
{
    KIT_PSPEED_FAST,
    KIT_PSPEED_SLOW,
    KIT_PSPEED_STEP
} kit_pspeed;

typedef enum
{
    SCOMMAND_FREE,
    SCOMMAND_STABLE
} scommand_type;

typedef struct
{
    pulse_width pulseWidth;
    unsigned char length; // 0x40 and above = unlimited
    unsigned char sweep;
    plvib_type plvib;
    vibrato_direction vibratoDirection;
    tuning_mode tuning;
    unsigned char pulse2tune;
    unsigned char fineTune;
} lsdj_instrument_pulse_t;

typedef struct
{
    plvib_type plvib;
    vibrato_direction vibratoDirection;
    tuning_mode tuning;
    unsigned char synth;
    playback_mode playback;
    unsigned char length;
    unsigned char repeat;
    unsigned char speed;
} lsdj_instrument_wave_t;

typedef struct
{
    unsigned char kit1;
    unsigned char offset1;
    unsigned char length1;
    kit_loop_mode loop1;
    
    unsigned char kit2;
    unsigned char offset2;
    unsigned char length2;
    kit_loop_mode loop2;
    
    unsigned char pitch;
    unsigned char halfSpeed;
    kit_distortion distortion;
    kit_pspeed pSpeed;
} lsdj_instrument_kit_t;

typedef struct
{
    unsigned char length; // 0x40 and above = unlimited
    unsigned char shape;
    scommand_type sCommand;
} lsdj_instrument_noise_t;

static const unsigned char NO_TABLE = 0x20;
static const unsigned char UNLIMITED_LENGTH = 0x40;
static const unsigned char KIT_LENGTH_AUTO = 0x0;
    
// Structure representing one instrument
typedef struct
{
    char name[INSTRUMENT_NAME_LENGTH];

    instrument_type type;
    
    union { unsigned char envelope; unsigned char volume; };
    panning panning;
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
void lsdj_read_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_instrument_t* instrument, lsdj_error_t** error);
void lsdj_write_instrument(lsdj_vio_write_t write, void* user_data, lsdj_instrument_t* instrument, lsdj_error_t** error);

#endif
