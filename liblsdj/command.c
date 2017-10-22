//
//  command.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include "command.h"

void lsdj_clear_command(lsdj_command_t* command)
{
    command->command = 0;
    command->value = 0;
}
