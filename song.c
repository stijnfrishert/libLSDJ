#include <stdlib.h>
#include <string.h>

#include "song.h"

static unsigned char DEFAULT_WORD_NAMES[168] =
{
    'C',' ','2','C','#','2','D',' ','2','D','#','2','E',' ','2','F',' ','2','F','#','2','G',' ','2','G','#','2','A',' ','2','A','#','2','B',' ','2','B','#','2','C',' ','3','C','#','3','D',' ','3','D','#','3','E',' ','3','F',' ','3','F','#','3','G',' ','3','G','#','3','A',' ','3','A','#','3','B',' ','3','B','#','3','C',' ','4','C','#','4','D',' ','4','D','#','4','E',' ','4','F',' ','4','F','#','4','G',' ','4','G','#','4','A',' ','4','A','#','4','B',' ','4','B','#','4','C',' ','5','C','#','5','D',' ','5','D','#','5','E',' ','5','F',' ','5'
};

static unsigned char DEFAULT_SOFT_SYNTH[16] = { 0, 0, 0, 0, 0, 0x10, 0xff, 0, 0, 0x10, 0xff, 0, 0, 0, 0,  };
static const unsigned char DEFAULT_WAVE[WAVE_LENGTH] = { 0x8E, 0xCD, 0xCC, 0xBB, 0xAA, 0xA9, 0x99, 0x88, 0x87, 0x76, 0x66, 0x55, 0x54, 0x43, 0x32, 0x31 };

void read_bank0(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song)
{
    for (int i = 0; i < PHRASE_COUNT; ++i)
        read(song->phrases[i].notes, PHRASE_LENGTH, user_data);

    read(song->bookmarks, sizeof(song->bookmarks), user_data);
    read(song->empty1030, sizeof(song->empty1030), user_data); // empty space
    read(song->grooves, sizeof(song->grooves), user_data);
    read(song->rows, sizeof(song->rows), user_data);
    
    for (int i = 0; i < TABLE_COUNT; ++i)
        read(song->tables[i].volumes, TABLE_LENGTH, user_data);
    
    read(song->instrumentSpeechWords, sizeof(song->instrumentSpeechWords), user_data);
    read(song->instrumentSpeechWordNames, sizeof(song->instrumentSpeechWordNames), user_data);
    seek(2, SEEK_CUR, user_data); // rb
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
        read(song->instruments[i].name, sizeof(song->instruments[i].name), user_data);

    read(song->empty1fba, sizeof(song->empty1fba), user_data); // empty space
}

void write_bank0(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data)
{    
    for (int i = 0; i < PHRASE_COUNT; ++i)
        write(song->phrases[i].notes, PHRASE_LENGTH, user_data);
    
    write(song->bookmarks, sizeof(song->bookmarks), user_data);
    write(song->empty1030, sizeof(song->empty1030), user_data); // empty space
    write(song->grooves, sizeof(song->grooves), user_data);
    write(song->rows, sizeof(song->rows), user_data);
    
    for (int i = 0; i < TABLE_COUNT; ++i)
        write(song->tables[i].volumes, TABLE_LENGTH, user_data);
    
    write(song->instrumentSpeechWords, sizeof(song->instrumentSpeechWords), user_data);
    write(song->instrumentSpeechWordNames, sizeof(song->instrumentSpeechWordNames), user_data);
    write("rb", 2, user_data);
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
        write(song->instruments[i].name, sizeof(song->instruments[i].name), user_data);
    
    write(song->empty1fba, sizeof(song->empty1fba), user_data); // empty space
}

