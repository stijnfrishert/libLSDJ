//
//  synth.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include <string.h>

#include "synth.h"

void lsdj_clear_synth(lsdj_synth_t* synth)
{
    memcpy(synth->data, DEFAULT_SYNTH, SYNTH_LENGTH);
}
