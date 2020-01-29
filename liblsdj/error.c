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

#include "error.h"

#include <stdlib.h>
#include <string.h>

struct lsdj_error_t
{
    char* message;
    size_t length;
};

lsdj_error_t* lsdj_error_new(const char* message)
{
    // Allocate the error itself
    lsdj_error_t* error = (lsdj_error_t*)malloc(sizeof(lsdj_error_t));
    if (error == NULL)
        return NULL;

    // Compute the length of the message to be copied
    // Add one for the null-termination
    error->length = strlen(message) + 1;

    // Allocate space for the message
    error->message = malloc(error->length * sizeof(char));
    if (error->message == NULL)
    {
        free(error);
        return NULL;
    }

    // Copy the string over
    strncpy(error->message, message, error->length);

    return error;
}

void lsdj_error_optional_new(lsdj_error_t** error, const char* message)
{
    if (error)
        *error = lsdj_error_new(message);
}

void lsdj_error_free(lsdj_error_t* error)
{
    if (error)
    {
        if (error->message)
        {
            free(error->message);
            error->message = NULL;
            error->length = 0;
        }
        
        free(error);
    }
}

size_t lsdj_error_get_description_length(lsdj_error_t* error)
{
    // Subtract 1 for the null-termination
    return error->length - 1;
}

const char* lsdj_error_get_description(lsdj_error_t* error)
{
    return error->message;
}
