#include <stdlib.h>
#include <string.h>

#include "song.h"

static unsigned char DEFAULT_WORD_NAMES[168] =
{
    'C',' ','2','C','#','2','D',' ','2','D','#','2','E',' ','2','F',' ','2','F','#','2','G',' ','2','G','#','2','A',' ','2','A','#','2','B',' ','2','B','#','2','C',' ','3','C','#','3','D',' ','3','D','#','3','E',' ','3','F',' ','3','F','#','3','G',' ','3','G','#','3','A',' ','3','A','#','3','B',' ','3','B','#','3','C',' ','4','C','#','4','D',' ','4','D','#','4','E',' ','4','F',' ','4','F','#','4','G',' ','4','G','#','4','A',' ','4','A','#','4','B',' ','4','B','#','4','C',' ','5','C','#','5','D',' ','5','D','#','5','E',' ','5','F',' ','5'
};

static unsigned char DEFAULT_SOFT_SYNTH[16] = { 0, 0, 0, 0, 0, 0x10, 0xFF, 0, 0, 0x10, 0xFF, 0, 0, 0, 0,  };
static const unsigned char DEFAULT_WAVE[WAVE_LENGTH] = { 0x8E, 0xCD, 0xCC, 0xBB, 0xAA, 0xA9, 0x99, 0x88, 0x87, 0x76, 0x66, 0x55, 0x54, 0x43, 0x32, 0x31 };
static const unsigned char DEFAULT_INSTRUMENT[16] = { 0, 0xA8, 0, 0, 0xFF, 0, 0, 3, 0, 0, 0xD0, 0, 0, 0, 0xF3, 0 };

static const int INSTR_ALLOC_TABLE_SIZE = 64;
static const int TABLE_ALLOC_TABLE_SIZE = 32;
static const int CHAIN_ALLOC_TABLE_SIZE = 16;
static const int PHRASE_ALLOC_TABLE_SIZE = 32;

static unsigned char TABLE_LENGTH_ZERO[TABLE_LENGTH] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char CHAIN_LENGTH_ZERO[TABLE_LENGTH] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char CHAIN_LENGTH_FF[TABLE_LENGTH] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static unsigned char PHRASE_LENGTH_ZERO[PHRASE_LENGTH] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char PHRASE_LENGTH_FF[PHRASE_LENGTH] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void read_bank0(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song)
{
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
            read(song->phrases[i]->notes, PHRASE_LENGTH, user_data);
        else
            seek(PHRASE_LENGTH, SEEK_CUR, user_data);
    }

    read(song->bookmarks, sizeof(song->bookmarks), user_data);
    read(song->reserved1030, sizeof(song->reserved1030), user_data);
    read(song->grooves, sizeof(song->grooves), user_data);
    read(song->rows, sizeof(song->rows), user_data);
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
            read(song->tables[i]->volumes, TABLE_LENGTH, user_data);
        else
            seek(TABLE_LENGTH, SEEK_CUR, user_data);
    }
    
    read(song->instrumentSpeechWords, sizeof(song->instrumentSpeechWords), user_data);
    read(song->instrumentSpeechWordNames, sizeof(song->instrumentSpeechWordNames), user_data);
    seek(2, SEEK_CUR, user_data); // rb
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (song->instruments[i])
            read(song->instruments[i]->name, INSTRUMENT_NAME_LENGTH, user_data);
        else
            seek(INSTRUMENT_NAME_LENGTH, SEEK_CUR, user_data);
    }

    read(song->reserved1fba, sizeof(song->reserved1fba), user_data);
}

