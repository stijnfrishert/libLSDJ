#ifndef LSDJ_TABLE_H
#define LSDJ_TABLE_H

#include "command.h"

#ifdef __cplusplus
extern "C" {
#endif
    
static const unsigned int TABLE_LENGTH = 16;
    
typedef struct
{
    unsigned char volumes[TABLE_LENGTH];
    unsigned char transposes[TABLE_LENGTH];
    lsdj_command_t commands1[TABLE_LENGTH];
    lsdj_command_t commands2[TABLE_LENGTH];
} lsdj_table_t;
    
void lsdj_clear_table(lsdj_table_t* table);
    
#ifdef __cplusplus
}
#endif

#endif

