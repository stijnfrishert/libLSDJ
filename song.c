#include <stdlib.h>
#include <string.h>

#include "song.h"

void read_bank0(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song)
{
    const long begin = tell(user_data);
    read(song->bank0, 0x2000, user_data);
    seek(begin, SEEK_SET, user_data);
    
    read(song->phrases, sizeof(song->phrases), user_data);
    read(song->bookmarks, sizeof(song->bookmarks), user_data);
    seek(96, SEEK_CUR, user_data); // empty space
    read(song->grooves, sizeof(song->grooves), user_data);
    read(song->sequences, sizeof(song->sequences), user_data);
    seek(512, SEEK_CUR, user_data); // tables->envelope
    seek(1344, SEEK_CUR, user_data); // instrument->speech->words
    seek(168, SEEK_CUR, user_data); // instrument->speech->wordnames
    seek(2, SEEK_CUR, user_data); // rb
    
    // Read the instrument names
    for (int i = 0; i < INSTRUMENT_COUNT; ++i)
        read(song->instruments[i].name, sizeof(song->instruments[i].name), user_data);

    seek(70, SEEK_CUR, user_data);
}

void write_bank0(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data)
{
    write(song->bank0, 0x2000, user_data);
}

void read_bank1(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song)
{
    read(song->bank1, 0x2000, user_data);
}

void write_bank1(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data)
{
    write(song->bank1, 0x2000, user_data);
}

void read_bank2(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song)
{
    read(song->bank2, 0x2000, user_data);
}

void write_bank2(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data)
{
    write(song->bank2, 0x2000, user_data);
}

void read_bank3(lsdj_vio_read_t read, lsdj_vio_tell_t tell, lsdj_vio_seek_t seek, void* user_data, lsdj_song_t* song)
{
    read(song->bank3, 0x2000, user_data);
}

void write_bank3(const lsdj_song_t* song, lsdj_vio_write_t write, void* user_data)
{
    write(song->bank3, 0x2000, user_data);
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
    
    read_bank0(read, tell, seek, user_data, song);
    read_bank1(read, tell, seek, user_data, song);
    read_bank2(read, tell, seek, user_data, song);
    read_bank3(read, tell, seek, user_data, song);
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
