//
//  instrument_wave.h
//  liblsdj
//
//  Created by Stijn Frishert on 10/25/17.
//

#ifndef LSDJ_INSTRUMENT_WAVE_H
#define LSDJ_INSTRUMENT_WAVE_H

#include "instrument_constants.h"

typedef unsigned char lsdj_playback_mode;
static const lsdj_playback_mode LSDJ_PLAY_ONCE = 0;
static const lsdj_playback_mode LSDJ_PLAY_LOOP = 1;
static const lsdj_playback_mode LSDJ_PLAY_PING_PONG = 2;
static const lsdj_playback_mode LSDJ_PLAY_MANUAL = 3;

typedef struct
{
    lsdj_plvib_speed plvibSpeed;
    lsdj_vib_shape vibShape;
    lsdj_vib_direction vibratoDirection;
    lsdj_tuning_mode tuning;
    unsigned char synth;
    lsdj_playback_mode playback;
    unsigned char length;
    unsigned char repeat;
    unsigned char speed;
} lsdj_instrument_wave_t;

#endif
