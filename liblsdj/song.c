/*
 
 This file is a part of liblsdj, a C library for managing everything
 that has to do with LSDJ, software for writing music (chiptune) with
 your gameboy. For more information, see:
 
 * https://github.com/stijnfrishert/liblsdj
 * http://www.littlesounddj.com
 
 --------------------------------------------------------------------------------
 
 MIT License
 
 Copyright (c) 2018 Stijn Frishert
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 */

#include <stdlib.h>
#include <string.h>

#include "chain.h"
#include "error.h"
#include "groove.h"
#include "instrument.h"
#include "phrase.h"
#include "row.h"
#include "song.h"
#include "synth.h"
#include "table.h"
#include "vio.h"
#include "wave.h"
#include "word.h"

static char DEFAULT_WORD_NAMES[WORD_COUNT][WORD_NAME_LENGTH] =
{
    {'C',' ','2',' '},{'C',' ','2',' '},{'D',' ','2',' '},{'D',' ','2',' '},{'E',' ','2',' '},{'F',' ','2',' '},{'F',' ','2',' '},{'G',' ','2',' '},{'G',' ','2',' '},{'A',' ','2',' '},{'A',' ','2',' '},{'B',' ','2',' '},{'C',' ','3',' '},{'C',' ','3',' '},{'D',' ','3',' '},{'D',' ','3',' '},{'E',' ','3',' '},{'F',' ','3',' '},{'F',' ','3',' '},{'G',' ','3',' '},{'G',' ','3',' '},{'A',' ','3',' '},{'A',' ','3',' '},{'B',' ','3',' '},{'C',' ','4',' '},{'C',' ','4',' '},{'D',' ','4',' '},{'D',' ','4',' '},{'E',' ','4',' '},{'F',' ','4',' '},{'F',' ','4',' '},{'G',' ','4',' '},{'G',' ','4',' '},{'A',' ','4',' '},{'A',' ','4',' '},{'B',' ','4',' '},{'C',' ','5',' '},{'C',' ','5',' '},{'D',' ','5',' '},{'D',' ','5',' '},{'E',' ','5',' '},{'F',' ','5',' '}
};

#define INSTR_ALLOC_TABLE_SIZE 64
#define TABLE_ALLOC_TABLE_SIZE 32
#define CHAIN_ALLOC_TABLE_SIZE 16
#define PHRASE_ALLOC_TABLE_SIZE 32

static unsigned char TABLE_LENGTH_ZERO[TABLE_LENGTH] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char CHAIN_LENGTH_ZERO[TABLE_LENGTH] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char CHAIN_LENGTH_FF[TABLE_LENGTH] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static unsigned char PHRASE_LENGTH_ZERO[PHRASE_LENGTH] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char PHRASE_LENGTH_FF[PHRASE_LENGTH] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

struct lsdj_song_t
{
    unsigned char formatVersion;
    unsigned char tempo;
    unsigned char transposition;
    unsigned char drumMax;
    
    // The sequences of chains in the song
    lsdj_row_t rows[ROW_COUNT];
    
    // The chains in the song
    lsdj_chain_t* chains[CHAIN_COUNT];
    
    // The prases in the song
    lsdj_phrase_t* phrases[PHRASE_COUNT];
    
    // Instruments of the song
    lsdj_instrument_t* instruments[INSTRUMENT_COUNT];
    
    // Soft synths of the song
    lsdj_synth_t synths[SYNTH_COUNT];
    
    // Wave frames of the song
    lsdj_wave_t waves[WAVE_COUNT];
    
    // The tables in the song
    lsdj_table_t* tables[TABLE_COUNT];
    
    // The grooves in the song
    lsdj_groove_t grooves[GROOVE_COUNT];
    
    // The speech synth words in the song
    lsdj_word_t words[WORD_COUNT];
    char wordNames[WORD_COUNT][WORD_NAME_LENGTH];
    
    // Bookmarks
    unsigned char bookmarks[BOOKMARK_COUNT];
    
    struct
    {
        unsigned char keyDelay;
        unsigned char keyRepeat;
        unsigned char font;
        unsigned char sync;
        unsigned char colorSet;
        unsigned char clone;
        unsigned char fileChangedFlag;
        unsigned char powerSave;
        unsigned char preListen;
        
        struct
        {
            unsigned char days;
            unsigned char hours;
            unsigned char minutes;
        } totalTime;
        
        struct
        {
            unsigned char hours;
            unsigned char minutes;
        } workTime;
    } meta;
    
