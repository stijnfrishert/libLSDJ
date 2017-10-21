#ifndef LSDJ_SYNTH_H
#define LSDJ_SYNTH_H

#ifdef __cplusplus
extern "C" {
#endif
    
static const unsigned char SYNTH_WAVEFORM_SAWTOOTH = 0;
static const unsigned char SYNTH_WAVEFORM_SQUARE = 1;
static const unsigned char SYNTH_WAVEFORM_TRIANGLE = 2;
    
static const unsigned char SYNTH_FILTER_LOW_PASS = 0;
static const unsigned char SYNTH_FILTER_HIGH_PASS = 1;
static const unsigned char SYNTH_FILTER_BAND_PASS = 2;
static const unsigned char SYNTH_FILTER_ALL_PASS = 3;
    
static const unsigned char SYNTH_DISTORTION_CLIP = 0;
static const unsigned char SYNTH_DISTORTION_WRAP = 1;
    
static const unsigned char SYNTH_PHASE_NORMAL = 0;
static const unsigned char SYNTH_PHASE_RESYNC = 1;
static const unsigned char SYNTH_PHASE_RESYNC2 = 2;

typedef struct
{
    unsigned char waveform;
    unsigned char filter;
    unsigned char resonance;
    unsigned char distortion;
    unsigned char phase;
    unsigned char volumeStart;
    unsigned char volumeEnd;
    unsigned char cutOffStart;
    unsigned char cutOffEnd;
    unsigned char phaseStart;
    unsigned char phaseEnd;
    unsigned char vshiftStart;
    unsigned char vshiftEnd;
    
    unsigned char reserved[3];
    
    unsigned char overwritten; // 0 if false, 1 if true
} lsdj_synth_t;

void lsdj_clear_synth(lsdj_synth_t* synth);
    
#ifdef __cplusplus
}
#endif

#endif
