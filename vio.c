//
//  vio.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include <assert.h>
#include <string.h>

#include "vio.h"

size_t lsdj_fread(void* ptr, size_t size, void* user_data)
{
    return fread(ptr, size, 1, (FILE*)user_data);
}

size_t lsdj_fwrite(const void* ptr, size_t size, void* user_data)
{
    return fwrite(ptr, size, 1, (FILE*)user_data);
}

long lsdj_ftell(void* user_data)
{
    return ftell((FILE*)user_data);
}

long lsdj_fseek(long offset, int whence, void* user_data)
{
    return fseek((FILE*)user_data, offset, whence);
}

size_t lsdj_mread(void* ptr, size_t size, void* user_data)
{
    lsdj_memory_data_t* mem = (lsdj_memory_data_t*)user_data;
    
    memcpy(ptr, mem->cur, size);
    mem->cur += size;
    assert((mem->cur - mem->begin) <= mem->size);
    
    return size;
}

size_t lsdj_mwrite(const void* ptr, size_t size, void* user_data)
{
    lsdj_memory_data_t* mem = (lsdj_memory_data_t*)user_data;
    
    memcpy(mem->cur, ptr, size);
    mem->cur += size;
    
    const long pos = (mem->cur - mem->begin);
    printf("%04x\n", (int)pos);
    
    return size;
}

long lsdj_mtell(void* user_data)
{
    const lsdj_memory_data_t* mem = (const lsdj_memory_data_t*)user_data;
    
    return mem->cur - mem->begin;
}

long lsdj_mseek(long offset, int whence, void* user_data)
{
    lsdj_memory_data_t* mem = (lsdj_memory_data_t*)user_data;
    
    switch (whence)
    {
        case SEEK_SET: mem->cur = mem->begin + offset; break;
        case SEEK_CUR: mem->cur = mem->cur + offset; break;
        case SEEK_END: mem->cur = mem->begin + mem->size + offset; break;
    }
    
    return 0;
}
