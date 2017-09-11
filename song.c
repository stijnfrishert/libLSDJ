#include <stdlib.h>
#include <string.h>

#include "song.h"

void lsdj_read_song_from_memory(const unsigned char* data, lsdj_song_t* song)
{
    memcpy(song->data, data, 0x8000);
}

void lsdj_write_song_to_memory(const lsdj_song_t* song, unsigned char* data)
{
    memcpy(data, song->data, 0x8000);
}
