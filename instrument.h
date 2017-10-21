//
//  instrument.h
//  lsdj
//
//  Created by Stijn Frishert on 9/11/17.
//
//

#ifndef LSDJ_INSTRUMENT_H
#define LSDJ_INSTRUMENT_H

// The default constant length of an instrument name
static const unsigned int INSTRUMENT_NAME_LENGTH = 5;
    
static const unsigned char DEFAULT_INSTRUMENT[16] = { 0, 0xA8, 0, 0, 0xFF, 0, 0, 3, 0, 0, 0xD0, 0, 0, 0, 0xF3, 0 };
    
static const unsigned char INSTR_TYPE_PULSE = 0;
static const unsigned char INSTR_TYPE_WAVE = 1;
static const unsigned char INSTR_TYPE_KIT = 2;
static const unsigned char INSTR_TYPE_NOISE = 3;

typedef struct
{
    unsigned char pulseWidth;
    unsigned char length;
    unsigned char sweep;
    unsigned char plvib;
    unsigned char vibratoDirection;
    unsigned char transpose;
    unsigned char pulse2tune;
    unsigned char fineTune;
} lsdj_instrument_pulse_t;

typedef struct
{
    unsigned char plvib;
    unsigned char vibratoDirection;
    unsigned char transpose;
    unsigned char synth;
    unsigned char playback;
    unsigned char length;
    unsigned char repeat;
    unsigned char speed;
} lsdj_instrument_wave_t;

typedef struct
{
    unsigned char sample1;
    unsigned char sample2;
    unsigned char pitch;
    unsigned char offset1;
    unsigned char length1;
    unsigned char loop1;
    unsigned char offset2;
    unsigned char length2;
    unsigned char loop2;
    unsigned char speed;
    unsigned char distortion;
    unsigned char pSpeed;
} lsdj_instrument_kit_t;

typedef struct
{
    unsigned char length;
    unsigned char shape;
    unsigned char sCommand;
} lsdj_instrument_noise_t;
    
// Structure representing one instrument
typedef struct
{
    char name[INSTRUMENT_NAME_LENGTH];
    
    union
    {
        unsigned char data[16];
        
        struct
        {
            unsigned char type;
            
            union { unsigned char envelope; unsigned char volume; };
            unsigned char panning;
            unsigned char table;
            unsigned char automate;
            
            union
            {
                lsdj_instrument_pulse_t pulse;
                lsdj_instrument_wave_t wave;
                lsdj_instrument_kit_t kit;
                lsdj_instrument_noise_t noise;
            };
        };
    };
} lsdj_instrument_t;
    
// Clear all instrument data to factory settings
void lsdj_clear_instrument(lsdj_instrument_t* instrument);

#endif