void write_bank0(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data)
{    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
            write(song->phrases[i]->notes, PHRASE_LENGTH, user_data);
        else
            write(PHRASE_LENGTH_ZERO, PHRASE_LENGTH, user_data);
    }
    
    write(song->bookmarks, sizeof(song->bookmarks), user_data);
    write(song->reserved1030, sizeof(song->reserved1030), user_data);
    write(song->grooves, sizeof(song->grooves), user_data);
    write(song->rows, sizeof(song->rows), user_data);
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
            write(song->tables[i]->volumes, TABLE_LENGTH, user_data);
        else
            write(TABLE_LENGTH_ZERO, TABLE_LENGTH, user_data);
    }
    
    write(song->instrumentSpeechWords, sizeof(song->instrumentSpeechWords), user_data);
    write(song->instrumentSpeechWordNames, sizeof(song->instrumentSpeechWordNames), user_data);
    write("rb", 2, user_data);
    
    static char EMPTY_INSTRUMENT_NAME[5] = { 0, 0, 0, 0, 0 };
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (song->instruments[i])
            write(song->instruments[i]->name, INSTRUMENT_NAME_LENGTH, user_data);
        else
            write(EMPTY_INSTRUMENT_NAME, INSTRUMENT_NAME_LENGTH, user_data);
    }
    
    write(song->reserved1fba, sizeof(song->reserved1fba), user_data);
}

void read_bank1(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song, lsdj_error_t** error)
{
    read(song->reserved2000, sizeof(song->reserved2000), user_data);
    seek(TABLE_ALLOC_TABLE_SIZE + INSTR_ALLOC_TABLE_SIZE, SEEK_CUR, user_data); // table and instr alloc tables already read at beginning
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if (song->chains[i])
            read(song->chains[i]->phraseNumbers, CHAIN_LENGTH, user_data);
        else
            seek(CHAIN_LENGTH, SEEK_CUR, user_data);
    }
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if (song->chains[i])
            read(song->chains[i]->transposes, CHAIN_LENGTH, user_data);
        else
            seek(CHAIN_LENGTH, SEEK_CUR, user_data);
    }
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (song->instruments[i])
            read(song->instruments[i]->parameters, 16, user_data);
        else
            seek(16, SEEK_CUR, user_data);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
            read(song->tables[i]->transposes, TABLE_LENGTH, user_data);
        else
            seek(TABLE_LENGTH, SEEK_CUR, user_data);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (int j = 0; j < TABLE_LENGTH; ++j)
                read(&song->tables[i]->commands1[j].command, 1, user_data);
        } else {
            seek(TABLE_LENGTH, SEEK_CUR, user_data);
        }
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (int j = 0; j < TABLE_LENGTH; ++j)
                read(&song->tables[i]->commands1[j].value, 1, user_data);
        } else {
            seek(TABLE_LENGTH, SEEK_CUR, user_data);
        }
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (int j = 0; j < TABLE_LENGTH; ++j)
                read(&song->tables[i]->commands2[j].command, 1, user_data);
        } else {
            seek(TABLE_LENGTH, SEEK_CUR, user_data);
        }
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (int j = 0; j < TABLE_LENGTH; ++j)
                read(&song->tables[i]->commands2[j].value, 1, user_data);
        } else {
            seek(TABLE_LENGTH, SEEK_CUR, user_data);
        }
    }
    
    seek(2, SEEK_CUR, user_data); // "rb"
    seek(PHRASE_ALLOC_TABLE_SIZE + CHAIN_ALLOC_TABLE_SIZE, SEEK_CUR, user_data); // Already read at the beginning
    
    read(song->softSynthParams, sizeof(song->softSynthParams), user_data);
    read(&song->workTime, 2, user_data);
    read(&song->tempo, 1, user_data);
    read(&song->tuneSetting, 1, user_data);
    read(&song->totalTime, 3, user_data);
    read(&song->reserved3fb9, 1, user_data); // time checksum, doesn't appear to be used anymore
    read(&song->keyDelay, 1, user_data);
    read(&song->keyRepeat, 1, user_data);
    read(&song->font, 1, user_data);
    read(&song->syncSetting, 1, user_data);
    read(&song->colorSet, 1, user_data);
    read(&song->reserved3fbf, 1, user_data);
    read(&song->clone, 1, user_data);
    read(&song->fileChangedFlag, 1, user_data);
    read(&song->powerSave, 1, user_data);
    read(&song->preListen, 1, user_data);
    read(song->waveSynthOverwriteLocks, 2, user_data);
    read(&song->reserved3fc6, sizeof(song->reserved3fc6), user_data);
}

