//
//  table.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include <string.h>

#include "table.h"

void lsdj_clear_table(lsdj_table_t* table)
{
    memset(table->volumes, 0, TABLE_LENGTH);
    memset(table->transposes, 0, TABLE_LENGTH);
    
    for (int i = 0; i < TABLE_LENGTH; ++i)
    {
        lsdj_clear_command(&table->commands1[i]);
        lsdj_clear_command(&table->commands2[i]);
    }
}
