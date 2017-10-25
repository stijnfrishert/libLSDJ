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
    instrument->panning = LSDJ_PAN_LEFT_RIGHT;
    instrument->table = NO_TABLE;
    instrument->automate = 0;
    
    instrument->pulse.pulseWidth = LSDJ_PULSE_WAVE_PW_125;
    instrument->pulse.length = UNLIMITED_LENGTH;
    instrument->pulse.sweep = 0xFF;
    instrument->pulse.plvibSpeed = LSDJ_PLVIB_FAST;
    instrument->pulse.vibShape = LSDJ_VIB_TRIANGLE;
    instrument->pulse.vibratoDirection = LSDJ_VIB_UP;
    instrument->pulse.tuning = LSDJ_TUNE_12_TONE;
    instrument->pulse.pulse2tune = 0;
    instrument->pulse.fineTune = 0;
}

void lsdj_clear_instrument_as_wave(lsdj_instrument_t* instrument)
{
    instrument->type = INSTR_WAVE;
    instrument->volume = 3;
    instrument->panning = LSDJ_PAN_LEFT_RIGHT;
    instrument->table = NO_TABLE;
    instrument->automate = 0;
    
    instrument->wave.plvibSpeed = LSDJ_PLVIB_FAST;
    instrument->wave.vibShape = LSDJ_VIB_TRIANGLE;
    instrument->wave.vibratoDirection = LSDJ_VIB_UP;
    instrument->wave.tuning = LSDJ_TUNE_12_TONE;
    instrument->wave.synth = 0;
    instrument->wave.playback = LSDJ_PLAY_ONCE;
    instrument->wave.length = 0x0F;
    instrument->wave.repeat = 0;
    instrument->wave.speed = 4;
}

void lsdj_clear_instrument_as_kit(lsdj_instrument_t* instrument)
{
    instrument->type = INSTR_KIT;
    instrument->volume = 3;
    instrument->panning = LSDJ_PAN_LEFT_RIGHT;
    instrument->table = NO_TABLE;
    instrument->automate = 0;
    
    instrument->kit.kit1 = 0;
    instrument->kit.offset1 = 0;
    instrument->kit.length1 = KIT_LENGTH_AUTO;
    instrument->kit.loop1 = LSDJ_KIT_LOOP_OFF;
    
    instrument->kit.kit2 = 0;
    instrument->kit.offset2 = 0;
    instrument->kit.length2 = KIT_LENGTH_AUTO;
    instrument->kit.loop2 = LSDJ_KIT_LOOP_OFF;
    
    instrument->kit.pitch = 0;
    instrument->kit.halfSpeed = 0;
    instrument->kit.distortion = LSDJ_KIT_DIST_CLIP;
    instrument->kit.plvibSpeed = LSDJ_PLVIB_FAST;
    instrument->kit.vibShape = LSDJ_VIB_TRIANGLE;
}