    unsigned char reserved1030[96];
    unsigned char reserved1fba[70];
    unsigned char reserved2000[32];
    unsigned char reserved3fbf;
    unsigned char reserved3fb9;
    unsigned char reserved3fc6[10];
    unsigned char reserved3fd1[47];
    unsigned char reserved5fe0[32];
    unsigned char reserved7ff2[13];
};

lsdj_song_t* alloc_song(lsdj_error_t** error)
{
    lsdj_song_t* song = (lsdj_song_t*)calloc(sizeof(lsdj_song_t), 1);
    if (song == NULL)
    {
        lsdj_create_error(error, "could not allocate song");
        return NULL;
    }
    
    return song;
}

lsdj_song_t* lsdj_new_song(lsdj_error_t** error)
{
    lsdj_song_t* song = alloc_song(error);
    if (song == NULL)
        return NULL;
    
    song->formatVersion = 4;
    song->tempo = 128;
    song->transposition = 0;
    song->drumMax = 0x6C;
    
    for (int i = 0; i < ROW_COUNT; ++i)
        lsdj_clear_row(&song->rows[i]);
    
    memset(song->chains, 0, sizeof(song->chains));
    memset(song->phrases, 0, sizeof(song->phrases));
    memset(song->instruments, 0, sizeof(song->instruments));
    memset(song->synths, 0, sizeof(song->synths));
    
    for (int i = 0; i < WAVE_COUNT; ++i)
        lsdj_clear_wave(&song->waves[i]);
    
    memset(song->tables, 0, sizeof(song->tables));
    
    for (int i = 0; i < GROOVE_COUNT; ++i)
        lsdj_clear_groove(&song->grooves[i]);
    
    for (int i = 0; i < WORD_COUNT; ++i)
        lsdj_clear_word(&song->words[i]);
    
    memcpy(song->wordNames, DEFAULT_WORD_NAMES, sizeof(song->wordNames));
    memset(song->bookmarks, 0xFF, sizeof(song->bookmarks));
    
    song->meta.keyDelay = 7;
    song->meta.keyRepeat = 2;
    song->meta.font = 0;
    song->meta.sync = 0;
    song->meta.colorSet = 0;
    song->meta.clone = 0;
    song->meta.fileChangedFlag = 0;
    song->meta.powerSave = 0;
    song->meta.preListen = 1;
    
    song->meta.totalTime.days = 0;
    song->meta.totalTime.hours = 0;
    song->meta.totalTime.minutes = 0;
    song->meta.workTime.hours = 0;
    song->meta.workTime.minutes = 0;
    
    return song;
}

lsdj_song_t* lsdj_copy_song(const lsdj_song_t* rhs, lsdj_error_t** error)
{
    lsdj_song_t* song = lsdj_new_song(error);
    if (*error)
        return NULL;
    
    song->formatVersion = rhs->formatVersion;
    song->tempo = rhs->tempo;
    song->transposition = rhs->transposition;
    song->drumMax = rhs->drumMax;
    
    memcpy(song->rows, rhs->rows, sizeof(rhs->rows));
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if (rhs->chains[i])
            song->chains[i] = lsdj_copy_chain(rhs->chains[i]);
    }
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (rhs->phrases[i])
            song->phrases[i] = lsdj_copy_phrase(rhs->phrases[i]);
    }
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (rhs->instruments[i])
            song->instruments[i] = lsdj_copy_instrument(rhs->instruments[i]);
    }
    
    memcpy(song->synths, rhs->synths, sizeof(rhs->synths));
    memcpy(song->waves, rhs->waves, sizeof(rhs->waves));

    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (rhs->tables[i])
            song->tables[i] = lsdj_copy_table(rhs->tables[i]);
    }
    
    memcpy(song->grooves, rhs->grooves, sizeof(rhs->grooves));
    memcpy(song->words, rhs->words, sizeof(rhs->words));
    memcpy(song->wordNames, rhs->wordNames, sizeof(rhs->wordNames));
    memcpy(song->bookmarks, rhs->bookmarks, sizeof(rhs->bookmarks));
    
    memcpy(&song->meta, &rhs->meta, sizeof(rhs->meta));
    
    memcpy(song->reserved1030, rhs->reserved1030, sizeof(rhs->reserved1030));
    memcpy(song->reserved1fba, rhs->reserved1fba, sizeof(rhs->reserved1fba));
    memcpy(song->reserved2000, rhs->reserved2000, sizeof(rhs->reserved2000));
    memcpy(&song->reserved3fbf, &rhs->reserved3fbf, sizeof(rhs->reserved3fbf));
    memcpy(&song->reserved3fb9, &rhs->reserved3fb9, sizeof(rhs->reserved3fb9));
    memcpy(song->reserved3fc6, rhs->reserved3fc6, sizeof(rhs->reserved3fc6));
    memcpy(song->reserved3fd1, rhs->reserved3fd1, sizeof(rhs->reserved3fd1));
    memcpy(song->reserved5fe0, rhs->reserved5fe0, sizeof(rhs->reserved5fe0));
    memcpy(song->reserved7ff2, rhs->reserved7ff2, sizeof(rhs->reserved7ff2));
    
    return song;
}

