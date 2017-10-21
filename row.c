//
//  row.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include "row.h"

void lsdj_clear_row(lsdj_row_t* row)
{
    row->pulse1 = 0xFF;
    row->pulse2 = 0xFF;
    row->wave = 0xFF;
    row->noise = 0xFF;
}