void lsdj_clear_instrument_as_noise(lsdj_instrument_t* instrument)
{
    instrument->type = INSTR_NOISE;
    instrument->envelope = 0xA8;
    instrument->panning = LSDJ_PAN_LEFT_RIGHT;
    instrument->table = NO_TABLE;
    instrument->automate = 0;
    
    instrument->noise.length = UNLIMITED_LENGTH;
    instrument->noise.shape = 0xFF;
    instrument->noise.sCommand = LSDJ_SCOMMAND_FREE;
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

lsdj_panning parsePanning(unsigned char byte)
{
    return byte & 3;
}

lsdj_tuning_mode parseTuning(unsigned char byte)
{
    return (byte >> 5) & 0x3;
}

unsigned char parseAutomate(unsigned char byte)
{
    return (byte >> 3) & 3;
}

lsdj_pulse_wave parsePulseWidth(unsigned char byte)
{
    return (byte >> 6) & 3;
}

lsdj_playback_mode parsePlaybackMode(unsigned char byte)
{
    return byte & 0x3;
}

lsdj_kit_distortion parseKitDistortion(unsigned char byte)
{
    return byte & 3;
}

lsdj_scommand_type parseScommand(unsigned char byte)
{
    return byte & 1;
}

void read_pulse_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, unsigned char version, lsdj_instrument_t* instrument, lsdj_error_t** error)
{
    instrument->type = INSTR_PULSE; // 0
    
    read(&instrument->envelope, 1, user_data); // 1
    
    read(&instrument->pulse.pulse2tune, 1, user_data); // 2
    
    // 3
    unsigned char byte;
    read(&byte, 1, user_data);
    instrument->pulse.length = parseLength(byte);
    
    // 4
    read(&instrument->pulse.sweep, 1, user_data);
    
    // 5
    read(&byte, 1, user_data);
    instrument->pulse.tuning = (version >= 0x03) ? parseTuning(byte) : LSDJ_TUNE_12_TONE;
    instrument->automate = parseAutomate(byte);
    instrument->pulse.vibratoDirection = byte & 1;
    
    if (version < 4)
    {
        switch ((byte >> 1) & 3)
        {
            case 0:
                instrument->pulse.plvibSpeed = LSDJ_PLVIB_FAST;
                instrument->pulse.vibShape = LSDJ_VIB_TRIANGLE;
                break;
            case 1:
                instrument->pulse.plvibSpeed = LSDJ_PLVIB_TICK;
                instrument->pulse.vibShape = LSDJ_VIB_SAWTOOTH;
                break;
            case 2:
                instrument->pulse.plvibSpeed = LSDJ_PLVIB_TICK;
                instrument->pulse.vibShape = LSDJ_VIB_TRIANGLE;
                break;
            case 3:
                instrument->pulse.plvibSpeed = LSDJ_PLVIB_TICK;
                instrument->pulse.vibShape = LSDJ_VIB_SQUARE;
                break;
        }
    } else {
        if (byte & 0x80)
            instrument->pulse.plvibSpeed = LSDJ_PLVIB_STEP;
        else if (byte & 0x10)
            instrument->pulse.plvibSpeed = LSDJ_PLVIB_TICK;
        else
            instrument->pulse.plvibSpeed = LSDJ_PLVIB_FAST;
        
        switch ((byte >> 1) & 3)
        {
            case 0: instrument->pulse.vibShape = LSDJ_VIB_TRIANGLE; break;
            case 1: instrument->pulse.vibShape = LSDJ_VIB_SAWTOOTH; break;
            case 2: instrument->pulse.vibShape = LSDJ_VIB_SQUARE; break;
        }
    }
    
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
    instrument->wave.tuning = (version >= 0x03) ? parseTuning(byte) : LSDJ_TUNE_12_TONE;
    instrument->automate = parseAutomate(byte);
    instrument->pulse.vibratoDirection = byte & 1;
    
    if (version < 4)
    {
        switch ((byte >> 1) & 3)
        {
            case 0:
                instrument->pulse.plvibSpeed = LSDJ_PLVIB_FAST;
                instrument->pulse.vibShape = LSDJ_VIB_TRIANGLE;
                break;
            case 1:
                instrument->pulse.plvibSpeed = LSDJ_PLVIB_TICK;
                instrument->pulse.vibShape = LSDJ_VIB_SAWTOOTH;
                break;
            case 2:
                instrument->pulse.plvibSpeed = LSDJ_PLVIB_TICK;
                instrument->pulse.vibShape = LSDJ_VIB_TRIANGLE;
                break;
            case 3:
                instrument->pulse.plvibSpeed = LSDJ_PLVIB_TICK;
                instrument->pulse.vibShape = LSDJ_VIB_SQUARE;
                break;
        }
    } else {
        if (byte & 0x80)
            instrument->wave.plvibSpeed = LSDJ_PLVIB_STEP;
        else if (byte & 0x10)
            instrument->wave.plvibSpeed = LSDJ_PLVIB_TICK;
        else
            instrument->wave.plvibSpeed = LSDJ_PLVIB_FAST;
        
        switch ((byte >> 1) & 3)
        {
            case 0: instrument->wave.vibShape = LSDJ_VIB_TRIANGLE; break;
            case 1: instrument->wave.vibShape = LSDJ_VIB_SAWTOOTH; break;
            case 2: instrument->wave.vibShape = LSDJ_VIB_SQUARE; break;
        }
        
        instrument->kit.vibratoDirection = (byte & 1) == 1 ? LSDJ_VIB_UP : LSDJ_VIB_DOWN;
    }
    
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

void read_kit_instrument(lsdj_vio_read_t read, lsdj_vio_seek_t seek, void* user_data, unsigned char version, lsdj_instrument_t* instrument, lsdj_error_t** error)
{
    instrument->type = INSTR_KIT;
    read(&instrument->volume, 1, user_data);
    
    instrument->kit.loop1 = LSDJ_KIT_LOOP_OFF;
    instrument->kit.loop2 = LSDJ_KIT_LOOP_OFF;
    
    unsigned char byte;
    read(&byte, 1, user_data);
    if ((byte >> 7) & 1)
        instrument->kit.loop1 = LSDJ_KIT_LOOP_ATTACK;
    instrument->kit.halfSpeed = (byte >> 6) & 1;
    instrument->kit.kit1 = byte & 0x3F;
    read(&instrument->kit.length1, 1, user_data);
    
    seek(1, SEEK_CUR, user_data); // Byte 4 is empty
    
    read(&byte, 1, user_data);
    if (instrument->kit.loop1 != LSDJ_KIT_LOOP_ATTACK)
        instrument->kit.loop1 = ((byte >> 6) & 1) ? LSDJ_KIT_LOOP_ON : LSDJ_KIT_LOOP_OFF;
    instrument->kit.loop2 = ((byte >> 6) & 1) ? LSDJ_KIT_LOOP_ON : LSDJ_KIT_LOOP_OFF;
    instrument->automate = parseAutomate(byte);
    
    instrument->kit.vibratoDirection = byte & 1;
    
    if (version < 4)
    {
        switch ((byte >> 1) & 3)
        {
            case 0:
                instrument->kit.plvibSpeed = LSDJ_PLVIB_FAST;
                instrument->kit.vibShape = LSDJ_VIB_TRIANGLE;
                break;
            case 1:
                instrument->kit.plvibSpeed = LSDJ_PLVIB_TICK;
                instrument->kit.vibShape = LSDJ_VIB_TRIANGLE;
                break;
            case 2:
                instrument->kit.plvibSpeed = LSDJ_PLVIB_STEP;
                instrument->kit.vibShape = LSDJ_VIB_TRIANGLE;
                break;
        }
    } else {
        if (byte & 0x80)
            instrument->kit.plvibSpeed = LSDJ_PLVIB_STEP;
        else if (byte & 0x10)
            instrument->kit.plvibSpeed = LSDJ_PLVIB_TICK;
        else
            instrument->kit.plvibSpeed = LSDJ_PLVIB_FAST;
        
        switch ((byte >> 1) & 3)
        {
            case 0: instrument->kit.vibShape = LSDJ_VIB_TRIANGLE; break;
            case 1: instrument->kit.vibShape = LSDJ_VIB_SAWTOOTH; break;
            case 2: instrument->kit.vibShape = LSDJ_VIB_SQUARE; break;
        }
    }
    
    read(&byte, 1, user_data);
    instrument->table = parseTable(byte);
    
    read(&byte, 1, user_data);
    instrument->panning = parsePanning(byte);
    
    read(&instrument->kit.pitch, 1, user_data);
    
    read(&byte, 1, user_data);
    if ((byte >> 7) & 1)
        instrument->kit.loop2 = LSDJ_KIT_LOOP_ATTACK;
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
        case 2: read_kit_instrument(read, seek, user_data, version, instrument, error); break;
        case 3: read_noise_instrument(read, seek, user_data, instrument, error); break;
        default: return lsdj_create_error(error, "unknown instrument type");
    }
}

