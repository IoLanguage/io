/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoAudioDevice_DEFINED
#define IoAudioDevice_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoSeq.h"
#include "AudioDevice.h"

#define ISAUDIOOUTPUT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoAudioDevice_rawClone)

typedef IoObject IoAudioDevice;

typedef struct
{
	AudioDevice *audioDevice;
	IoSeq *writeBuffer;
	IoSeq *readBuffer;
	IoObject *pausedActor;
	//IoMessage *readyForInput;
} IoAudioDeviceData;

IoAudioDevice *IoAudioDevice_proto(void *state);
IoAudioDevice *IoAudioDevice_new(void *state);
IoAudioDevice *IoAudioDevice_rawClone(IoAudioDevice *self);

void IoAudioDevice_free(IoAudioDevice *self);
void IoAudioDevice_mark(IoAudioDevice *self);

AudioDevice *IoAudioDevice_rawAudioDevice(IoAudioDevice *self);
void IoAudioDevice_clearBuffers(IoAudioDevice *self);

/* ----------------------------------------------------------- */

IoObject *IoAudioDevice_open(IoAudioDevice *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioDevice_openForReadingAndWriting(IoAudioDevice *self, IoObject *locals, IoMessage *m);

IoObject *IoAudioDevice_close(IoAudioDevice *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioDevice_needsData(IoAudioDevice *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioDevice_asyncWrite(IoAudioDevice *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioDevice_read(IoAudioDevice *self, IoObject *locals, IoMessage *m);
void IoAudioDevice_AudioDeviceCallback(void *context, AudioDevice *audioDevice);

IoObject *IoAudioDevice_error(IoAudioDevice *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioDevice_isActive(IoAudioDevice *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioDevice_streamTime(IoAudioDevice *self, IoObject *locals, IoMessage *m);
IoObject *IoAudioDevice_writeBufferIsEmpty(IoAudioDevice *self, IoObject *locals, IoMessage *m);

#endif
