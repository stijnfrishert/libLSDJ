/*
 
 This file is a part of liblsdj, a C library for managing everything
 that has to do with LSDJ, software for writing music (chiptune) with
 your gameboy. For more information, see:
 
 * https://github.com/stijnfrishert/liblsdj
 * http://www.littlesounddj.com
 
 --------------------------------------------------------------------------------
 
 MIT License
 
 Copyright (c) 2018 - 2020 Stijn Frishert
 
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

#include "vio.h"

#include <assert.h>
#include <string.h>


// --- File --- //

size_t lsdj_fread(void* ptr, size_t size, void* userData)
{
    return fread(ptr, size, 1, (FILE*)userData) * size;
}

size_t lsdj_fwrite(const void* ptr, size_t size, void* userData)
{
    return fwrite(ptr, size, 1, (FILE*)userData) * size;
}

long lsdj_ftell(void* userData)
{
    return ftell((FILE*)userData);
}

long lsdj_fseek(long offset, int whence, void* userData)
{
    return fseek((FILE*)userData, offset, whence);
}

lsdj_vio_t lsdj_create_file_vio(FILE* file)
{
    lsdj_vio_t vio;

    vio.read = lsdj_fread;
    vio.write = lsdj_fwrite;
    vio.tell = lsdj_ftell;
    vio.seek = lsdj_fseek;
    vio.userData = (void*)file;

    return vio;
}


// --- Memory --- //

size_t lsdj_mread(void* ptr, size_t size, void* userData)
{
    lsdj_memory_access_state_t* mem = (lsdj_memory_access_state_t*)userData;
    
    assert(mem->cur <= mem->begin + mem->size);
    
    const size_t available = mem->size - (size_t)(mem->cur - mem->begin);
    const size_t minSize = size < available ? size : available;
    
    memcpy(ptr, mem->cur, minSize);
    mem->cur += minSize;
    assert(mem->cur <= mem->begin + mem->size);
    
    return minSize;
}

size_t lsdj_mwrite(const void* ptr, size_t size, void* userData)
{
    lsdj_memory_access_state_t* mem = (lsdj_memory_access_state_t*)userData;
    
    const size_t available = mem->size - (size_t)(mem->cur - mem->begin);
    const size_t minSize = size < available ? size : available;
    
    memcpy(mem->cur, ptr, minSize);
    mem->cur += minSize;
    assert(mem->cur <= mem->begin + mem->size);
    
    return minSize;
}

long lsdj_mtell(void* userData)
{
    const lsdj_memory_access_state_t* mem = (const lsdj_memory_access_state_t*)userData;
    
    long pos = mem->cur - mem->begin;
    if (pos < 0 || pos > mem->size)
        return -1L;
    
    return pos;
}

long lsdj_mseek(long offset, int whence, void* userData)
{
    lsdj_memory_access_state_t* mem = (lsdj_memory_access_state_t*)userData;
    
    unsigned char* cur = mem->cur;
    
    switch (whence)
    {
        case SEEK_SET: mem->cur = mem->begin + offset; break;
        case SEEK_CUR: mem->cur = mem->cur + offset; break;
        case SEEK_END: mem->cur = mem->begin + mem->size + offset; break;
    }
    
    if (mem->cur < mem->begin ||
        mem->cur > mem->begin + mem->size)
    {
        mem->cur = cur;
        return 1;
    }
    
    return 0;
}

lsdj_vio_t lsdj_create_memory_vio(lsdj_memory_access_state_t* state)
{
    lsdj_vio_t vio;

    vio.read = lsdj_mread;
    vio.write = lsdj_mwrite;
    vio.tell = lsdj_mtell;
    vio.seek = lsdj_mseek;
    vio.userData = (void*)state;

    return vio;
}

