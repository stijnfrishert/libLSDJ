#ifndef LSDJ_PHRASE_H
#define LSDJ_PHRASE_H

#include "command.h"

// The default constant lenght of a phrase
#define PHRASE_LENGTH 16

typedef struct
{
    // The notes in the phrase
    unsigned char notes[PHRASE_LENGTH];
    
    // The instruments the phrase uses for each row
    unsigned char instruments[PHRASE_LENGTH];
    
    // The effect command column for a phrase
    lsdj_command_t commands[PHRASE_LENGTH];
} lsdj_phrase_t;
    
// Clear all phrase data to factory settings
void lsdj_clear_phrase(lsdj_phrase_t* phrase);

#endif
