#ifndef LSDJ_WORD_H
#define LSDJ_WORD_H

#ifdef __cplusplus
extern "C" {
#endif
    
// The default constant length of a word
static const unsigned int WORD_LENGTH = 16;
    
// The constant length of all word names
static const unsigned int WORD_NAME_LENGTH = 4;

// Structure representing word data for the speech synthesizer
typedef struct
{
    unsigned char allophones[WORD_LENGTH];
    unsigned char lengths[WORD_LENGTH];
} lsdj_word_t;
    
// Clear all word data to factory settings
void lsdj_clear_word(lsdj_word_t* word);
    
#ifdef __cplusplus
}
#endif

#endif
