#ifndef LSDJ_WORD_H
#define LSDJ_WORD_H

#ifdef __cplusplus
extern "C" {
#endif
    
static const unsigned int WORD_LENGTH = 16;
static const unsigned int WORD_NAME_LENGTH = 4;

typedef struct
{
    unsigned char allophones[WORD_LENGTH];
    unsigned char lengths[WORD_LENGTH];
} lsdj_word_t;
    
typedef struct
{
    char word_name[WORD_NAME_LENGTH];
} lsdj_word_name_t;
    
void lsdj_clear_word(lsdj_word_t* word);
    
#ifdef __cplusplus
}
#endif

#endif
