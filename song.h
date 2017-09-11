#ifndef LSDJ_SONG_H
#define LSDJ_SONG_H

#ifdef __cplusplus
extern "C" {
#endif

//! An LSDJ song
typedef struct
{
    unsigned char data[0x8000];
} lsdj_song_t;
    
//! Read an LSDJ song from memory
void lsdj_read_song_from_memory(const unsigned char* data, lsdj_song_t* song);
    
//! Write an LSDJ song to memory
void lsdj_write_song_to_memory(const lsdj_song_t* song, unsigned char* data);

#ifdef __cplusplus
}
#endif

#endif
