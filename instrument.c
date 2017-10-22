//
//  instrument.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include <string.h>

#include "instrument.h"

void lsdj_clear_instrument(lsdj_instrument_t* instrument)
{
    memset(instrument->name, 0, INSTRUMENT_NAME_LENGTH);
    lsdj_clear_instrument_as_pulse(instrument);
}

void lsdj_clear_instrument_as_pulse(lsdj_instrument_t* instrument)
{
    instrument->type = INSTR_PULSE;
    instrument->envelope = 0xA8;
    instrument->panning = PAN_LEFT_RIGHT;
    instrument->table = NO_TABLE;
    instrument->automate = 0;
    
    instrument->pulse.pulseWidth = PULSE_WIDTH_125;
    instrument->pulse.length = UNLIMITED_LENGTH;
    instrument->pulse.sweep = 0xFF;
    instrument->pulse.plvib = PLVIB_HIGH_FREQUENCY;
    instrument->pulse.vibratoDirection = VIB_UP;
    instrument->pulse.transpose = 1;
    instrument->pulse.pulse2tune = 0;
    instrument->pulse.fineTune = 0;
}

void lsdj_clear_instrument_as_wave(lsdj_instrument_t* instrument)
{
    instrument->type = INSTR_WAVE;
    instrument->volume = 3;
    instrument->panning = PAN_LEFT_RIGHT;
    instrument->table = NO_TABLE;
    instrument->automate = 0;
    
    instrument->wave.plvib = PLVIB_HIGH_FREQUENCY;
    instrument->wave.vibratoDirection = VIB_UP;
    instrument->wave.transpose = 1;
    instrument->wave.synth = 0;
    instrument->wave.playback = PLAY_ONCE;
    instrument->wave.length = 0x0F;
    instrument->wave.repeat = 0;
    instrument->wave.speed = 4;
}

void lsdj_clear_instrument_as_kit(lsdj_instrument_t* instrument)
{
    instrument->type = INSTR_KIT;
    instrument->volume = 3;
    instrument->panning = PAN_LEFT_RIGHT;
    instrument->table = NO_TABLE;
    instrument->automate = 0;
    
    instrument->kit.kit1 = 0;
    instrument->kit.offset1 = 0;
    instrument->kit.length1 = KIT_LENGTH_AUTO;
    instrument->kit.loop1 = 0;
    
    instrument->kit.kit2 = 0;
    instrument->kit.offset2 = 0;
    instrument->kit.length2 = KIT_LENGTH_AUTO;
    instrument->kit.loop2 = 0;
    
    instrument->kit.pitch = 0;
    instrument->kit.halfSpeed = 0;
    instrument->kit.distortion = KIT_DIST_CLIP;
    instrument->kit.pSpeed = KIT_PSPEED_FAST;
}

void lsdj_clear_instrument_as_noise(lsdj_instrument_t* instrument)
{
    instrument->type = INSTR_NOISE;
    instrument->envelope = 0xA8;
    instrument->panning = PAN_LEFT_RIGHT;
    instrument->table = NO_TABLE;
    instrument->automate = 0;
    
    instrument->noise.length = UNLIMITED_LENGTH;
    instrument->noise.shape = 0xFF;
    instrument->noise.sCommand = SCOMMAND_FREE;
}

// --- Reading --- //

unsigned char parseLength(unsigned char byte)
{
    if (byte & 0x40)
        return ~(byte & 0x3F);
    else
        return UNLIMITED_LENGTH;
}

unsigned char parseTable(unsigned char byte)
{
    if (byte & 0x20)
        return byte & 0x1F;
    else
        return NO_TABLE;
}

panning parsePanning(unsigned char byte)
{
    switch (byte & 3)
    {
        case 0: return PAN_NONE;
        case 1: return PAN_RIGHT;
        case 2: return PAN_LEFT;
        case 3: return PAN_LEFT_RIGHT;
        default: return PAN_LEFT_RIGHT;
    }
}

