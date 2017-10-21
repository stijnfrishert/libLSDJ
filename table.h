#ifndef LSDJ_TABLE_H
#define LSDJ_TABLE_H

#include "command.h"

#ifdef __cplusplus
extern "C" {
#endif
    
// The default constant length of a table
static const unsigned int TABLE_LENGTH = 16;
    
typedef struct
{
    // The volume column of the table
    unsigned char volumes[TABLE_LENGTH];
    
    // The transposition column of the table
    unsigned char transpositions[TABLE_LENGTH];
    
    // The first effect command column of the table
    lsdj_command_t commands1[TABLE_LENGTH];
    
    // The second effect command column of the table
    lsdj_command_t commands2[TABLE_LENGTH];
} lsdj_table_t;
    
// Clear all table data to factory settings
void lsdj_clear_table(lsdj_table_t* table);
    
#ifdef __cplusplus
}
#endif

#endif
