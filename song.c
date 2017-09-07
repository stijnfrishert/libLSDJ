#include "song.h"

const unsigned char RUN_LENGTH_ENCODING_BYTE = 0xC0;
const unsigned char SPECIAL_ACTION_BYTE = 0xE0;
const unsigned char END_OF_FILE_BYTE = 0xFF;
const unsigned char DEFAULT_WAVE_BYTE = 0xF0;
const unsigned char DEFAULT_INSTRUMENT_BYTE = 0xF1;

const unsigned char DEFAULT_WAVE[16] = { 0x8E, 0xCD, 0xCC, 0xBB, 0xAA, 0xA9, 0x99, 0x88, 0x87, 0x76, 0x66, 0x55, 0x54, 0x43, 0x32, 0x31 };
const unsigned char DEFAULT_INSTRUMENT[16] = { 0xA8, 0, 0, 0xFF, 0, 0, 3, 0, 0, 0xD0, 0, 0, 0, 0xF3, 0, 0 };

void lsdj_decompress_song(const unsigned char* data, lsdj_song_t* song)
{
    const unsigned char* read = data;
    unsigned char* write = song->data;
    
    int eof = 0;
    while (eof == 0)
    {
        switch (*read)
        {
            case RUN_LENGTH_ENCODING_BYTE:
            {
                unsigned char c = *++read;
                if (c == RUN_LENGTH_ENCODING_BYTE)
                {
                    *write++ = RUN_LENGTH_ENCODING_BYTE;
                } else {
                    unsigned char count = *++read;
                    for (int i = 0; i < count; ++i)
                        *write++ = c;
                }
                break;
            }
            case SPECIAL_ACTION_BYTE:
            {
                unsigned char c = *++read;
                switch (c)
                {
                    case SPECIAL_ACTION_BYTE:
                        *write++ = SPECIAL_ACTION_BYTE;
                        break;
                    case DEFAULT_WAVE_BYTE:
                        for (int i = 0; i < 16; ++i)
                            *write++ = DEFAULT_WAVE[i];
                        break;
                    case DEFAULT_INSTRUMENT_BYTE:
                        for (int i = 0; i < 16; ++i)
                            *write++ = DEFAULT_INSTRUMENT[i];
                        break;
                    case END_OF_FILE_BYTE:
                        eof = 1;
                        break;
                    default:
                        *write++ = c;
                        break;
                }
                break;
            }
            default:
                *write++ = *read++;
        }
    }
}
