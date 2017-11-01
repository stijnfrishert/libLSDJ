//
//  instrument_kit.h
//  liblsdj
//
//  Created by Stijn Frishert on 10/25/17.
//

#ifndef LSDJ_INSTRUMENT_KIT_H
#define LSDJ_INSTRUMENT_KIT_H

#include "instrument_constants.h"

typedef unsigned char lsdj_kit_loop_mode;
static const lsdj_kit_loop_mode LSDJ_KIT_LOOP_OFF = 0;
static const lsdj_kit_loop_mode LSDJ_KIT_LOOP_ON = 1;
static const lsdj_kit_loop_mode LSDJ_KIT_LOOP_ATTACK = 2;

typedef unsigned char lsdj_kit_distortion;
static const lsdj_kit_distortion LSDJ_KIT_DIST_CLIP = 0xD0;
static const lsdj_kit_distortion LSDJ_KIT_DIST_SHAPE = 0xD1;
static const lsdj_kit_distortion LSDJ_KIT_DIST_SHAPE2 = 0xD2;
static const lsdj_kit_distortion LSDJ_KIT_DIST_WRAP = 0xD3;

typedef unsigned char lsdj_kit_pspeed;
static const lsdj_kit_pspeed LSDJ_KIT_PSPEED_FAST = 0;
static const lsdj_kit_pspeed LSDJ_KIT_PSPEED_SLOW = 1;
static const lsdj_kit_pspeed LSDJ_KIT_PSPEED_STEP = 2;

typedef struct
{
    unsigned char kit1;
    unsigned char offset1;
    unsigned char length1;
    lsdj_kit_loop_mode loop1;
    
    unsigned char kit2;
    unsigned char offset2;
    unsigned char length2;
    lsdj_kit_loop_mode loop2;
    
    unsigned char pitch;
    unsigned char halfSpeed;
    lsdj_kit_distortion distortion;
    lsdj_plvib_speed plvibSpeed;
    lsdj_vib_shape vibShape;
    lsdj_vib_direction vibratoDirection;
} lsdj_instrument_kit_t;

#endif
