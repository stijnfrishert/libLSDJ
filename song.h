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
    
//! Decompress a song
void lsdj_decompress_song(const unsigned char* data, lsdj_song_t* song);

#ifdef __cplusplus
}
#endif

#endif
