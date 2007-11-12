/*   Copyright (c) 2002, Steve Dekorte
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoAudioTrack_DEFINED
#define IoAudioTrack_DEFINED 1

#include "IoObject.h"
#include "IoString.h"
#include "IoBuffer.h"
#include "Sound.h"

#define ISAUDIOTRACK(self) (IoObject_tag(v)->cloneFunc == (IoTagCloneFunc *)IoAudioTrack_rawClone)

typedef IoObject IoAudioTrack;

typedef struct
{
  Sound *sound;
  IoBuffer *buffer;
} IoAudioTrackData;

IoObject *IoMessage_locals_audioTrackArgAt_(IoMessage *self, IoObject *locals, int n);

IoAudioTrack *IoAudioTrack_proto(void *state);
IoAudioTrack *IoAudioTrack_new(void *state);
IoAudioTrack *IoAudioTrack_rawClone(IoAudioTrack *self);
void IoAudioTrack_mark(IoAudioTrack *self);
void IoAudioTrack_free(IoAudioTrack *self);
Sound *IoAudioTrack_rawSound(IoAudioTrack *self);

/* ----------------------------------------------------------- */
IoObject *IoAudioTrack_setPath(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_path(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_open(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_read(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_read(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_write(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_setSampleRate(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_sampleRate(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_setChannels(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_channels(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_setBuffer(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_buffer(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_sampleCount(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_seconds(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_setVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_volume(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_setChannelVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_channelVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_aveOutput(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_aveOutputCountLength(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_setAveOutputCountLength(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_setPosition(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_position(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_setPlayPosition(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_playPosition(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_isLooping(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_loopingOn(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_loopingOff(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_resampleToSampleCount(IoAudioTrack *self, IoObject *locals, IoMessage *m);

/* --- OpenGL --- */
IoObject *IoAudioTrack_setPlayLength(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_playLength(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_setPixelsPerVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_pixelsPerVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioTrack_pixelsPerSecond(IoAudioTrack *self, IoObject *locals, IoMessage *m);

/* --- volumes --- */
IoObject *IoAudioTrack_setVolumeFromTo(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_volumeAt(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_setSamplesPerVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_samplesPerVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m);

/* --- wave form ------------------------------- */
IoObject *IoAudioTrack_waveFormDisplayList(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_updateWaveFormDisplayList(IoAudioTrack *self, IoObject *locals, IoMessage *m);

/* --- volume mask ------------------------------- */
IoObject *IoAudioTrack_volumeMaskDisplayList(IoAudioTrack *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioTrack_updateVolumeMaskDisplayList(IoAudioTrack *self, IoObject *locals, IoMessage *m);



#endif
