//
//  wave.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include <string.h>

#include "wave.h"

void lsdj_clear_wave(lsdj_wave_t* wave)
{
    memcpy(wave->data, DEFAULT_WAVE, WAVE_LENGTH);
}
