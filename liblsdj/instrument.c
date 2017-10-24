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
    instrument->pulse.tuning = TUNE_12_TONE;
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
    instrument->wave.tuning = TUNE_12_TONE;
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
    instrument->kit.loop1 = KIT_LOOP_OFF;
    
    instrument->kit.kit2 = 0;
    instrument->kit.offset2 = 0;
    instrument->kit.length2 = KIT_LENGTH_AUTO;
    instrument->kit.loop2 = KIT_LOOP_OFF;
    
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
    switch (byte & 0x3)
    {
        case 0: return PAN_NONE;
        case 1: return PAN_RIGHT;
        case 2: return PAN_LEFT;
        case 3: return PAN_LEFT_RIGHT;
        default: return PAN_LEFT_RIGHT;
    }
}

tuning_mode parseTuning(unsigned char byte)
{
    switch ((byte >> 5) & 0x3)
    {
        case 0: return TUNE_12_TONE;
        case 1: return TUNE_FIXED;
        case 2: return TUNE_DRUM;
        default: return TUNE_12_TONE;
    }
}

unsigned char parseAutomate(unsigned char byte)
{
    return (byte >> 3) & 3;
}

plvib_type parsePlvib(unsigned char byte)
{
    switch ((byte >> 1) & 0x3)
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
    switch (byte & 0x1)
    {
        case 0: return VIB_UP;
        case 1: return VIB_DOWN;
        default: return VIB_UP;
    }
}

pulse_width parsePulseWidth(unsigned char byte)
{
    switch ((byte >> 6) & 0x3)
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
    switch (byte & 0x3)
    {
        case 0: return PLAY_ONCE;
        case 1: return PLAY_LOOP;
        case 2: return PLAY_PING_PONG;
        case 3: return PLAY_MANUAL;
        default: return PLAY_ONCE;
    }
}

kit_pspeed parsePspeed(unsigned char byte, lsdj_error_t** error)
{
    switch ((byte >> 1) & 0x3)
    {
        case 0: return KIT_PSPEED_FAST;
        case 1: return KIT_PSPEED_SLOW;
        case 2: return KIT_PSPEED_STEP;
        default: lsdj_create_error(error, "unknown kit pspeed bit pattern"); return KIT_PSPEED_FAST;
    }
}

kit_distortion parseKitDistortion(unsigned char byte)
{
    switch (byte & 0x3)
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
    switch (byte & 0x1)
    {
        case 0: return SCOMMAND_FREE;
        case 1: return SCOMMAND_STABLE;
        default: return SCOMMAND_FREE;
    }
}

void read_pulse_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, unsigned char version, lsdj_instrument_t* instrument, lsdj_error_t** error)
{
    instrument->type = INSTR_PULSE;
    read(&instrument->envelope, 1, user_data);
    read(&instrument->pulse.pulse2tune, 1, user_data);
    
    unsigned char byte;
    read(&byte, 1, user_data);
    instrument->pulse.length = parseLength(byte);
    
    read(&instrument->pulse.sweep, 1, user_data);
    
    read(&byte, 1, user_data);
    instrument->pulse.tuning = (version >= 0x03) ? parseTuning(byte) : TUNE_12_TONE;
    instrument->automate = parseAutomate(byte);
    instrument->pulse.plvib = parsePlvib(byte);
    instrument->pulse.vibratoDirection = parseVibrationDirection(byte);
    
    read(&byte, 1, user_data);
    instrument->table = parseTable(byte);
    
    read(&byte, 1, user_data);
    instrument->pulse.pulseWidth = parsePulseWidth(byte);
    instrument->pulse.fineTune = ((byte >> 2) & 0xF);
    instrument->panning = parsePanning(byte);
    
//    unsigned char data[8];
//    read(data, 8, user_data);
    seek(8, SEEK_CUR, user_data); // Bytes 8-15 are empty
}

void read_wave_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, unsigned char version, lsdj_instrument_t* instrument, lsdj_error_t** error)
{
    instrument->type = INSTR_WAVE;
    read(&instrument->volume, 1, user_data);
    
    unsigned char byte;
    read(&byte, 1, user_data);
    instrument->wave.synth = (byte >> 4) & 0xF;
    instrument->wave.repeat = (byte & 0xF);
    seek(2, SEEK_CUR, user_data); // Bytes 3 and 4 are empty
    
    read(&byte, 1, user_data);
    instrument->wave.tuning = (version >= 0x03) ? parseTuning(byte) : TUNE_12_TONE;
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
    instrument->wave.speed = (byte & 0xF);
    
    seek(1, SEEK_CUR, user_data); // Byte 15 is empty
}

