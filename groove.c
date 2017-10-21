//
//  groove.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include <string.h>

#include "groove.h"

void lsdj_clear_groove(lsdj_groove_t* groove)
{
    memcpy(groove->data, DEFAULT_GROOVE, GROOVE_LENGTH);
}