void lsdj_free_song(lsdj_song_t* song)
{
    if (song == NULL)
        return;
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if (song->chains[i])
            free(song->chains[i]);
    }
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
            free(song->phrases[i]);
    }
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (song->instruments[i])
            free(song->instruments[i]);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
            free(song->tables[i]);
    }
    
    free(song);
}

void read_bank0(lsdj_vio_t* vio, lsdj_song_t* song)
{
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
            vio->read(song->phrases[i]->notes, PHRASE_LENGTH, vio->user_data);
        else
            vio->seek(PHRASE_LENGTH, SEEK_CUR, vio->user_data);
    }

    vio->read(song->bookmarks, sizeof(song->bookmarks), vio->user_data);
    vio->read(song->reserved1030, sizeof(song->reserved1030), vio->user_data);
    vio->read(song->grooves, sizeof(song->grooves), vio->user_data);
    vio->read(song->rows, sizeof(song->rows), vio->user_data);
    
    unsigned char volumes[TABLE_LENGTH];
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            vio->read(volumes, TABLE_LENGTH, vio->user_data);
            lsdj_table_set_volumes(song->tables[i], volumes);
        }
        else
            vio->seek(TABLE_LENGTH, SEEK_CUR, vio->user_data);
    }
    
    for (int i = 0; i < WORD_COUNT; ++i)
    {
        vio->read(song->words[i].allophones, WORD_LENGTH, vio->user_data);
        vio->read(song->words[i].lengths, WORD_LENGTH, vio->user_data);
    }
    
    vio->read(song->wordNames, sizeof(song->wordNames), vio->user_data);
    vio->seek(2, SEEK_CUR, vio->user_data); // rb
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (song->instruments[i])
            vio->read(song->instruments[i]->name, INSTRUMENT_NAME_LENGTH, vio->user_data);
        else
            vio->seek(INSTRUMENT_NAME_LENGTH, SEEK_CUR, vio->user_data);
    }

    vio->read(song->reserved1fba, sizeof(song->reserved1fba), vio->user_data);
}

void write_bank0(const lsdj_song_t* song, lsdj_vio_t* vio)
{    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
            vio->write(song->phrases[i]->notes, PHRASE_LENGTH, vio->user_data);
        else
            vio->write(PHRASE_LENGTH_ZERO, PHRASE_LENGTH, vio->user_data);
    }
    
    vio->write(song->bookmarks, sizeof(song->bookmarks), vio->user_data);
    vio->write(song->reserved1030, sizeof(song->reserved1030), vio->user_data);
    vio->write(song->grooves, sizeof(song->grooves), vio->user_data);
    vio->write(song->rows, sizeof(song->rows), vio->user_data);
    
    unsigned char volumes[TABLE_LENGTH];
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (size_t j = 0; j < TABLE_LENGTH; ++j)
                volumes[j] = lsdj_table_get_volume(song->tables[i], j);
            vio->write(volumes, TABLE_LENGTH, vio->user_data);
        }
        else
            vio->write(TABLE_LENGTH_ZERO, TABLE_LENGTH, vio->user_data);
    }
    
    for (int i = 0; i < WORD_COUNT; ++i)
    {
        vio->write(song->words[i].allophones, WORD_LENGTH, vio->user_data);
        vio->write(song->words[i].lengths, WORD_LENGTH, vio->user_data);
    }
    
    vio->write(song->wordNames, sizeof(song->wordNames), vio->user_data);
    vio->write("rb", 2, vio->user_data);
    
    static char EMPTY_INSTRUMENT_NAME[5] = { 0, 0, 0, 0, 0 };
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (song->instruments[i])
            vio->write(song->instruments[i]->name, INSTRUMENT_NAME_LENGTH, vio->user_data);
        else
            vio->write(EMPTY_INSTRUMENT_NAME, INSTRUMENT_NAME_LENGTH, vio->user_data);
    }
    
    vio->write(song->reserved1fba, sizeof(song->reserved1fba), vio->user_data);
}

