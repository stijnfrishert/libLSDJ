#ifndef LSDJ_VIO_H
#define LSDJ_VIO_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef size_t (*lsdj_vio_read_t)(void* ptr, size_t size, void* user_data);
typedef size_t (*lsdj_vio_write_t)(const void* ptr, size_t size, void* user_data);
typedef long (*lsdj_vio_tell_t)(void* user_data);
typedef long (*lsdj_vio_seek_t)(long offset, int whence, void* user_data);
    
typedef struct
{
    unsigned char* begin;
    unsigned char* cur;
    size_t size;
} lsdj_memory_data_t;
    
size_t lsdj_fread(void* ptr, size_t size, void* user_data);
size_t lsdj_fwrite(const void* ptr, size_t size, void* user_data);
long lsdj_ftell(void* user_data);
long lsdj_fseek(long offset, int whence, void* user_data);
    
size_t lsdj_mread(void* ptr, size_t size, void* user_data);
size_t lsdj_mwrite(const void* ptr, size_t size, void* user_data);
long lsdj_mtell(void* user_data);
long lsdj_mseek(long offset, int whence, void* user_data);
    
#ifdef __cplusplus
}
#endif

#endif
