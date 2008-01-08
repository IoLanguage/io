#ifndef SoundTouch_wrapper_H
#define SoundTouch_wrapper_H

#ifdef __cplusplus
extern "C" {
#endif

void *SoundTouch_init(void);
void SoundTouch_free(void *stouch);

void SoundTouch_setSampleRate(void *stouch, unsigned int sampleRate);
void SoundTouch_setChannels(void *stouch, unsigned int channels);
void SoundTouch_setTempoChange(void *stouch, float percentTempo);
void SoundTouch_setTempo(void *stouch, float tempo); /* normal = 1.0 */
void SoundTouch_setPitchSemiTones(void *stouch, float semiTones);

void SoundTouch_putSamples(void* stouch, float *samples, unsigned int numSamples);
unsigned int SoundTouch_receiveSamples(void *stouch, float *samples, unsigned int maxSamples);

#ifdef __cplusplus
}
#endif

#endif