void read_soft_synth_parameters(lsdj_vio_t* vio, lsdj_synth_t* synth)
{
    vio->read(&synth->waveform, 1, vio->user_data);
    vio->read(&synth->filter, 1, vio->user_data);

    unsigned char resonance = 0;
    vio->read(&resonance, 1, vio->user_data);
    synth->resonanceStart = (resonance & 0xF0) >> 4;
    synth->resonanceEnd = resonance & 0x0F;

    vio->read(&synth->distortion, 1, vio->user_data);
    vio->read(&synth->phase, 1, vio->user_data);
    vio->read(&synth->volumeStart, 1, vio->user_data);
    vio->read(&synth->cutOffStart, 1, vio->user_data);
    vio->read(&synth->phaseStart, 1, vio->user_data);
    vio->read(&synth->vshiftStart, 1, vio->user_data);
    vio->read(&synth->volumeEnd, 1, vio->user_data);
    vio->read(&synth->cutOffEnd, 1, vio->user_data);
    vio->read(&synth->phaseEnd, 1, vio->user_data);
    vio->read(&synth->vshiftEnd, 1, vio->user_data);
    vio->read(synth->reserved, 3, vio->user_data);
}

void read_bank1(lsdj_vio_t* vio, lsdj_song_t* song, lsdj_error_t** error)
{
    vio->read(song->reserved2000, sizeof(song->reserved2000), vio->user_data);
    vio->seek(TABLE_ALLOC_TABLE_SIZE + INSTR_ALLOC_TABLE_SIZE, SEEK_CUR, vio->user_data); // table and instr alloc tables already read at beginning
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if (song->chains[i])
            vio->read(song->chains[i]->phrases, CHAIN_LENGTH, vio->user_data);
        else
            vio->seek(CHAIN_LENGTH, SEEK_CUR, vio->user_data);
    }
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if (song->chains[i])
            vio->read(song->chains[i]->transpositions, CHAIN_LENGTH, vio->user_data);
        else
            vio->seek(CHAIN_LENGTH, SEEK_CUR, vio->user_data);
    }
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (song->instruments[i])
            lsdj_read_instrument(vio, song->formatVersion, song->instruments[i], error);
        else
            vio->seek(16, SEEK_CUR, vio->user_data);
        
        if (error && *error)
            return;
    }
    
    unsigned char transpositions[TABLE_LENGTH];
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            vio->read(transpositions, TABLE_LENGTH, vio->user_data);
            lsdj_table_set_transpositions(song->tables[i], transpositions);
        }
        else
            vio->seek(TABLE_LENGTH, SEEK_CUR, vio->user_data);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (size_t j = 0; j < TABLE_LENGTH; ++j)
            {
                vio->read(&lsdj_table_get_command1(song->tables[i], j)->command, 1, vio->user_data);
            }
        } else {
            vio->seek(TABLE_LENGTH, SEEK_CUR, vio->user_data);
        }
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (size_t j = 0; j < TABLE_LENGTH; ++j)
                vio->read(&lsdj_table_get_command1(song->tables[i], j)->value, 1, vio->user_data);
        } else {
            vio->seek(TABLE_LENGTH, SEEK_CUR, vio->user_data);
        }
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (size_t j = 0; j < TABLE_LENGTH; ++j)
                vio->read(&lsdj_table_get_command2(song->tables[i], j)->command, 1, vio->user_data);
        } else {
            vio->seek(TABLE_LENGTH, SEEK_CUR, vio->user_data);
        }
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (size_t j = 0; j < TABLE_LENGTH; ++j)
                vio->read(&lsdj_table_get_command2(song->tables[i], j)->value, 1, vio->user_data);
        } else {
            vio->seek(TABLE_LENGTH, SEEK_CUR, vio->user_data);
        }
    }
    
    vio->seek(2, SEEK_CUR, vio->user_data); // "rb"
    vio->seek(PHRASE_ALLOC_TABLE_SIZE + CHAIN_ALLOC_TABLE_SIZE, SEEK_CUR, vio->user_data); // Already read at the beginning
    
    for (int i = 0; i < SYNTH_COUNT; ++i)
        read_soft_synth_parameters(vio, &song->synths[i]);
    
    vio->read(&song->meta.workTime, 2, vio->user_data);
    vio->read(&song->tempo, 1, vio->user_data);
    vio->read(&song->transposition, 1, vio->user_data);
    vio->read(&song->meta.totalTime, 3, vio->user_data);
    vio->read(&song->reserved3fb9, 1, vio->user_data); // time checksum, doesn't appear to be used anymore
    vio->read(&song->meta.keyDelay, 1, vio->user_data);
    vio->read(&song->meta.keyRepeat, 1, vio->user_data);
    vio->read(&song->meta.font, 1, vio->user_data);
    vio->read(&song->meta.sync, 1, vio->user_data);
    vio->read(&song->meta.colorSet, 1, vio->user_data);
    vio->read(&song->reserved3fbf, 1, vio->user_data);
    vio->read(&song->meta.clone, 1, vio->user_data);
    vio->read(&song->meta.fileChangedFlag, 1, vio->user_data);
    vio->read(&song->meta.powerSave, 1, vio->user_data);
    vio->read(&song->meta.preListen, 1, vio->user_data);
    
    unsigned char waveSynthOverwriteLocks[2];
    vio->read(waveSynthOverwriteLocks, 2, vio->user_data);
    for (int i = 0; i < SYNTH_COUNT; ++i)
        song->synths[i].overwritten = ((waveSynthOverwriteLocks[1 - (i / 8)] >> (i % 8)) & 1);
    
    vio->read(&song->reserved3fc6, sizeof(song->reserved3fc6), vio->user_data);
    vio->read(&song->drumMax, 1, vio->user_data);
    vio->read(&song->reserved3fd1, sizeof(song->reserved3fd1), vio->user_data);
}

