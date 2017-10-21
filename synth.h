#ifndef LSDJ_SYNTH_H
#define LSDJ_SYNTH_H

#include "command.h"

#ifdef __cplusplus
extern "C" {
#endif
    
static const unsigned int SYNTH_LENGTH = 16;
static unsigned char DEFAULT_SYNTH[SYNTH_LENGTH] = { 0, 0, 0, 0, 0, 0x10, 0xFF, 0, 0, 0x10, 0xFF, 0, 0, 0, 0,  };

typedef struct
{
    unsigned char data[SYNTH_LENGTH];
} lsdj_synth_t;

void lsdj_clear_synth(lsdj_synth_t* synth);
    
#ifdef __cplusplus
}
#endif

#endif
