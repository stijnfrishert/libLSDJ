#include <stdlib.h>
#include <string.h>

#include "song.h"

void copy_and_increase(const unsigned char** source, unsigned char* destination, unsigned int size)
{
    memcpy(destination, *source, size);
    *source += size;
}

void read_bank0(const unsigned char** read, lsdj_song_t* song)
{
    copy_and_increase(read, song->phrases[0], sizeof(song->phrases));
    copy_and_increase(read, song->bookmarks, sizeof(song->bookmarks));
    *read += 96; // empty space
    copy_and_increase(read, song->grooves, sizeof(song->grooves));
    copy_and_increase(read, song->sequences[0], sizeof(song->sequences));
    *read += 512; // tables->envelope
    *read += 1344; // instrument->speech->words
    *read += 168; // instrument->speed->wordnames
    *read += 2; // 'rb'
    
    // Read the instrument names
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
        copy_and_increase(read, (unsigned char*)song->instruments[i].name, sizeof(song->instruments[i].name));
    
    *read += 70; // empty
}

void read_bank1(const unsigned char** read, lsdj_song_t* song)
{
    *read += 32; // empty
}

void read_bank2(const unsigned char** read, lsdj_song_t* song)
{
    
}

void read_bank3(const unsigned char** read, lsdj_song_t* song)
{
    
}

void lsdj_read_song_from_memory(const unsigned char* data, lsdj_song_t* song, lsdj_error_t** error)
{
    if (data[0x1E78] != 'r' || data[0x1E79] != 'b')
    {
        lsdj_create_error(error, "memory flag 'rb' not found");
        return;
    }
    
    const unsigned char* read = data;
    
    memcpy(song->data, read, 0x8000);
    
    read_bank0(&read, song);
    read_bank1(&read, song);
    read_bank2(&read, song);
    read_bank3(&read, song);
}

void lsdj_write_song_to_memory(const lsdj_song_t* song, unsigned char* data)
{
    memcpy(data, song->data, 0x8000);
}