void write_soft_synth_parameters(const lsdj_synth_t* synth, lsdj_vio_t* vio)
{
    vio->write(&synth->waveform, 1, vio->user_data);
    vio->write(&synth->filter, 1, vio->user_data);

    unsigned char resonance = (unsigned char)((synth->resonanceStart & 0x0F) << 4) | (synth->resonanceEnd & 0x0F);
    vio->write(&resonance, 1, vio->user_data);
    
    vio->write(&synth->distortion, 1, vio->user_data);
    vio->write(&synth->phase, 1, vio->user_data);
    vio->write(&synth->volumeStart, 1, vio->user_data);
    vio->write(&synth->cutOffStart, 1, vio->user_data);
    vio->write(&synth->phaseStart, 1, vio->user_data);
    vio->write(&synth->vshiftStart, 1, vio->user_data);
    vio->write(&synth->volumeEnd, 1, vio->user_data);
    vio->write(&synth->cutOffEnd, 1, vio->user_data);
    vio->write(&synth->phaseEnd, 1, vio->user_data);
    vio->write(&synth->vshiftEnd, 1, vio->user_data);
    vio->write(synth->reserved, 3, vio->user_data);
}

void write_bank1(const lsdj_song_t* song, lsdj_vio_t* vio, lsdj_error_t** error)
{
    unsigned char instrAllocTable[INSTR_ALLOC_TABLE_SIZE];
    memset(instrAllocTable, 0, INSTR_ALLOC_TABLE_SIZE);
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
        instrAllocTable[i] = (song->instruments[i] == NULL) ? 0 : 1;
    
    unsigned char tableAllocTable[TABLE_ALLOC_TABLE_SIZE];
    memset(tableAllocTable, 0, TABLE_ALLOC_TABLE_SIZE);
    for (int i = 0; i < TABLE_COUNT; ++i)
        tableAllocTable[i] = (song->tables[i] == NULL) ? 0 : 1;
    
    unsigned char chainAllocTable[CHAIN_ALLOC_TABLE_SIZE];
    memset(chainAllocTable, 0, CHAIN_ALLOC_TABLE_SIZE);
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if (song->chains[i])
            chainAllocTable[i / 8] |= (1 << (i % 8));
    }
    
    unsigned char phraseAllocTable[PHRASE_ALLOC_TABLE_SIZE];
    memset(phraseAllocTable, 0, PHRASE_ALLOC_TABLE_SIZE);
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
            phraseAllocTable[i / 8] |= (1 << (i % 8));
    }
    
    vio->write(song->reserved2000, sizeof(song->reserved2000), vio->user_data);
    vio->write(tableAllocTable, TABLE_ALLOC_TABLE_SIZE, vio->user_data);
    vio->write(instrAllocTable, INSTR_ALLOC_TABLE_SIZE, vio->user_data);
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if (song->chains[i])
            vio->write(song->chains[i]->phrases, CHAIN_LENGTH, vio->user_data);
        else
            vio->write(CHAIN_LENGTH_FF, CHAIN_LENGTH, vio->user_data);
    }
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if (song->chains[i])
            vio->write(song->chains[i]->transpositions, CHAIN_LENGTH, vio->user_data);
        else
            vio->write(CHAIN_LENGTH_ZERO, CHAIN_LENGTH, vio->user_data);
    }
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (song->instruments[i])
        {
            lsdj_write_instrument(song->instruments[i], song->formatVersion, vio, error);
        } else {
            vio->write(DEFAULT_INSTRUMENT, sizeof(DEFAULT_INSTRUMENT), vio->user_data);
        }
        
        if (error && *error)
            return;
    }
    
    unsigned char transpositions[TABLE_LENGTH];
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (size_t j = 0; j < TABLE_LENGTH; ++j)
                transpositions[j] = lsdj_table_get_transposition(song->tables[i], j);
            vio->write(&transpositions, TABLE_LENGTH, vio->user_data);
        }
        else
            vio->write(TABLE_LENGTH_ZERO, TABLE_LENGTH, vio->user_data);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (size_t j = 0; j < TABLE_LENGTH; ++j)
                vio->write(&lsdj_table_get_command1(song->tables[i], j)->command, 1, vio->user_data);
        } else {
            vio->write(TABLE_LENGTH_ZERO, TABLE_LENGTH, vio->user_data);
        }
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (size_t j = 0; j < TABLE_LENGTH; ++j)
                vio->write(&lsdj_table_get_command1(song->tables[i], j)->value, 1, vio->user_data);
        } else {
            vio->write(TABLE_LENGTH_ZERO, TABLE_LENGTH, vio->user_data);
        }
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (size_t j = 0; j < TABLE_LENGTH; ++j)
                vio->write(&lsdj_table_get_command2(song->tables[i], j)->command, 1, vio->user_data);
        } else {
            vio->write(TABLE_LENGTH_ZERO, TABLE_LENGTH, vio->user_data);
        }
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (size_t j = 0; j < TABLE_LENGTH; ++j)
                vio->write(&lsdj_table_get_command2(song->tables[i], j)->value, 1, vio->user_data);
        } else {
            vio->write(TABLE_LENGTH_ZERO, TABLE_LENGTH, vio->user_data);
        }
    }
    
    vio->write("rb", 2, vio->user_data);
    vio->write(phraseAllocTable, PHRASE_ALLOC_TABLE_SIZE, vio->user_data);
    vio->write(chainAllocTable, CHAIN_ALLOC_TABLE_SIZE, vio->user_data);
    
    for (int i = 0; i < SYNTH_COUNT; ++i)
        write_soft_synth_parameters(&song->synths[i], vio);
    
    vio->write(&song->meta.workTime, 2, vio->user_data);
    vio->write(&song->tempo, 1, vio->user_data);
    vio->write(&song->transposition, 1, vio->user_data);
    vio->write(&song->meta.totalTime, 3, vio->user_data);
    vio->write(&song->reserved3fb9, 1, vio->user_data); // checksum?
    vio->write(&song->meta.keyDelay, 1, vio->user_data);
    vio->write(&song->meta.keyRepeat, 1, vio->user_data);
    vio->write(&song->meta.font, 1, vio->user_data);
    vio->write(&song->meta.sync, 1, vio->user_data);
    vio->write(&song->meta.colorSet, 1, vio->user_data);
    vio->write(&song->reserved3fbf, 1, vio->user_data);
    vio->write(&song->meta.clone, 1, vio->user_data);
    vio->write(&song->meta.fileChangedFlag, 1, vio->user_data);
    vio->write(&song->meta.powerSave, 1, vio->user_data);
    vio->write(&song->meta.preListen, 1, vio->user_data);
    
    unsigned char waveSynthOverwriteLocks[2];
    memset(waveSynthOverwriteLocks, 0, sizeof(waveSynthOverwriteLocks));
    for (int i = 0; i < SYNTH_COUNT; ++i)
    {
        if (song->synths[i].overwritten)
            waveSynthOverwriteLocks[1 - (i / 8)] |= (1 << (i % 8));
    }
    vio->write(waveSynthOverwriteLocks, 2, vio->user_data);
    
    vio->write(&song->reserved3fc6, sizeof(song->reserved3fc6), vio->user_data);
    vio->write(&song->drumMax, 1, vio->user_data);
    vio->write(&song->reserved3fd1, sizeof(song->reserved3fd1), vio->user_data);
}

