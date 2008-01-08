/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoSoundTouch_DEFINED
#define IoSoundTouch_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "SoundTouch_wrapper.h"

#define ISMP3DECODER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSoundTouch_rawClone)

typedef IoObject IoSoundTouch;

typedef struct
{
	void *soundTouch;
	IoSeq *inputBuffer;
	IoSeq *outputBuffer;
} IoSoundTouchData;

IoSoundTouch *IoSoundTouch_proto(void *state);
IoSoundTouch *IoSoundTouch_new(void *state);
IoSoundTouch *IoSoundTouch_rawClone(IoSoundTouch *self);

void IoSoundTouch_mark(IoSoundTouch *self);
void IoSoundTouch_free(IoSoundTouch *self);

// --- processing --------------------------------------------------------

IoObject *IoSoundTouch_start(IoSoundTouch *self, IoObject *locals, IoMessage *m);
IoObject *IoSoundTouch_process(IoSoundTouch *self, IoObject *locals, IoMessage *m);
IoObject *IoSoundTouch_stop(IoSoundTouch *self, IoObject *locals, IoMessage *m);

// --- options -------------------------------------------------

IoObject *IoSoundTouch_setSampleRate(IoSoundTouch *self, IoObject *locals, IoMessage *m);
IoObject *IoSoundTouch_setChannels(IoSoundTouch *self, IoObject *locals, IoMessage *m);
IoObject *IoSoundTouch_setTempoChange(IoSoundTouch *self, IoObject *locals, IoMessage *m);
IoObject *IoSoundTouch_setTempo(IoSoundTouch *self, IoObject *locals, IoMessage *m);
IoObject *IoSoundTouch_setPitchSemitones(IoSoundTouch *self, IoObject *locals, IoMessage *m);
IoObject *IoSoundTouch_inputBuffer(IoSoundTouch *self, IoObject *locals, IoMessage *m);

// --- buffers -------------------------------------------------------

IoObject *IoSoundTouch_inputBuffer(IoSoundTouch *self, IoObject *locals, IoMessage *m);
IoObject *IoSoundTouch_outputBuffer(IoSoundTouch *self, IoObject *locals, IoMessage *m);


#endif