void read_kit_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_instrument_t* instrument, lsdj_error_t** error)
{
    instrument->type = INSTR_KIT;
    read(&instrument->volume, 1, user_data);
    
    instrument->kit.loop1 = KIT_LOOP_OFF;
    instrument->kit.loop2 = KIT_LOOP_OFF;
    
    unsigned char byte;
    read(&byte, 1, user_data);
    if ((byte >> 7) & 1)
        instrument->kit.loop1 = KIT_LOOP_ATTACK;
    instrument->kit.halfSpeed = (byte >> 6) & 1;
    instrument->kit.kit1 = byte & 0x3F;
    read(&instrument->kit.length1, 1, user_data);
    
    seek(1, SEEK_CUR, user_data); // Byte 4 is empty
    
    read(&byte, 1, user_data);
    if (instrument->kit.loop1 != KIT_LOOP_ATTACK)
        instrument->kit.loop1 = ((byte >> 6) & 1) ? KIT_LOOP_ON : KIT_LOOP_OFF;
    instrument->kit.loop2 = ((byte >> 6) & 1) ? KIT_LOOP_ON : KIT_LOOP_OFF;
    instrument->automate = parseAutomate(byte);
    instrument->kit.pSpeed = parsePspeed(byte, error);
    if (*error)
        return;
//    instrument->kit.upDown = byte & 1; // up/down?
    
    read(&byte, 1, user_data);
    instrument->table = parseTable(byte);
    
    read(&byte, 1, user_data);
    instrument->panning = parsePanning(byte);
    
    read(&instrument->kit.pitch, 1, user_data);
    
    read(&byte, 1, user_data);
    if ((byte >> 7) & 1)
        instrument->kit.loop2 = KIT_LOOP_ATTACK;
    instrument->kit.kit2 = byte & 0x3F;
    
    read(&byte, 1, user_data);
    instrument->kit.distortion = parseKitDistortion(byte);
    
    read(&instrument->kit.length2, 1, user_data);
    read(&instrument->kit.offset1, 1, user_data);
    read(&instrument->kit.offset2, 1, user_data);
    
    seek(2, SEEK_CUR, user_data); // Bytes 14 and 15 are empty
}

void read_noise_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, lsdj_instrument_t* instrument, lsdj_error_t** error)
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

void lsdj_read_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, unsigned char version, lsdj_instrument_t* instrument, lsdj_error_t** error)
{
    if (read == NULL)
        return lsdj_create_error(error, "read is NULL");
    
    if (seek == NULL)
        return lsdj_create_error(error, "seek is NULL");
    
    if (instrument == NULL)
        return lsdj_create_error(error, "instrument is NULL");
    
    unsigned char type;
    read(&type, 1, user_data);
    
    switch (type)
    {
        case 0: read_pulse_instrument(read, seek, user_data, version, instrument, error); break;
        case 1: read_wave_instrument(read, seek, user_data, version, instrument, error); break;
        case 2: read_kit_instrument(read, seek, user_data, instrument, error); break;
        case 3: read_noise_instrument(read, seek, user_data, instrument, error); break;
        default: return lsdj_create_error(error, "unknown instrument type");
    }
}

unsigned char createWaveVolumeByte(unsigned char volume)
{
    return (volume > 0x3) ? 0x3 : volume;
}

unsigned char createPanningByte(panning pan)
{
    switch (pan)
    {
        case PAN_LEFT_RIGHT: return 0x3;
        case PAN_LEFT: return 0x2;
        case PAN_RIGHT: return 0x1;
        case PAN_NONE: return 0x0;
    }
}

unsigned char createLengthByte(unsigned char length)
{
    if (length >= UNLIMITED_LENGTH)
        return 0;
    else
        return ~(length & 0x3F);
}

unsigned char createTableByte(unsigned char table)
{
    if (table >= NO_TABLE)
        return 0;
    else
        return table & 0xF;
}

unsigned char createAutomateByte(unsigned char automate)
{
    return (automate == 0) ? 0x0 : 0x8;
}

unsigned char createTuningByte(tuning_mode tuning)
{
    switch (tuning)
    {
        case TUNE_12_TONE: return 0x0;
        case TUNE_FIXED: return 0x20;
        case TUNE_DRUM: return 0x40;
    }
}

