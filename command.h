#ifndef LSDJ_COMMAND_H
#define LSDJ_COMMAND_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct
{
    unsigned char command;
    unsigned char value;
} lsdj_command_t;
    
void lsdj_clear_command(lsdj_command_t* command);
    
#ifdef __cplusplus
}
#endif

#endif
