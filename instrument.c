//
//  instrument.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include <string.h>

#include "instrument.h"

void lsdj_clear_instrument(lsdj_instrument_t* instrument)
{
    memset(instrument->name, 0, INSTRUMENT_NAME_LENGTH);
    memcpy(instrument->parameters, DEFAULT_INSTRUMENT, 16);
}
