//
// cwrapper.cpp - Binding for pitch and tempo resampling
//

#include <soundtouch/SoundTouch.h>
#include "SoundTouch_wrapper.h"

using namespace soundtouch;

void *SoundTouch_init(void) 
{
    SoundTouch *soundTouch = new SoundTouch();
    soundTouch->setSetting(SETTING_USE_QUICKSEEK, false);
    soundTouch->setSetting(SETTING_USE_AA_FILTER, true);
    return (void *)soundTouch;
}

void SoundTouch_setSampleRate(void *stouch, unsigned int sampleRate) 
{
    SoundTouch *soundTouch = (SoundTouch *)stouch;
    soundTouch->setSampleRate(sampleRate); 
}

void SoundTouch_setChannels(void *stouch, unsigned int channels) 
{
    SoundTouch *soundTouch = (SoundTouch *)stouch;
    soundTouch->setChannels(channels);
}

void SoundTouch_setTempoChange(void *stouch, float percentTempo) 
{
    SoundTouch *soundTouch = (SoundTouch *)stouch;
    soundTouch->setTempoChange(percentTempo);
}

void SoundTouch_setTempo(void *stouch, float tempo) 
{
    SoundTouch *soundTouch = (SoundTouch *)stouch;
    soundTouch->setTempo(tempo);
}

void SoundTouch_setPitchSemiTones(void *stouch, float semiTones) 
{
    SoundTouch *soundTouch = (SoundTouch *)stouch;
    soundTouch->setPitchSemiTones(semiTones);
}

void SoundTouch_free(void *stouch) 
{
    SoundTouch *soundTouch = (SoundTouch *)stouch;
    delete soundTouch;
}

void SoundTouch_putSamples(void *stouch, float *samples, unsigned int numSamples) 
{
    SoundTouch *soundTouch = (SoundTouch *)stouch;
    soundTouch->putSamples(samples, numSamples);
}

unsigned int SoundTouch_receiveSamples(void *stouch, float *samples, unsigned int maxSamples) 
{
    SoundTouch *soundTouch = (SoundTouch *)stouch;
    return soundTouch->receiveSamples(samples, maxSamples);
}