unsigned char createPlvibByte(plvib_type plvib)
{
    switch (plvib)
    {
        case PLVIB_HIGH_FREQUENCY: return 0x0;
        case PLVIB_SAWTOOTH: return 0x2;
        case PLVIB_TRIANGLE: return 0x4;
        case PLVIB_SQUARE: return 0x6;
    }
}

unsigned char createVibrationDirectionByte(vibrato_direction dir)
{
    switch (dir)
    {
        case VIB_UP: return 0x0;
        case VIB_DOWN: return 0x1;
    }
}

unsigned char createPulseWidthByte(pulse_width pw)
{
    switch (pw)
    {
        case PULSE_WIDTH_125: return 0x0;
        case PULSE_WIDTH_25: return 0x40;
        case PULSE_WIDTH_50: return 0x80;
        case PULSE_WIDTH_75: return 0xC0;
    }
}

unsigned char createPlaybackModeByte(playback_mode play)
{
    switch (play)
    {
        case PLAY_ONCE: return 0x0;
        case PLAY_LOOP: return 0x1;
        case PLAY_PING_PONG: return 0x2;
        case PLAY_MANUAL: return 0x3;
    }
}

unsigned char createPspeedByte(kit_pspeed pspeed)
{
    switch (pspeed)
    {
        case KIT_PSPEED_FAST: return 0x0;
        case KIT_PSPEED_SLOW: return 0x2;
        case KIT_PSPEED_STEP: return 0x4;
    }
}

unsigned char createKitDistortionByte(kit_distortion dist)
{
    switch (dist)
    {
        case KIT_DIST_CLIP: return 0x0;
        case KIT_DIST_SHAPE: return 0x1;
        case KIT_DIST_SHAPE2: return 0x2;
        case KIT_DIST_WRAP: return 0x3;
    }
}

unsigned char createScommandByte(scommand_type type)
{
    switch (type)
    {
        case SCOMMAND_FREE: return 0x0;
        case SCOMMAND_STABLE: return 0x1;
    }
}