void write_bank1(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data)
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
    
    write(song->reserved2000, sizeof(song->reserved2000), user_data);
    write(tableAllocTable, TABLE_ALLOC_TABLE_SIZE, user_data);
    write(instrAllocTable, INSTR_ALLOC_TABLE_SIZE, user_data);
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if (song->chains[i])
            write(song->chains[i]->phraseNumbers, CHAIN_LENGTH, user_data);
        else
            write(CHAIN_LENGTH_FF, CHAIN_LENGTH, user_data);
    }
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if (song->chains[i])
            write(song->chains[i]->transposes, CHAIN_LENGTH, user_data);
        else
            write(CHAIN_LENGTH_ZERO, CHAIN_LENGTH, user_data);
    }
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (song->instruments[i])
            write(song->instruments[i]->parameters, 16, user_data);
        else
            write(DEFAULT_INSTRUMENT, sizeof(DEFAULT_INSTRUMENT), user_data);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
            write(song->tables[i]->transposes, TABLE_LENGTH, user_data);
        else
            write(TABLE_LENGTH_ZERO, TABLE_LENGTH, user_data);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (int j = 0; j < TABLE_LENGTH; ++j)
                write(&song->tables[i]->commands1[j].command, 1, user_data);
        } else {
            write(TABLE_LENGTH_ZERO, TABLE_LENGTH, user_data);
        }
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (int j = 0; j < TABLE_LENGTH; ++j)
                write(&song->tables[i]->commands1[j].value, 1, user_data);
        } else {
            write(TABLE_LENGTH_ZERO, TABLE_LENGTH, user_data);
        }
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (int j = 0; j < TABLE_LENGTH; ++j)
                write(&song->tables[i]->commands2[j].command, 1, user_data);
        } else {
            write(TABLE_LENGTH_ZERO, TABLE_LENGTH, user_data);
        }
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            for (int j = 0; j < TABLE_LENGTH; ++j)
                write(&song->tables[i]->commands2[j].value, 1, user_data);
        } else {
            write(TABLE_LENGTH_ZERO, TABLE_LENGTH, user_data);
        }
    }
    
    write("rb", 2, user_data);
    write(phraseAllocTable, PHRASE_ALLOC_TABLE_SIZE, user_data);
    write(chainAllocTable, CHAIN_ALLOC_TABLE_SIZE, user_data);
    write(song->softSynthParams, sizeof(song->softSynthParams), user_data);
    
    write(&song->workTime, 2, user_data);
    write(&song->tempo, 1, user_data);
    write(&song->tuneSetting, 1, user_data);
    write(&song->totalTime, 3, user_data);
    write(&song->reserved3fb9, 1, user_data); // checksum?
    write(&song->keyDelay, 1, user_data);
    write(&song->keyRepeat, 1, user_data);
    write(&song->font, 1, user_data);
    write(&song->syncSetting, 1, user_data);
    write(&song->colorSet, 1, user_data);
    write(&song->reserved3fbf, 1, user_data);
    write(&song->clone, 1, user_data);
    write(&song->fileChangedFlag, 1, user_data);
    write(&song->powerSave, 1, user_data);
    write(&song->preListen, 1, user_data);
    write(song->waveSynthOverwriteLocks, 2, user_data);
    write(&song->reserved3fc6, sizeof(song->reserved3fc6), user_data);
}

void read_bank2(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song)
{
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
        {
            for (int j = 0; j < PHRASE_LENGTH; ++j)
                read(&song->phrases[i]->commands[j].command, 1, user_data);
        } else {
            seek(PHRASE_LENGTH, SEEK_CUR, user_data);
        }
    }
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
        {
            for (int j = 0; j < PHRASE_LENGTH; ++j)
                read(&song->phrases[i]->commands[j].value, 1, user_data);
        } else {
            seek(PHRASE_LENGTH, SEEK_CUR, user_data);
        }
    }
    
    read(song->reserved5fe0, sizeof(song->reserved5fe0), user_data);
}

