#ifndef LSDJ_CHAIN_H
#define LSDJ_CHAIN_H

#include "command.h"

#ifdef __cplusplus
extern "C" {
#endif
    
static const unsigned int CHAIN_LENGTH = 16;
    
typedef struct
{
    unsigned char phraseNumbers[CHAIN_LENGTH];
    unsigned char transposes[CHAIN_LENGTH];
} lsdj_chain_t;
    
void lsdj_clear_chain(lsdj_chain_t* chain);
    
#ifdef __cplusplus
}
#endif

#endif
