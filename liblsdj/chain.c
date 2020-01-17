/*
 
 This file is a part of liblsdj, a C library for managing everything
 that has to do with LSDJ, software for writing music (chiptune) with
 your gameboy. For more information, see:
 
 * https://github.com/stijnfrishert/liblsdj
 * http://www.littlesounddj.com
 
 --------------------------------------------------------------------------------
 
 MIT License
 
 Copyright (c) 2018 - 2019 Stijn Frishert
 
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

#include <stdlib.h>
#include <string.h>

#include "chain.h"

lsdj_chain_t* lsdj_chain_new()
{
    lsdj_chain_t* chain = (lsdj_chain_t*)malloc(sizeof(lsdj_chain_t));
    lsdj_chain_clear(chain);
    return chain;
}

lsdj_chain_t* lsdj_chain_copy(const lsdj_chain_t* chain)
{
    lsdj_chain_t* newChain = malloc(sizeof(lsdj_chain_t));
    memcpy(newChain, chain, sizeof(lsdj_chain_t));
    return newChain;
}

void lsdj_chain_free(lsdj_chain_t* chain)
{
    free(chain);
}

void lsdj_chain_clear(lsdj_chain_t* chain)
{
    memset(chain->phrases, LSDJ_CHAIN_NO_PHRASE, LSDJ_CHAIN_LENGTH);
    memset(chain->transpositions, 0, LSDJ_CHAIN_LENGTH);
}

bool lsdj_chain_equals(const lsdj_chain_t* lhs, const lsdj_chain_t* rhs)
{
    return memcmp(lhs, rhs, sizeof(lsdj_chain_t)) == 0 ? true : false;
}

void lsdj_chain_replace_phrase(lsdj_chain_t* chain, unsigned char phrase, unsigned char replacement)
{
    for (int p = 0; p < LSDJ_CHAIN_LENGTH; p += 1)
    {
        if (chain->phrases[p] == phrase)
            chain->phrases[p] = replacement;
    }
}
