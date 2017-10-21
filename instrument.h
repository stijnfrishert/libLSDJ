//
//  instrument.h
//  lsdj
//
//  Created by Stijn Frishert on 9/11/17.
//
//

#ifndef LSDJ_INSTRUMENT_H
#define LSDJ_INSTRUMENT_H

// The default constant length of an instrument name
static const unsigned int INSTRUMENT_NAME_LENGTH = 5;
    
static const unsigned char DEFAULT_INSTRUMENT[16] = { 0, 0xA8, 0, 0, 0xFF, 0, 0, 3, 0, 0, 0xD0, 0, 0, 0, 0xF3, 0 };
    
static const unsigned char INSTR_TYPE_PULSE = 0;
static const unsigned char INSTR_TYPE_WAVE = 1;
static const unsigned char INSTR_TYPE_KIT = 2;
static const unsigned char INSTR_TYPE_NOISE = 3;
    
// Structure representing one instrument
typedef struct
{
    char name[INSTRUMENT_NAME_LENGTH];
    
    union
    {
        unsigned char data[16];
        
        struct
        {
            unsigned char type;
            
            union { unsigned char envelope; unsigned char volume; };
            
            union
            {
                struct
                {
                    
                } pulse;
                
                struct
                {
                    
                } wave;
                
                struct
                {
                    
                } kit;
                
                struct
                {
                    
                } noise;
            };
        };
    };
} lsdj_instrument_t;
    
// Clear all instrument data to factory settings
void lsdj_clear_instrument(lsdj_instrument_t* instrument);

#endif
