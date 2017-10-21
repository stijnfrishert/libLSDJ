//
//  phrase.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include <string.h>

#include "phrase.h"

void lsdj_clear_phrase(lsdj_phrase_t* phrase)
{
    memset(phrase->notes, 0, PHRASE_LENGTH);
    memset(phrase->instruments, 0xFF, PHRASE_LENGTH);
    
    for (int i = 0; i < PHRASE_LENGTH; ++i)
        lsdj_clear_command(&phrase->commands[i]);
}