unsigned char parseAutomate(unsigned char byte)
{
    return (byte >> 3) & 3;
}

plvib_type parsePlvib(unsigned char byte)
{
    switch ((byte >> 1) & 2)
    {
        case 0: return PLVIB_HIGH_FREQUENCY;
        case 1: return PLVIB_SAWTOOTH;
        case 2: return PLVIB_TRIANGLE;
        case 3: return PLVIB_SQUARE;
        default: return PLVIB_HIGH_FREQUENCY;
    }
}

vibrato_direction parseVibrationDirection(unsigned char byte)
{
    switch (byte & 1)
    {
        case 0: return VIB_UP;
        case 1: return VIB_DOWN;
        default: return VIB_UP;
    }
}

pulse_width parsePulseWidth(unsigned char byte)
{
    switch ((byte >> 6) & 3)
    {
        case 0: return PULSE_WIDTH_125;
        case 1: return PULSE_WIDTH_25;
        case 2: return PULSE_WIDTH_50;
        case 3: return PULSE_WIDTH_75;
        default: return PULSE_WIDTH_125;
    }
}

playback_mode parsePlaybackMode(unsigned char byte)
{
    switch (byte & 3)
    {
        case 0: return PLAY_ONCE;
        case 1: return PLAY_LOOP;
        case 2: return PLAY_PING_PONG;
        case 3: return PLAY_MANUAL;
        default: return PLAY_ONCE;
    }
}

kit_pspeed parsePspeed(unsigned char byte)
{
    switch ((byte >> 1) & 3)
    {
        case 0: return KIT_PSPEED_FAST;
        case 1: return KIT_PSPEED_SLOW;
        default: return KIT_PSPEED_FAST;
    }
}

kit_distortion parseKitDistortion(unsigned char byte)
{
    switch (byte & 3)
    {
        case 0: return KIT_DIST_CLIP;
        case 1: return KIT_DIST_SHAPE;
        case 2: return KIT_DIST_SHAPE2;
        case 3: return KIT_DIST_WRAP;
        default: return KIT_DIST_CLIP;
    }
}

scommand_type parseScommand(unsigned char byte)
{
    switch (byte & 1)
    {
        case 0: return SCOMMAND_FREE;
        case 1: return SCOMMAND_STABLE;
        default: return SCOMMAND_FREE;
    }
}

void read_pulse_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_instrument_t* instrument)
{
    instrument->type = INSTR_PULSE;
    read(&instrument->envelope, 1, user_data);
    read(&instrument->pulse.pulse2tune, 1, user_data);
    
    unsigned char byte;
    read(&byte, 1, user_data);
    instrument->pulse.length = parseLength(byte);
    
    read(&instrument->pulse.sweep, 1, user_data);
    
    read(&byte, 1, user_data);
    instrument->automate = parseAutomate(byte);
    instrument->pulse.plvib = parsePlvib(byte);
    instrument->pulse.vibratoDirection = parseVibrationDirection(byte);
    
    read(&byte, 1, user_data);
    instrument->table = parseTable(byte);
    
    read(&byte, 1, user_data);
    instrument->pulse.pulseWidth = parsePulseWidth(byte);
    instrument->pulse.fineTune = ((byte >> 2) & 0xF);
    instrument->panning = parsePanning(byte);
    instrument->pulse.transpose = 0;
    
    seek(8, SEEK_CUR, user_data); // Bytes 8-15 are empty
}

