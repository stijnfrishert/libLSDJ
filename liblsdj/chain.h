#ifndef LSDJ_CHAIN_H
#define LSDJ_CHAIN_H

#include "command.h"

// The length of a chain
#define CHAIN_LENGTH 16
    
// Structure representing a chain
typedef struct
{
    // The phrases in the chain (indices, actual phrases are stored in the song)
    unsigned char phrases[CHAIN_LENGTH];
    
    // The transpositions for each row
    unsigned char transpositions[CHAIN_LENGTH];
} lsdj_chain_t;
    
// Clear chain data to factory settings
void lsdj_clear_chain(lsdj_chain_t* chain);

#endif