void write_pulse_instrument(const lsdj_instrument_t* instrument, unsigned char version, lsdj_vio_write_t write, void* user_data)
{
    unsigned char byte = 0;
    write(&byte, 1, user_data);
    write(&instrument->envelope, 1, user_data);
    write(&instrument->pulse.pulse2tune, 1, user_data);
    
    byte = createLengthByte(instrument->pulse.length);
    write(&byte, 1, user_data);
    write(&instrument->pulse.sweep, 1, user_data);
    
    byte = (version >= 0x03) ? createTuningByte(instrument->pulse.tuning) : 0;
    byte |= createAutomateByte(instrument->automate) | createPlvibByte(instrument->pulse.plvib) | createVibrationDirectionByte(instrument->pulse.vibratoDirection);
    write(&byte, 1, user_data);
    
    byte = createTableByte(instrument->table);
    write(&byte, 1, user_data);
    
    byte = (unsigned char)(createPulseWidthByte(instrument->pulse.pulseWidth) | ((instrument->pulse.fineTune & 0xF) << 2) | createPanningByte(instrument->panning));
    write(&byte, 1, user_data);
    
    static unsigned char empty[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    write(empty, sizeof(empty), user_data); // Bytes 8-15 are empty
}

void write_wave_instrument(const lsdj_instrument_t* instrument, unsigned char version, lsdj_vio_write_t write, void* user_data)
{
    unsigned char byte = 1;
    write(&byte, 1, user_data);
    
    byte = createWaveVolumeByte(instrument->volume);
    write(&byte, 1, user_data);
    
    byte = (unsigned char)((instrument->wave.synth & 0xF) << 4) | (instrument->wave.repeat & 0xF);
    write(&byte, 1, user_data);
    
    byte = 0;
    write(&byte, 1, user_data); // Byte 3 is empty
    write(&byte, 1, user_data); // Byte 4 is empty
    
    byte = (version >= 0x03) ? createTuningByte(instrument->pulse.tuning) : 0;
    byte |= createAutomateByte(instrument->automate) | createPlvibByte(instrument->wave.plvib) | createVibrationDirectionByte(instrument->wave.vibratoDirection);
    write(&byte, 1, user_data);
    
    byte = createTableByte(instrument->table);
    write(&byte, 1, user_data);
    
    byte = createPanningByte(instrument->panning);
    write(&byte, 1, user_data);
    
    byte = 0;
    write(&byte, 1, user_data); // Byte 8 is empty
    
    byte = createPlaybackModeByte(instrument->wave.playback);
    write(&byte, 1, user_data);
    
    byte = 0;
    write(&byte, 1, user_data); // Byte 10 is empty
    write(&byte, 1, user_data); // Byte 11 is empty
    write(&byte, 1, user_data); // Byte 12 is empty
    write(&byte, 1, user_data); // Byte 13 is empty
    
    byte = (unsigned char)(((instrument->wave.length & 0xF) << 4) | (instrument->wave.speed & 0xF));
    write(&byte, 1, user_data);
    
    byte = 0;
    write(&byte, 1, user_data); // Byte 15 is empty
}

void write_kit_instrument(const lsdj_instrument_t* instrument, lsdj_vio_write_t write, void* user_data)
{
    unsigned char byte = 2;
    write(&byte, 1, user_data);
    
    byte = createWaveVolumeByte(instrument->volume);
    write(&byte, 1, user_data);
    
    byte = ((instrument->kit.loop1 == KIT_LOOP_ATTACK) ? 0x80 : 0x0) | (instrument->kit.halfSpeed ? 0x40 : 0x0) | (instrument->kit.kit1 & 0x3F); // Keep attack 1?
    write(&byte, 1, user_data);
    
    write(&instrument->kit.length1, 1, user_data);
    
    byte = 0;
    write(&byte, 1, user_data); // Byte 4 is empty
    
    byte = ((instrument->kit.loop1 == KIT_LOOP_ON) ? 0x80 : 0x0) |
           ((instrument->kit.loop2 == KIT_LOOP_ON) ? 0x40 : 0x0) |
           createAutomateByte(instrument->automate) |
           createPspeedByte(instrument->kit.pSpeed) |
           0x0; // up/down?
    write(&byte, 1, user_data);
    
    byte = createTableByte(instrument->table);
    write(&byte, 1, user_data);
    
    byte = createPanningByte(instrument->panning);
    write(&byte, 1, user_data);
    
    write(&instrument->kit.pitch, 1, user_data);
    
    byte = ((instrument->kit.loop2 == KIT_LOOP_ATTACK) ? 0x80 : 0x0) | (instrument->kit.kit2 & 0x3F);
    write(&byte, 1, user_data);
    
    byte = createKitDistortionByte(instrument->kit.distortion);
    write(&byte, 1, user_data);
    
    write(&instrument->kit.length2, 1, user_data);
    write(&instrument->kit.offset1, 1, user_data);
    write(&instrument->kit.offset2, 1, user_data);
    
    byte = 0;
    write(&byte, 1, user_data); // Byte 14 is empty
    write(&byte, 1, user_data); // Byte 15 is empty
}

void write_noise_instrument(const lsdj_instrument_t* instrument, lsdj_vio_write_t write, void* user_data)
{
    unsigned char byte = 3;
    write(&byte, 1, user_data);
    
    write(&instrument->envelope, 1, user_data);
    
    byte = createScommandByte(instrument->noise.sCommand);
    write(&byte, 1, user_data);
    
    byte = createLengthByte(instrument->noise.length);
    write(&byte, 1, user_data);
    
    write(&instrument->noise.shape, 1, user_data);
    
    byte = createAutomateByte(instrument->automate);
    write(&byte, 1, user_data);
    
    byte = createTableByte(instrument->table);
    write(&byte, 1, user_data);
    
    byte = createPanningByte(instrument->panning);
    write(&byte, 1, user_data);
    
    static unsigned char empty[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    write(empty, sizeof(empty), user_data); // Bytes 8-15 are empty
}

void lsdj_write_instrument(const lsdj_instrument_t* instrument, unsigned char version, lsdj_vio_write_t write, void* user_data, lsdj_error_t** error)
{
    if (write == NULL)
        return lsdj_create_error(error, "write is NULL");
    
    if (instrument == NULL)
        return lsdj_create_error(error, "instrument is NULL");
    
    switch (instrument->type)
    {
        case INSTR_PULSE: write_pulse_instrument(instrument, version, write, user_data); break;
        case INSTR_WAVE: write_wave_instrument(instrument, version, write, user_data); break;
        case INSTR_KIT: write_kit_instrument(instrument, write, user_data); break;
        case INSTR_NOISE: write_noise_instrument(instrument, write, user_data); break;
    }
}