void read_bank1(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song, lsdj_error_t** error)
{
    read(song->empty2000, sizeof(song->empty2000), user_data);
    read(song->tableAllocTable, sizeof(song->tableAllocTable), user_data);
    read(song->instrAllocTable, sizeof(song->instrAllocTable), user_data);
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
        read(song->chains[i].phraseNumbers, CHAIN_LENGTH, user_data);
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
        read(song->chains[i].transposes, CHAIN_LENGTH, user_data);
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
        read(song->instruments[i].parameters, 16, user_data);
    
    for (int i = 0; i < TABLE_COUNT; ++i)
        read(song->tables[i].transposes, TABLE_LENGTH, user_data);
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        for (int j = 0; j < TABLE_LENGTH; ++j)
            read(&song->tables[i].commands1[j].command, 1, user_data);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        for (int j = 0; j < TABLE_LENGTH; ++j)
            read(&song->tables[i].commands1[j].value, 1, user_data);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        for (int j = 0; j < TABLE_LENGTH; ++j)
            read(&song->tables[i].commands2[j].command, 1, user_data);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        for (int j = 0; j < TABLE_LENGTH; ++j)
            read(&song->tables[i].commands2[j].value, 1, user_data);
    }
    
    seek(2, SEEK_CUR, user_data); // rb
    read(song->phraseAllocTable, sizeof(song->phraseAllocTable), user_data);
    read(song->chainAllocTable, sizeof(song->chainAllocTable), user_data);
    read(song->softSynthParams, sizeof(song->softSynthParams), user_data);
    
    read(&song->workTime, 2, user_data);
    read(&song->tempo, 1, user_data);
    read(&song->tuneSetting, 1, user_data);
    read(&song->totalTime, 3, user_data);
    seek(1, SEEK_CUR, user_data); // time checksum, doesn't appear to be used anymore
    read(&song->keyDelay, 1, user_data);
    read(&song->keyRepeat, 1, user_data);
    read(&song->font, 1, user_data);
    read(&song->syncSetting, 1, user_data);
    read(&song->colorSet, 1, user_data);
    read(&song->empty3fbf, 1, user_data);
    read(&song->clone, 1, user_data);
    read(&song->fileChangedFlag, 1, user_data);
    read(&song->powerSave, 1, user_data);
    read(&song->preListen, 1, user_data);
    read(song->waveSynthOverwriteLocks, 2, user_data);
    read(&song->empty3fc6, sizeof(song->empty3fc6), user_data);
}

void write_bank1(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data)
{
    write(song->empty2000, sizeof(song->empty2000), user_data);
    write(song->tableAllocTable, sizeof(song->tableAllocTable), user_data);
    write(song->instrAllocTable, sizeof(song->instrAllocTable), user_data);
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
        write(song->chains[i].phraseNumbers, CHAIN_LENGTH, user_data);
    
    for (int i = 0; i < CHAIN_COUNT; ++i)
        write(song->chains[i].transposes, CHAIN_LENGTH, user_data);
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
        write(song->instruments[i].parameters, 16, user_data);
    
    for (int i = 0; i < TABLE_COUNT; ++i)
        write(song->tables[i].transposes, TABLE_LENGTH, user_data);
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        for (int j = 0; j < TABLE_LENGTH; ++j)
            write(&song->tables[i].commands1[j].command, 1, user_data);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        for (int j = 0; j < TABLE_LENGTH; ++j)
            write(&song->tables[i].commands1[j].value, 1, user_data);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        for (int j = 0; j < TABLE_LENGTH; ++j)
            write(&song->tables[i].commands2[j].command, 1, user_data);
    }
    
    for (int i = 0; i < TABLE_COUNT; ++i)
    {
        for (int j = 0; j < TABLE_LENGTH; ++j)
            write(&song->tables[i].commands2[j].value, 1, user_data);
    }
    
    write("rb", 2, user_data);
    write(song->phraseAllocTable, sizeof(song->phraseAllocTable), user_data);
    write(song->chainAllocTable, sizeof(song->chainAllocTable), user_data);
    write(song->softSynthParams, sizeof(song->softSynthParams), user_data);
    
    write(&song->workTime, 2, user_data);
    write(&song->tempo, 1, user_data);
    write(&song->tuneSetting, 1, user_data);
    write(&song->totalTime, 3, user_data);
    write(&song->keyDelay, 1, user_data);
    write(&song->keyRepeat, 1, user_data);
    write(&song->font, 1, user_data);
    write(&song->syncSetting, 1, user_data);
    write(&song->colorSet, 1, user_data);
    write(&song->empty3fbf, 1, user_data);
    write(&song->clone, 1, user_data);
    write(&song->fileChangedFlag, 1, user_data);
    write(&song->powerSave, 1, user_data);
    write(&song->preListen, 1, user_data);
    write(song->waveSynthOverwriteLocks, 2, user_data);
    write(&song->empty3fc6, sizeof(song->empty3fc6), user_data);
}