void write_bank2(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data)
{
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
        {
            for (int j = 0; j < PHRASE_LENGTH; ++j)
                write(&song->phrases[i]->commands[j].command, 1, user_data);
        } else {
            write(PHRASE_LENGTH_ZERO, PHRASE_LENGTH, user_data);
        }
    }
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
        {
            for (int j = 0; j < PHRASE_LENGTH; ++j)
                write(&song->phrases[i]->commands[j].value, 1, user_data);
        } else {
            write(PHRASE_LENGTH_ZERO, PHRASE_LENGTH, user_data);
        }
    }
    
    write(song->reserved5fe0, sizeof(song->reserved5fe0), user_data);
}

void read_bank3(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song)
{
    for (int i = 0; i < WAVE_COUNT; ++i)
        read(song->waves[i], WAVE_LENGTH, user_data);
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
            read(&song->phrases[i]->instruments, PHRASE_LENGTH, user_data);
        else
            seek(PHRASE_LENGTH, SEEK_CUR, user_data);
    }
    
    seek(2, SEEK_CUR, user_data); // "rb"
    
    read(song->reserved7ff2, sizeof(song->reserved7ff2), user_data);
    read(&song->version, 1, user_data);
}

void write_bank3(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data)
{
    for (int i = 0; i < WAVE_COUNT; ++i)
        write(song->waves[i], WAVE_LENGTH, user_data);
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->phrases[i])
            write(&song->phrases[i]->instruments, PHRASE_LENGTH, user_data);
        else
            write(PHRASE_LENGTH_FF, PHRASE_LENGTH, user_data);
    }
    
    write("rb", 2, user_data);
    
    write(song->reserved7ff2, sizeof(song->reserved7ff2), user_data);
    write(&song->version, 1, user_data);
}

int check_rb(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, long position)
{
    char data[2];
    
    seek(position, SEEK_SET, user_data);
    read(data, 2, user_data);
    
    return (data[0] == 'r' && data[1] == 'b') ? 0 : 1;
}

void lsdj_read_song(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song, lsdj_error_t** error)
{
    // Check for incorrect input
    if (read == NULL)
        return lsdj_create_error(error, "read is NULL");
    
    if (tell == NULL)
        return lsdj_create_error(error, "tell is NULL");
    
    if (seek == NULL)
        return lsdj_create_error(error, "seek is NULL");
    
    if (song == NULL)
        return lsdj_create_error(error, "song is NULL");
    
    const long begin = tell(user_data);
    
    // Check if the 'rb' flags have been set correctly
    if (check_rb(read, seek, user_data, 0x1E78) != 0) return lsdj_create_error(error, "memory flag 'rb' not found at 0x1E78");
    if (check_rb(read, seek, user_data, 0x3E80) != 0) return lsdj_create_error(error, "memory flag 'rb' not found at 0x3E80");
    if (check_rb(read, seek, user_data, 0x7FF0) != 0) return lsdj_create_error(error, "memory flag 'rb' not found at 0x7FF0");
    
    // Read the allocation tables
    unsigned char instrAllocTable[INSTR_ALLOC_TABLE_SIZE];
    unsigned char tableAllocTable[TABLE_ALLOC_TABLE_SIZE];
    unsigned char chainAllocTable[CHAIN_ALLOC_TABLE_SIZE];
    unsigned char phraseAllocTable[PHRASE_ALLOC_TABLE_SIZE];
    
    seek(begin + 0x2020, SEEK_SET, user_data);
    read(tableAllocTable, TABLE_ALLOC_TABLE_SIZE, user_data);
    read(instrAllocTable, INSTR_ALLOC_TABLE_SIZE, user_data);
    
    seek(begin + 0x3E82, SEEK_SET, user_data);
    read(phraseAllocTable, PHRASE_ALLOC_TABLE_SIZE, user_data);
    read(chainAllocTable, CHAIN_ALLOC_TABLE_SIZE, user_data);
    
    for (int i = 0; i < TABLE_ALLOC_TABLE_SIZE; ++i)
    {
        if (tableAllocTable[i])
            song->tables[i] = malloc(sizeof(lsdj_table_t));
    }
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (instrAllocTable[i])
            song->instruments[i] = malloc(sizeof(lsdj_instrument_t));
    }
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if ((chainAllocTable[i / 8] >> (i % 8)) & 1)
            song->chains[i] = malloc(sizeof(lsdj_chain_t));
    }
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if ((phraseAllocTable[i / 8] >> (i % 8)) & 1)
            song->phrases[i] = malloc(sizeof(lsdj_phrase_t));
    }
    
    // Read the banks
    seek(begin, SEEK_SET, user_data);
    read_bank0(read, seek, user_data, song);
    read_bank1(read, seek, user_data, song, error);
    read_bank2(read, seek, user_data, song);
    read_bank3(read, seek, user_data, song);
}

