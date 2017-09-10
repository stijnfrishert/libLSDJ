#include <stdlib.h>
#include <string.h>

#include "song.h"

void lsdj_read_song_from_memory(const unsigned char* data, lsdj_song_t* song)
{
    memcpy(song->data, data, 0x8000);
}
