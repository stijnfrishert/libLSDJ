//
//  word.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include <string.h>

#include "word.h"

void lsdj_clear_word(lsdj_word_t* word)
{
    memset(word->allophones, 0, WORD_LENGTH);
    memset(word->lengths, 0, WORD_LENGTH);
}
