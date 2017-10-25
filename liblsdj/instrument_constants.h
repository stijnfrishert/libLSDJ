//
//  instrument_constants.h
//  lsdj
//
//  Created by Stijn Frishert on 10/25/17.
//

#ifndef LSDJ_INSTRUMENT_CONSTANTS_H
#define LSDJ_INSTRUMENT_CONSTANTS_H

typedef unsigned char lsdj_plvib_type;
static const lsdj_plvib_type LSDJ_PLVIB_HIGH_FREQUENCY = 0;
static const lsdj_plvib_type LSDJ_PLVIB_SAWTOOTH = 1;
static const lsdj_plvib_type LSDJ_PLVIB_TRIANGLE = 2;
static const lsdj_plvib_type LSDJ_PLVIB_SQUARE = 3;

typedef unsigned char lsdj_vibrato_direction;
static const lsdj_vibrato_direction LSDJ_VIB_UP = 0;
static const lsdj_vibrato_direction LSDJ_VIB_DOWN = 1;

typedef enum
{
    TUNE_12_TONE,
    TUNE_FIXED,
    TUNE_DRUM
} tuning_mode;

#endif
