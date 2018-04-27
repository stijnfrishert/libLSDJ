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

#ifndef LSDJ_SAV_H
#define LSDJ_SAV_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

#include "error.h"
#include "project.h"
#include "song.h"
#include "vio.h"
    
#define NO_ACTIVE_PROJECT (0xFF)
    
typedef struct lsdj_sav_t lsdj_sav_t;

// Create/free saves
lsdj_sav_t* lsdj_new_sav(lsdj_error_t** error);
void lsdj_free_sav(lsdj_sav_t* sav);
    
// Deserialize a sav
lsdj_sav_t* lsdj_read_sav(lsdj_vio_t* vio, lsdj_error_t** error);
lsdj_sav_t* lsdj_read_sav_from_file(const char* path, lsdj_error_t** error);
lsdj_sav_t* lsdj_read_sav_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error);
    
// Serialize a sav
void lsdj_write_sav(const lsdj_sav_t* sav, lsdj_vio_t* vio, lsdj_error_t** error);
void lsdj_write_sav_to_file(const lsdj_sav_t* sav, const char* path, lsdj_error_t** error);
void lsdj_write_sav_to_memory(const lsdj_sav_t* sav, unsigned char* data, size_t size, lsdj_error_t** error);
    
// Change data in a sav
void lsdj_sav_set_song(lsdj_sav_t* sav, lsdj_song_t* song);
lsdj_song_t* lsdj_sav_get_song(const lsdj_sav_t* sav);
    
// Create a project that contains the working memory song
lsdj_project_t* lsdj_create_project_from_working_memory_song(const lsdj_sav_t* sav, lsdj_error_t** error);
void lsdj_sav_copy_active_project(lsdj_sav_t* sav, unsigned char index, lsdj_error_t** error);
    
// Retrieve hte project that is currently active
// If the working memory doesn't represent any active project, this is NO_ACTIVE_PROJECT
unsigned char lsdj_sav_get_active_project(const lsdj_sav_t* sav);
unsigned int lsdj_sav_get_project_count(const lsdj_sav_t* sav);
void lsdj_sav_set_project(lsdj_sav_t* sav, unsigned char index, lsdj_project_t* project, lsdj_error_t** error);
lsdj_project_t* lsdj_sav_get_project(const lsdj_sav_t* sav, unsigned char project);

#ifdef __cplusplus
}
#endif

#endif
