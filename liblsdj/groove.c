//
//  groove.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include <string.h>

#include "groove.h"

static const unsigned char DEFAULT_GROOVE[GROOVE_LENGTH] = { 0x06, 0x06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void lsdj_clear_groove(lsdj_groove_t* groove)
{
    memcpy(groove->data, DEFAULT_GROOVE, GROOVE_LENGTH);
}

