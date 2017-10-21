#ifndef LSDJ_GROOVE_H
#define LSDJ_GROOVE_H

#ifdef __cplusplus
extern "C" {
#endif
    
static const unsigned int GROOVE_LENGTH = 16;
static const unsigned char DEFAULT_GROOVE[GROOVE_LENGTH] = { 0x06, 0x06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

typedef struct
{
    unsigned char data[GROOVE_LENGTH];
} lsdj_groove_t;

void lsdj_clear_groove(lsdj_groove_t* groove);
    
#ifdef __cplusplus
}
#endif

#endif
