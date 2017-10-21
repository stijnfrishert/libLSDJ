//
//  instrument.h
//  lsdj
//
//  Created by Stijn Frishert on 9/11/17.
//
//

#ifndef LSDJ_INSTRUMENT_H
#define LSDJ_INSTRUMENT_H

#ifdef __cplusplus
extern "C" {
#endif
    
static const unsigned int INSTRUMENT_NAME_LENGTH = 5;
    
//! Structure representing one instrument
typedef struct
{
    //! The name of an instrument
    char name[INSTRUMENT_NAME_LENGTH];
    unsigned char parameters[16];
} lsdj_instrument_t;
    
void lsdj_clear_instrument(lsdj_instrument_t* instrument);
    
#ifdef __cplusplus
}
#endif


#endif /* LSDJ_INSTRUMENT_H */
