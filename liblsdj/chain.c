//
//  chain.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include <string.h>

#include "chain.h"

void lsdj_clear_chain(lsdj_chain_t* chain)
{
    memset(chain->phrases, 0xFF, CHAIN_LENGTH);
    memset(chain->transpositions, 0, CHAIN_LENGTH);
}
