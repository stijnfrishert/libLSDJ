//
//  instrument_noise.h
//  liblsdj
//
//  Created by Stijn Frishert on 10/25/17.
//

#ifndef LSDJ_INSTRUMENT_NOISE_H
#define LSDJ_INSTRUMENT_NOISE_H

typedef unsigned char lsdj_scommand_type;
static const lsdj_scommand_type LSDJ_SCOMMAND_FREE = 0;
static const lsdj_scommand_type LSDJ_SCOMMAND_STABLE = 1;

typedef struct
{
    unsigned char length; // 0x40 and above = unlimited
    unsigned char shape;
    lsdj_scommand_type sCommand;
} lsdj_instrument_noise_t;

#endif
