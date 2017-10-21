#ifndef LSDJ_PHRASE_H
#define LSDJ_PHRASE_H

#include "command.h"

#ifdef __cplusplus
extern "C" {
#endif
    
static const unsigned int PHRASE_LENGTH = 16;
    
typedef struct
{
    unsigned char notes[PHRASE_LENGTH];
    unsigned char instruments[PHRASE_LENGTH];
    lsdj_command_t commands[PHRASE_LENGTH];
} lsdj_phrase_t;
    
void lsdj_clear_phrase(lsdj_phrase_t* phrase);
    
#ifdef __cplusplus
}
#endif

#endif
