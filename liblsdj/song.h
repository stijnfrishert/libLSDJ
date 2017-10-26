#ifndef LSDJ_SONG_H
#define LSDJ_SONG_H

#include "chain.h"
#include "error.h"
#include "groove.h"
#include "instrument.h"
#include "phrase.h"
#include "row.h"
#include "synth.h"
#include "table.h"
#include "vio.h"
#include "wave.h"
#include "word.h"

static const unsigned int SONG_DECOMPRESSED_SIZE = 0x8000;
static const unsigned int ROW_COUNT = 256;
static const unsigned int CHAIN_COUNT = 128;
static const unsigned int PHRASE_COUNT = 0xFF;
static const unsigned int BOOKMARK_COUNT = 64;
static const unsigned int INSTRUMENT_COUNT = 64;
static const unsigned int SYNTH_COUNT = 16;
static const unsigned int TABLE_COUNT = 32;
static const unsigned int WAVE_COUNT = 256;
static const unsigned int GROOVE_COUNT = 32;
static const unsigned int WORD_COUNT = 42;
    
static const unsigned char CLONE_DEEP = 0;
static const unsigned char CLONE_SLIM = 1;

// An LSDJ song
typedef struct lsdj_song_t lsdj_song_t;

// Create/free projects
lsdj_song_t* lsdj_new_song(lsdj_error_t** error);
void lsdj_free_song(lsdj_song_t* song);

// Deserialize a song
lsdj_song_t* lsdj_read_song(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_error_t** error);
lsdj_song_t* lsdj_read_song_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error);
    
// Serialize a song
void lsdj_write_song(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data, lsdj_error_t** error);
void lsdj_write_song_to_memory(const lsdj_song_t* song, unsigned char* data, size_t size, lsdj_error_t** error);

// Change data in a song
void lsdj_song_set_format_version(lsdj_song_t* song, unsigned char version);
unsigned char lsdj_song_get_format_version(lsdj_song_t* song);
unsigned char lsdj_song_get_tempo(lsdj_song_t* song);
void lsdj_song_set_tempo(lsdj_song_t* song, unsigned char tempo);
unsigned char lsdj_song_get_transposition(lsdj_song_t* song);
void lsdj_song_set_transposition(lsdj_song_t* song, unsigned char transposition);

#endif
