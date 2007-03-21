/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOAUDIOMIXER_DEFINED
#define IOAUDIOMIXER_DEFINED 1

#include "IoAudioDevice.h"
#include "IoSound.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoSeq.h"

#define ISAUDIOMIXER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoAudioMixer_rawClone)

#define AUDIOEVENT_ADD 0
#define AUDIOEVENT_REMOVE 1

typedef struct 
{
  IoSound *ioTriggerSound;
  long sample;
  IoSound *ioPlaySound;
  char etype;
} AudioEvent;

typedef IoObject IoAudioMixer;

typedef struct
{
  IoSeq *ioBuffer;
  UArray *buffer;
  UArray *mixBuffer;
  float tempo;
  float pitch;
  int sampleRate;
  List *sounds;
  List *events;
  List *activeEvents;
  List *soundsToRemove;
  IoAudioDevice *ioAudioDevice;
  IoMessage *writeMessage;
  IoMessage *nonBlockingWriteMessage;
  char isRunning;
  float scale;
  int samplesPerBuffer;
  double volume;
  void *soundTouch;
} IoAudioMixerData;

IoAudioMixer *IoAudioMixer_proto(void *state);
IoAudioMixer *IoAudioMixer_new(void *state);
IoAudioMixer *IoAudioMixer_rawClone(IoAudioMixer *self);

void IoAudioMixer_free(IoAudioMixer *self);
void IoAudioMixer_mark(IoAudioMixer *self);

/* ----------------------------------------------------------- */
void IoAudioMixer_updateBufferSize(IoAudioMixer *self);
IoObject *IoAudioMixer_setAudioDevice(IoAudioMixer *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioMixer_setSamplesPerBuffer(IoAudioMixer *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioMixer_setVolume(IoAudioMixer *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioMixer_addSound_(IoAudioMixer *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioMixer_addSound_onSample_ofSound_(IoAudioMixer *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioMixer_removeSound_(IoAudioMixer *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioMixer_removeSound_onSample_ofSound_(IoAudioMixer *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioMixer_removeAllSounds(IoAudioMixer *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioMixer_sounds(IoAudioMixer *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioMixer_start(IoAudioMixer *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioMixer_stop(IoAudioMixer *self, IoObject *locals, IoMessage *m);
void IoAudioMixer_processActiveEvents(IoAudioMixer *self);
int IoAudioMixer_mixOneChunk(IoAudioMixer *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioMixer_setTempo(IoAudioMixer *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioMixer_setSampleRate(IoAudioMixer *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioMixer_setPitchSemiTones(IoAudioMixer *self, IoObject *locals, IoMessage *m);

#endif