void lsdj_read_song_from_memory(const unsigned char* data, size_t size, lsdj_song_t* song, lsdj_error_t** error)
{
    if (data == NULL)
        return lsdj_create_error(error, "data is NULL");
    
    if (song == NULL)
        return lsdj_create_error(error, "song is NULL");
    
    lsdj_memory_data_t mem;
    mem.begin = (unsigned char*)data;
    mem.cur = mem.begin;
    mem.size = size;
    
    lsdj_read_song(lsdj_mread, lsdj_mtell, lsdj_mseek, &mem, song, error);
}

void lsdj_write_song(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data, lsdj_error_t** error)
{
    write_bank0(song, write, user_data);
    write_bank1(song, write, user_data);
    write_bank2(song, write, user_data);
    write_bank3(song, write, user_data);
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
    
    lsdj_write_song(song, lsdj_mwrite, &mem, error);
}

void lsdj_clear_song(lsdj_song_t* song)
{
    song->version = 3;
    
    for (int i = 0; i < ROW_COUNT; ++i)
        lsdj_clear_row(&song->rows[i]);
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
    {
        if (song->instruments[i])
        {
            free(song->chains[i]);
            song->chains[i] = NULL;
        }
    }
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        if (song->instruments[i])
        {
            free(song->phrases[i]);
            song->phrases[i] = NULL;
        }
    }
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
    {
        if (song->instruments[i])
        {
            free(song->instruments[i]);
            song->instruments[i] = NULL;
        }
    }
    
    for (int i = 0; i < WAVE_COUNT; ++i)
        memcpy(song->waves[i], DEFAULT_WAVE, WAVE_LENGTH);
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        if (song->tables[i])
        {
            free(song->tables[i]);
            song->tables[i] = NULL;
        }
    }
    
    for (int i = 0; i < GROOVE_COUNT; ++i)
    {
        song->grooves[i][0] = 0x06;
        song->grooves[i][1] = 0x06;
        memset(&song->grooves[2], 0, GROOVE_LENGTH - 2);
    }
    
    memset(song->bookmarks, 0xFF, sizeof(BOOKMARK_COUNT));
    memset(song->instrumentSpeechWords, 0, sizeof(song->instrumentSpeechWords));
    memcpy(song->instrumentSpeechWordNames, DEFAULT_WORD_NAMES, sizeof(song->instrumentSpeechWordNames));
    
    for (int i = 0; i < SYNTH_COUNT; ++i)
        memcpy(song->softSynthParams[i], DEFAULT_SOFT_SYNTH, 16);
    
    song->workTime.hours = 0;
    song->workTime.minutes = 0;
    song->tempo = 120;
    song->tuneSetting = 0;
    song->totalTime.days = 0;
    song->totalTime.hours = 0;
    song->totalTime.minutes = 0;
    song->keyDelay = 7;
    song->keyRepeat = 2;
    song->font = 0;
    song->syncSetting = 0;
    song->colorSet = 0;
    song->clone = 0;
    song->fileChangedFlag = 0;
    song->powerSave = 0;
    song->preListen = 1;
    song->waveSynthOverwriteLocks[0] = song->waveSynthOverwriteLocks[1] = 0;
}
