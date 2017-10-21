#ifndef LSDJ_GROOVE_H
#define LSDJ_GROOVE_H

// The default constant length of a groove
static const unsigned int GROOVE_LENGTH = 16;

// Structure representing a groove
typedef struct
{
    unsigned char data[GROOVE_LENGTH];
} lsdj_groove_t;

    // Clear all groove data to factory settings
void lsdj_clear_groove(lsdj_groove_t* groove);
    
#endif