void read_wave_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_instrument_t* instrument)
{
    instrument->type = INSTR_WAVE;
    read(&instrument->volume, 1, user_data);
    
    unsigned char byte;
    read(&byte, 1, user_data);
    instrument->wave.synth = (byte >> 4) & 0xF;
    instrument->wave.repeat = (byte & 0x7);
    seek(2, SEEK_CUR, user_data); // Bytes 3 and 4 are empty
    
    read(&byte, 1, user_data);
    instrument->automate = parseAutomate(byte);
    instrument->pulse.plvib = parsePlvib(byte);
    instrument->pulse.vibratoDirection = parseVibrationDirection(byte);
    
    read(&byte, 1, user_data);
    instrument->table = parseTable(byte);
    
    read(&byte, 1, user_data);
    instrument->panning = parsePanning(byte);
    
    seek(1, SEEK_CUR, user_data); // Byte 8 is empty
    
    read(&byte, 1, user_data);
    instrument->wave.playback = parsePlaybackMode(byte);
    
    seek(4, SEEK_CUR, user_data); // Bytes 10-13 are empty
    
    read(&byte, 1, user_data);
    instrument->wave.length = ((byte >> 4) & 0xF);
    instrument->wave.speed = (byte & 0x7);
    
    seek(1, SEEK_CUR, user_data); // Byte 15 is empty
    
    instrument->wave.transpose = 0;
}

void read_kit_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_instrument_t* instrument)
{
    instrument->type = INSTR_KIT;
    read(&instrument->volume, 1, user_data);
    
    unsigned char byte;
    read(&byte, 1, user_data);
//    instrument->kit.keepAttack1 = (byte >> 7) & 1; // keep attack 1?
    instrument->kit.halfSpeed = (byte >> 6) & 1;
    instrument->kit.kit1 = byte & 0x1F;
    read(&instrument->kit.length1, 1, user_data);
    
    seek(1, SEEK_CUR, user_data); // Byte 4 is empty
    
    read(&byte, 1, user_data);
    instrument->kit.loop1 = (byte >> 6) & 1;
    instrument->kit.loop2 = (byte >> 5) & 1;
    instrument->automate = parseAutomate(byte);
    instrument->kit.pSpeed = parsePspeed(byte);
//    instrument->kit.upDown = byte & 1; // up/down?
    
    read(&byte, 1, user_data);
    instrument->table = parseTable(byte);
    
    read(&byte, 1, user_data);
    instrument->panning = parsePanning(byte);
    
    read(&instrument->kit.pitch, 1, user_data);
    
    read(&byte, 1, user_data);
//    instrument->kit.keepAttack2 = (byte >> 7) & 1; // keep attack 2?
    instrument->kit.kit1 = byte & 0x1F;
    
    read(&byte, 1, user_data);
    instrument->kit.distortion = parseKitDistortion(byte);
    
    read(&instrument->kit.length2, 1, user_data);
    read(&instrument->kit.offset1, 1, user_data);
    read(&instrument->kit.offset2, 1, user_data);
    
    seek(2, SEEK_CUR, user_data); // Bytes 14 and 15 are empty
}

void read_noise_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_instrument_t* instrument)
{
    instrument->type = INSTR_NOISE;
    read(&instrument->envelope, 1, user_data);
    
    unsigned char byte;
    read(&byte, 1, user_data);
    instrument->noise.sCommand = parseScommand(byte);
    
    read(&byte, 1, user_data);
    instrument->noise.length = parseLength(byte);
    
    read(&instrument->noise.shape, 1, user_data);
    
    read(&byte, 1, user_data);
    instrument->automate = parseAutomate(byte);
    
    read(&byte, 1, user_data);
    instrument->table = parseTable(byte);
    
    read(&byte, 1, user_data);
    instrument->panning = parsePanning(byte);
    
    seek(8, SEEK_CUR, user_data); // Bytes 8-15 are empty
}

void read_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_instrument_t* instrument)
{
    unsigned char type;
    read(&type, 1, user_data);
    
    switch (type)
    {
        case 0: read_pulse_instrument(read, seek, user_data, instrument); break;
        case 1: read_wave_instrument(read, seek, user_data, instrument); break;
        case 2: read_kit_instrument(read, seek, user_data, instrument); break;
        case 3: read_noise_instrument(read, seek, user_data, instrument); break;
        default:
            lsdj_clear_instrument(instrument);
            seek(15, SEEK_CUR, user_data);
            break;
    }
}
