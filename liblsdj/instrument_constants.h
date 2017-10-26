//
//  instrument_constants.h
//  lsdj
//
//  Created by Stijn Frishert on 10/25/17.
//

#ifndef LSDJ_INSTRUMENT_CONSTANTS_H
#define LSDJ_INSTRUMENT_CONSTANTS_H

typedef unsigned char lsdj_plvib_speed;
static const lsdj_plvib_speed LSDJ_PLVIB_FAST = 0;
static const lsdj_plvib_speed LSDJ_PLVIB_TICK = 1;
static const lsdj_plvib_speed LSDJ_PLVIB_STEP = 2;

typedef unsigned char lsdj_vib_shape;
#define LSDJ_VIB_TRIANGLE 0
#define LSDJ_VIB_SAWTOOTH 1
#define LSDJ_VIB_SQUARE 2

typedef unsigned char lsdj_vib_direction;
static const lsdj_vib_direction LSDJ_VIB_UP = 0;
static const lsdj_vib_direction LSDJ_VIB_DOWN = 1;

typedef unsigned char lsdj_tuning_mode;
static const lsdj_tuning_mode LSDJ_TUNE_12_TONE = 0;
static const lsdj_tuning_mode LSDJ_TUNE_FIXED = 1;
static const lsdj_tuning_mode LSDJ_TUNE_DRUM = 2;

#endif