void read_bank2(lsdj_vio_t* vio, lsdj_song_t* song)
{
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
        {
            for (int j = 0; j < PHRASE_LENGTH; ++j)
                vio->read(&song->phrases[i]->commands[j].command, 1, vio->user_data);
        } else {
            vio->seek(PHRASE_LENGTH, SEEK_CUR, vio->user_data);
        }
    }
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
        {
            for (int j = 0; j < PHRASE_LENGTH; ++j)
                vio->read(&song->phrases[i]->commands[j].value, 1, vio->user_data);
        } else {
            vio->seek(PHRASE_LENGTH, SEEK_CUR, vio->user_data);
        }
    }
    
    vio->read(song->reserved5fe0, sizeof(song->reserved5fe0), vio->user_data);
}

void write_bank2(const lsdj_song_t* song, lsdj_vio_t* vio)
{
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
        {
            for (int j = 0; j < PHRASE_LENGTH; ++j)
                vio->write(&song->phrases[i]->commands[j].command, 1, vio->user_data);
        } else {
            vio->write(PHRASE_LENGTH_ZERO, PHRASE_LENGTH, vio->user_data);
        }
    }
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
        {
            for (int j = 0; j < PHRASE_LENGTH; ++j)
                vio->write(&song->phrases[i]->commands[j].value, 1, vio->user_data);
        } else {
            vio->write(PHRASE_LENGTH_ZERO, PHRASE_LENGTH, vio->user_data);
        }
    }
    
    vio->write(song->reserved5fe0, sizeof(song->reserved5fe0), vio->user_data);
}

