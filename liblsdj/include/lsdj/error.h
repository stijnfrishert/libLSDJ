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

#ifndef LSDJ_ERROR_H
#define LSDJ_ERROR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Structure containing specific error details
/*! Whenever other functions in LibLSDj fail, they put their error state in one
	these structs. To catch errors, be sure create an lsdj_error_t*, and pass
	its address to the function.

	In case of an error your pointer will be filled. You can read it out, and
	its also your responsibility to call lsdj_error_free. */
typedef struct lsdj_error_t lsdj_error_t;

//! Create an error with a specific message
/*! Commonly you wouldn't call this function yourself, but receive the error
	from another function that wants to express error state.

	The error copies the message, so make sure it's null-terminated

    @note Every call to lsdj_error_new() should be paired with one to lsdj_error_free()

    @return NULL if creating the error failed */
lsdj_error_t* lsdj_error_new(const char* message);

//! Create an error with a specific message
/*! If the passed in pointer refers to a valid lsdj_error_t*, lsdj_error_new()
	is called and te result put in there.

	If this function is called with a null ptr, nothing happens. This may seem re-
	dundant, but this provides a nice convenience where functions can just pass
	their lsdj_error_t** to this function and return without worrying.

	@see lsdj_error_new
	@param error A pointer to an lsdj_error_t* to be filled */
void lsdj_error_optional_new(lsdj_error_t** error, const char* message);
    
//! Free error data
/*! Other functions in LibLSDJ often accept an lsdj_error_t** parameter.
	They fill it to express an error happened, in which case it is your responsibility
	to free the error with lsdj_error_free().

	@note Every call to lsdj_error_new() should be paired with one to lsdj_error_free() */
void lsdj_error_free(lsdj_error_t* error);

//! Retrieve the length of the string description of an error
/*! This does not include any null-termination characters at the end */
size_t lsdj_error_get_description_length(lsdj_error_t* error);
    
//! Retrieve a string description of an error
const char* lsdj_error_get_description(lsdj_error_t* error);
    
#ifdef __cplusplus
}
#endif

#endif
