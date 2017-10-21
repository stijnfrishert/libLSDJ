#ifndef LSDJ_VIO_H
#define LSDJ_VIO_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef size_t (*lsdj_vio_read_t)(void* ptr, size_t size, void* user_data);
typedef size_t (*lsdj_vio_write_t)(const void* ptr, size_t size, void* user_data);
typedef int (*lsdj_vio_seek_t)(long offset, int whence, void* user_data);
    
#ifdef __cplusplus
}
#endif

#endif