void read_bank3(lsdj_vio_t* vio, lsdj_song_t* song)
{
    for (int i = 0; i < WAVE_COUNT; ++i)
        vio->read(song->waves[i].data, WAVE_LENGTH, vio->user_data);
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
            vio->read(&song->phrases[i]->instruments, PHRASE_LENGTH, vio->user_data);
        else
            vio->seek(PHRASE_LENGTH, SEEK_CUR, vio->user_data);
    }
    
    vio->seek(2, SEEK_CUR, vio->user_data); // "rb"
    
    vio->read(song->reserved7ff2, sizeof(song->reserved7ff2), vio->user_data);
    
    vio->seek(1, SEEK_CUR, vio->user_data); // Already read the version number
}

void write_bank3(const lsdj_song_t* song, lsdj_vio_t* vio)
{
    for (int i = 0; i < WAVE_COUNT; ++i)
        vio->write(song->waves[i].data, WAVE_LENGTH, vio->user_data);
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
            vio->write(&song->phrases[i]->instruments, PHRASE_LENGTH, vio->user_data);
        else
            vio->write(PHRASE_LENGTH_FF, PHRASE_LENGTH, vio->user_data);
    }
    
    vio->write("rb", 2, vio->user_data);
    
    vio->write(song->reserved7ff2, sizeof(song->reserved7ff2), vio->user_data);
    vio->write(&song->formatVersion, 1, vio->user_data);
}

int check_rb(lsdj_vio_t* vio, long position)
{
    char data[2];
    
    vio->seek(position, SEEK_SET, vio->user_data);
    vio->read(data, 2, vio->user_data);
    
    return (data[0] == 'r' && data[1] == 'b') ? 0 : 1;
}

lsdj_song_t* lsdj_read_song(lsdj_vio_t* vio, lsdj_error_t** error)
{
    // Check for incorrect input
    if (vio->read == NULL)
    {
        lsdj_create_error(error, "read is NULL");
        return NULL;
    }
    
    if (vio->tell == NULL)
    {
        lsdj_create_error(error, "tell is NULL");
        return NULL;
    }
    
    if (vio->seek == NULL)
    {
        lsdj_create_error(error, "seek is NULL");
        return NULL;
    }
    
    const long begin = vio->tell(vio->user_data);
    
    // Check if the 'rb' flags have been set correctly
    if (check_rb(vio, begin + 0x1E78) != 0) { lsdj_create_error(error, "memory flag 'rb' not found at 0x1E78"); return NULL; }
    if (check_rb(vio, begin + 0x3E80) != 0) { lsdj_create_error(error, "memory flag 'rb' not found at 0x3E80"); return NULL; }
    if (check_rb(vio, begin + 0x7FF0) != 0) { lsdj_create_error(error, "memory flag 'rb' not found at 0x7FF0"); return NULL; }
    
    // We passed the 'rb' check, and can create a song now for reading
    lsdj_song_t* song = alloc_song(error);
    if (error && *error)
        return NULL;
    
    // Read the version number
    vio->seek(begin + 0x7FFF, SEEK_SET, vio->user_data);
    vio->read(&song->formatVersion, 1, vio->user_data);
    
    // Read the allocation tables
    unsigned char instrAllocTable[INSTR_ALLOC_TABLE_SIZE];
    unsigned char tableAllocTable[TABLE_ALLOC_TABLE_SIZE];
    unsigned char chainAllocTable[CHAIN_ALLOC_TABLE_SIZE];
    unsigned char phraseAllocTable[PHRASE_ALLOC_TABLE_SIZE];
    
    vio->seek(begin + 0x2020, SEEK_SET, vio->user_data);
    vio->read(tableAllocTable, TABLE_ALLOC_TABLE_SIZE, vio->user_data);
    vio->read(instrAllocTable, INSTR_ALLOC_TABLE_SIZE, vio->user_data);
    
    vio->seek(begin + 0x3E82, SEEK_SET, vio->user_data);
    vio->read(phraseAllocTable, PHRASE_ALLOC_TABLE_SIZE, vio->user_data);
    vio->read(chainAllocTable, CHAIN_ALLOC_TABLE_SIZE, vio->user_data);

    for (int i = 0; i < TABLE_ALLOC_TABLE_SIZE; ++i)
    {
        if (tableAllocTable[i])
            song->tables[i] = lsdj_table_new();
    }
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (instrAllocTable[i])
            song->instruments[i] = (lsdj_instrument_t*)malloc(sizeof(lsdj_instrument_t));
    }
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if ((chainAllocTable[i / 8] >> (i % 8)) & 1)
            song->chains[i] = (lsdj_chain_t*)malloc(sizeof(lsdj_chain_t));
    }
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if ((phraseAllocTable[i / 8] >> (i % 8)) & 1)
            song->phrases[i] = (lsdj_phrase_t*)malloc(sizeof(lsdj_phrase_t));
    }
    
    // Read the banks
    vio->seek(begin, SEEK_SET, vio->user_data);
    read_bank0(vio, song);
    read_bank1(vio, song, error);
    if (error && *error)
    {
        lsdj_free_song(song);
        return NULL;
    }
    
    read_bank2(vio, song);
    read_bank3(vio, song);
    
    return song;
}