unsigned char createWaveVolumeByte(unsigned char volume)
{
    return (volume > 0x3) ? 0x3 : volume;
}

unsigned char createPanningByte(lsdj_panning pan)
{
    return pan & 3;
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

unsigned char createTuningByte(lsdj_tuning_mode tuning)
{
    return (unsigned char)((tuning & 3) << 5);
}

unsigned char createVibrationDirectionByte(lsdj_vib_direction dir)
{
    return dir & 1;
}

unsigned char createPulseWidthByte(lsdj_pulse_wave pw)
{
    return (unsigned char)((pw & 3) << 6);
}

unsigned char createPlaybackModeByte(lsdj_playback_mode play)
{
    return play & 3;
}

unsigned char createKitDistortionByte(lsdj_kit_distortion dist)
{
    return dist;
}

unsigned char createScommandByte(lsdj_scommand_type type)
{
    return type & 1;
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
    byte |= createAutomateByte(instrument->automate) | createVibrationDirectionByte(instrument->pulse.vibratoDirection);
    
    if (version < 4)
    {
        switch (instrument->pulse.vibShape)
        {
            case LSDJ_VIB_SAWTOOTH: byte |= 2; break;
            case LSDJ_VIB_SQUARE: byte |= 6; break;
            case LSDJ_VIB_TRIANGLE:
                if (instrument->pulse.plvibSpeed != LSDJ_PLVIB_FAST)
                    byte |= 4;
                break;
        }
    } else {
        byte |= (instrument->pulse.vibShape & 3) << 1;
        if (instrument->pulse.plvibSpeed == LSDJ_PLVIB_TICK)
            byte |= 0x10;
        else if (instrument->pulse.plvibSpeed == LSDJ_PLVIB_STEP)
            byte |= 0x80;
    }
    
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
    byte |= createAutomateByte(instrument->automate) | createVibrationDirectionByte(instrument->wave.vibratoDirection);
    if (version < 4)
    {
        switch (instrument->pulse.vibShape)
        {
            case LSDJ_VIB_SAWTOOTH: byte |= 2; break;
            case LSDJ_VIB_SQUARE: byte |= 6; break;
            case LSDJ_VIB_TRIANGLE:
                if (instrument->pulse.plvibSpeed != LSDJ_PLVIB_FAST)
                    byte |= 4;
                break;
        }
    } else {
        byte |= (instrument->pulse.vibShape & 3) << 1;
        if (instrument->pulse.plvibSpeed == LSDJ_PLVIB_TICK)
            byte |= 0x10;
        else if (instrument->pulse.plvibSpeed == LSDJ_PLVIB_STEP)
            byte |= 0x80;
    }
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

void write_kit_instrument(const lsdj_instrument_t* instrument, unsigned char version, lsdj_vio_write_t write, void* user_data)
{
    unsigned char byte = 2;
    write(&byte, 1, user_data);
    
    byte = createWaveVolumeByte(instrument->volume);
    write(&byte, 1, user_data);
    
    byte = ((instrument->kit.loop1 == LSDJ_KIT_LOOP_ATTACK) ? 0x80 : 0x0) | (instrument->kit.halfSpeed ? 0x40 : 0x0) | (instrument->kit.kit1 & 0x3F); // Keep attack 1?
    write(&byte, 1, user_data);
    
    write(&instrument->kit.length1, 1, user_data);
    
    byte = 0;
    write(&byte, 1, user_data); // Byte 4 is empty
    
    byte = ((instrument->kit.loop1 == LSDJ_KIT_LOOP_ON) ? 0x80 : 0x0) |
           ((instrument->kit.loop2 == LSDJ_KIT_LOOP_ON) ? 0x40 : 0x0) |
           createAutomateByte(instrument->automate);
    
    if (version < 4)
    {
        byte |= (instrument->kit.plvibSpeed & 3) << 1;
    } else {
        byte |= (instrument->pulse.vibShape & 3) << 1;
        if (instrument->pulse.plvibSpeed == LSDJ_PLVIB_TICK)
            byte |= 0x10;
        else if (instrument->pulse.plvibSpeed == LSDJ_PLVIB_STEP)
            byte |= 0x80;
    }
    write(&byte, 1, user_data);
    
    byte = createTableByte(instrument->table);
    write(&byte, 1, user_data);
    
    byte = createPanningByte(instrument->panning);
    write(&byte, 1, user_data);
    
    write(&instrument->kit.pitch, 1, user_data);
    
    byte = ((instrument->kit.loop2 == LSDJ_KIT_LOOP_ATTACK) ? 0x80 : 0x0) | (instrument->kit.kit2 & 0x3F);
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
        case INSTR_KIT: write_kit_instrument(instrument, version, write, user_data); break;
        case INSTR_NOISE: write_noise_instrument(instrument, write, user_data); break;
    }
}
