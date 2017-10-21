//
//  synth.c
//  liblsdj
//
//  Created by Stijn on 21/10/2017.
//

#include <string.h>

#include "synth.h"

void lsdj_clear_synth(lsdj_synth_t* synth)
{
    synth->waveform = SYNTH_WAVEFORM_SAWTOOTH;
    synth->filter = SYNTH_FILTER_LOW_PASS;
    synth->resonance = 0;
    synth->distortion = SYNTH_DISTORTION_CLIP;
    synth->phase = SYNTH_PHASE_NORMAL;
    synth->volumeStart = 0x10;
    synth->cutOffStart = 0xFF;
    synth->phaseStart = 0;
    synth->vshiftStart = 0;
    synth->volumeEnd = 0x10;
    synth->cutOffEnd = 0xFF;
    synth->phaseEnd = 0;
    synth->vshiftEnd = 0;
    synth->reserved[0] = synth->reserved[1] = synth->reserved[2] = 0;
    
    synth->overwritten = 0;
}
