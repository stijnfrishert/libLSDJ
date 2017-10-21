#ifndef LSDJ_ROW_H
#define LSDJ_ROW_H

// Structure representing a row in an LSDJ song sequence
typedef struct
{
    // Four chain indices
    /* The chains themselves are stored in the encompassing song */
    unsigned char pulse1;
    unsigned char pulse2;
    unsigned char wave;
    unsigned char noise;
} lsdj_row_t;
    
// Clear all row data to factory settings
void lsdj_clear_row(lsdj_row_t* row);

#endif
