//
//  instrument_pulse.h
//  liblsdj
//
//  Created by Stijn Frishert on 10/25/17.
//

#ifndef LSDJ_INSTRUMENT_PULSE_H
#define LSDJ_INSTRUMENT_PULSE_H

#include "instrument_constants.h"

typedef unsigned char lsdj_pulse_wave;
static const lsdj_pulse_wave LSDJ_PULSE_WAVE_PW_125 = 0;
static const lsdj_pulse_wave LSDJ_PULSE_WAVE_PW_25 = 1;
static const lsdj_pulse_wave LSDJ_PULSE_WAVE_PW_50 = 2;
static const lsdj_pulse_wave LSDJ_PULSE_WAVE_PW_75 = 3;

typedef struct
{
    lsdj_pulse_wave pulseWidth;
    unsigned char length; // 0x40 and above = unlimited
    unsigned char sweep;
    lsdj_plvib_type plvib;
    lsdj_vibrato_direction vibratoDirection;
    tuning_mode tuning;
    unsigned char pulse2tune;
    unsigned char fineTune;
} lsdj_instrument_pulse_t;

#endif
