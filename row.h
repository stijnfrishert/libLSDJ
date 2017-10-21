#ifndef LSDJ_ROW_H
#define LSDJ_ROW_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    unsigned char pulse1;
    unsigned char pulse2;
    unsigned char wave;
    unsigned char noise;
} lsdj_row_t;
    
void lsdj_clear_row(lsdj_row_t* row);
    
#ifdef __cplusplus
}
#endif


#endif /* LSDJ_ROW_H */

