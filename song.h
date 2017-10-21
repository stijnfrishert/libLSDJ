#ifndef LSDJ_SONG_H
#define LSDJ_SONG_H

#include "chain.h"
#include "error.h"
#include "instrument.h"
#include "phrase.h"
#include "row.h"
#include "table.h"
#include "vio.h"

#ifdef __cplusplus
extern "C" {
#endif
    
static const unsigned int SONG_DECOMPRESSED_SIZE = 0x8000;
static const unsigned int ROW_COUNT = 256;
static const unsigned int CHAIN_COUNT = 128;
static const unsigned int PHRASE_COUNT = 0xFF;
static const unsigned int BOOKMARK_COUNT = 64;
static const unsigned int INSTRUMENT_COUNT = 64;
static const unsigned int SYNTH_COUNT = 16;
static const unsigned int TABLE_COUNT = 32;
static const unsigned int WAVE_COUNT = 256;
static const unsigned int WAVE_LENGTH = 16;
static const unsigned int GROOVE_LENGTH = 16;
static const unsigned int GROOVE_COUNT = 32;

//! An LSDJ song
typedef struct
{
    unsigned char version;
    
    //! The sequences of chains in the song
    lsdj_row_t rows[ROW_COUNT];
    
    //! The chains in the song
    lsdj_chain_t chains[CHAIN_COUNT];
    
    //! The prases in the song
    lsdj_phrase_t phrases[PHRASE_COUNT];
    
    //! Instruments of the song
    lsdj_instrument_t instruments[INSTRUMENT_COUNT];
    
    //! Wave frames of the song
    unsigned char waves[WAVE_COUNT][WAVE_LENGTH];
    
    //! The tables in the song
    lsdj_table_t tables[TABLE_COUNT];
    
    //! The grooves in the song
    unsigned char grooves[GROOVE_COUNT][GROOVE_LENGTH];
    
    //! Bookmarks
    unsigned char bookmarks[BOOKMARK_COUNT];
    
    unsigned char empty1030[96];
    unsigned char instrumentSpeechWords[1344];
    unsigned char instrumentSpeechWordNames[168];
    unsigned char empty1fba[70];
    
    unsigned char empty2000[32];
    unsigned char chainAllocTable[16];
    unsigned char phraseAllocTable[32];
    unsigned char instrAllocTable[64];
    unsigned char tableAllocTable[32];
    unsigned char softSynthParams[16][SYNTH_COUNT];
    
    struct
    {
        unsigned char hours;
        unsigned char minutes;
    } workTime;
    
    unsigned char tempo;
    unsigned char tuneSetting;
    
    struct
    {
        unsigned char days;
        unsigned char hours;
        unsigned char minutes;
    } totalTime;
    
    unsigned char empty3fb9;
    
    unsigned char keyDelay;
    unsigned char keyRepeat;
    unsigned char font;
    unsigned char syncSetting;
    unsigned char colorSet;
    unsigned char empty3fbf;
    unsigned char clone;
    unsigned char fileChangedFlag;
    unsigned char powerSave;
    unsigned char preListen;
    unsigned char waveSynthOverwriteLocks[2];
    unsigned char empty3fc6[58];
    unsigned char empty5fe0[32];
    unsigned char empty7ff2[13];
} lsdj_song_t;

void lsdj_read_song(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song, lsdj_error_t** error);
void lsdj_read_song_from_memory(const unsigned char* data, size_t size, lsdj_song_t* song, lsdj_error_t** error);
    
//! Write an LSDJ song to memory
void lsdj_write_song(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data, lsdj_error_t** error);
void lsdj_write_song_to_memory(const lsdj_song_t* song, unsigned char* data, size_t size, lsdj_error_t** error);
    
void lsdj_clear_song(lsdj_song_t* song);

#ifdef __cplusplus
}
#endif

#endif