void read_bank2(lsdj_vio_read_t read, void* user_data, lsdj_song_t* song)
{
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        for (int j = 0; j < PHRASE_LENGTH; ++j)
            read(&song->phrases[i].commands[j].command, 1, user_data);
    }
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        for (int j = 0; j < PHRASE_LENGTH; ++j)
            read(&song->phrases[i].commands[j].value, 1, user_data);
    }
    
    read(song->empty5fe0, sizeof(song->empty5fe0), user_data);
}

void write_bank2(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data)
{
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        for (int j = 0; j < PHRASE_LENGTH; ++j)
            write(&song->phrases[i].commands[j].command, 1, user_data);
    }
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
    {
        for (int j = 0; j < PHRASE_LENGTH; ++j)
            write(&song->phrases[i].commands[j].value, 1, user_data);
    }
    
    write(song->empty5fe0, sizeof(song->empty5fe0), user_data);
}

void read_bank3(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song)
{
    for (int i = 0; i < WAVE_COUNT; ++i)
        read(song->waves[i], WAVE_LENGTH, user_data);
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
        read(&song->phrases[i].instruments, PHRASE_LENGTH, user_data);
    
    seek(2, SEEK_CUR, user_data); // rb
    
    read(song->empty7ff2, sizeof(song->empty7ff2), user_data);
    read(&song->version, 1, user_data);
}

void write_bank3(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data)
{
    for (int i = 0; i < WAVE_COUNT; ++i)
        write(song->waves[i], sizeof(WAVE_LENGTH), user_data);
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
        write(&song->phrases[i].instruments, PHRASE_LENGTH, user_data);
    
    write("rb", 2, user_data);
    
    write(song->empty7ff2, sizeof(song->empty7ff2), user_data);
    write(&song->version, 1, user_data);
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
    seek(0x1E78, SEEK_SET, user_data);
    
    char data[2];
    read(data, 2, user_data);
    
    if (data[0] != 'r' || data[1] != 'b')
        return lsdj_create_error(error, "memory flag 'rb' not found");
    
    seek(begin, SEEK_SET, user_data);
    
    read_bank0(read, seek, user_data, song);
    read_bank1(read, seek, user_data, song, error);
    read_bank2(read, user_data, song);
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
        lsdj_clear_chain(&song->chains[i]);
    
    for (int i = 0; i < PHRASE_COUNT; ++i)
        lsdj_clear_phrase(&song->phrases[i]);
    
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
        lsdj_clear_instrument(&song->instruments[i]);
    
    for (int i = 0; i < WAVE_COUNT; ++i)
        memcpy(song->waves[i], DEFAULT_WAVE, WAVE_LENGTH);
    
    for (int i = 0; i < TABLE_COUNT; ++i)
        lsdj_clear_table(&song->tables[i]);
    
    for (int i = 0; i < GROOVE_COUNT; ++i)
    {
        song->grooves[i][0] = 0x06;
        song->grooves[i][1] = 0x06;
        memset(&song->grooves[2], 0, GROOVE_LENGTH - 2);
    }
    
    memset(song->bookmarks, 0xFF, sizeof(BOOKMARK_COUNT));
    memset(song->instrumentSpeechWords, 0, sizeof(song->instrumentSpeechWords));
    memcpy(song->instrumentSpeechWordNames, DEFAULT_WORD_NAMES, sizeof(song->instrumentSpeechWordNames));
    memset(song->chainAllocTable, 0, sizeof(song->chainAllocTable));
    memset(song->phraseAllocTable, 0, sizeof(song->phraseAllocTable));
    memset(song->instrAllocTable, 0, sizeof(song->instrAllocTable));
    memset(song->tableAllocTable, 0, sizeof(song->tableAllocTable));
    
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