lsdj_song_t* lsdj_read_song_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error)
{
    if (data == NULL)
    {
        lsdj_create_error(error, "data is NULL");
        return NULL;
    }
    
    lsdj_memory_data_t mem;
    mem.cur = mem.begin = (unsigned char*)data;
    mem.size = size;
    
    lsdj_vio_t vio;
    vio.read = lsdj_mread;
    vio.seek = lsdj_mseek;
    vio.tell = lsdj_mtell;
    vio.user_data = &mem;
    
    return lsdj_read_song(&vio, error);
}

void lsdj_write_song(const lsdj_song_t* song, lsdj_vio_t* vio, lsdj_error_t** error)
{
    write_bank0(song, vio);
    write_bank1(song, vio, error);
    write_bank2(song, vio);
    write_bank3(song, vio);
}

void lsdj_write_song_to_memory(const lsdj_song_t* song, unsigned char* data, size_t size, lsdj_error_t** error)
{
    if (song == NULL)
        return lsdj_create_error(error, "song is NULL");
    
    if (data == NULL)
        return lsdj_create_error(error, "data is NULL");
    
    if (size < SONG_DECOMPRESSED_SIZE)
        return lsdj_create_error(error, "memory is not big enough to store song");
    
    lsdj_memory_data_t mem;
    mem.begin = data;
    mem.cur = mem.begin;
    mem.size = size;
    
    lsdj_vio_t vio;
    vio.write = lsdj_mwrite;
    vio.seek = lsdj_mseek;
    vio.tell = lsdj_mtell;
    vio.user_data = &mem;
    
    lsdj_write_song(song, &vio, error);
}

void lsdj_song_set_format_version(lsdj_song_t* song, unsigned char version)
{
    song->formatVersion = version;
}

unsigned char lsdj_song_get_format_version(const lsdj_song_t* song)
{
    return song->formatVersion;
}

void lsdj_song_set_tempo(lsdj_song_t* song, unsigned char tempo)
{
    song->tempo = tempo;
}

unsigned char lsdj_song_get_tempo(const lsdj_song_t* song)
{
    return song->tempo;
}

void lsdj_song_set_transposition(lsdj_song_t* song, unsigned char transposition)
{
    song->transposition = transposition;
}

unsigned char lsdj_song_get_transposition(const lsdj_song_t* song)
{
    return song->transposition;
}

unsigned char lsdj_song_get_file_changed_flag(const lsdj_song_t* song)
{
    return song->meta.fileChangedFlag;
}

void lsdj_song_set_drum_max(lsdj_song_t* song, unsigned char drumMax)
{
    song->drumMax = drumMax;
}

unsigned char lsdj_song_get_drum_max(const lsdj_song_t* song)
{
    return song->drumMax;
}

lsdj_table_t* lsdj_song_get_table(lsdj_song_t* song, size_t index)
{
    return song->tables[index];
}
