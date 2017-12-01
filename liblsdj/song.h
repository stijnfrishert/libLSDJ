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

#define SONG_DECOMPRESSED_SIZE 0x8000
#define ROW_COUNT 256
#define CHAIN_COUNT 128
#define PHRASE_COUNT 0xFF
#define BOOKMARK_COUNT 64
#define INSTRUMENT_COUNT 64
#define SYNTH_COUNT 16
#define TABLE_COUNT 32
#define WAVE_COUNT 256
#define GROOVE_COUNT 32
#define WORD_COUNT 42
    
static const unsigned char CLONE_DEEP = 0;
static const unsigned char CLONE_SLIM = 1;

// An LSDJ song
typedef struct lsdj_song_t lsdj_song_t;

// Create/free projects
lsdj_song_t* lsdj_new_song(lsdj_error_t** error);
lsdj_song_t* lsdj_copy_song(const lsdj_song_t* song, lsdj_error_t** error);
void lsdj_free_song(lsdj_song_t* song);

// Deserialize a song
lsdj_song_t* lsdj_read_song(lsdj_vio_t* vio, lsdj_error_t** error);
lsdj_song_t* lsdj_read_song_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error);
    
// Serialize a song
void lsdj_write_song(const lsdj_song_t* song, lsdj_vio_t* vio, lsdj_error_t** error);
void lsdj_write_song_to_memory(const lsdj_song_t* song, unsigned char* data, size_t size, lsdj_error_t** error);

// Change data in a song
void lsdj_song_set_format_version(lsdj_song_t* song, unsigned char version);
unsigned char lsdj_song_get_format_version(const lsdj_song_t* song);
void lsdj_song_set_tempo(lsdj_song_t* song, unsigned char tempo);
unsigned char lsdj_song_get_tempo(const lsdj_song_t* song);
void lsdj_song_set_transposition(lsdj_song_t* song, unsigned char transposition);
unsigned char lsdj_song_get_transposition(const lsdj_song_t* song);
unsigned char lsdj_song_get_file_changed_flag(const lsdj_song_t* song);

#endif
