//metadoc SoundTouch copyright Steve Dekorte, 2004
//metadoc SoundTouch license BSD revised
//metadoc SoundTouch category Audio
/*metadoc SoundTouch description
	Used to change the tempo and/or pitch of an audio stream. 
	Input and output are in 32 bit floats in 2 channels at a rate 
	of 44100 samples per second.
*/

#include "IoSoundTouch.h"
#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define DATA(self) ((IoSoundTouchData *)IoObject_dataPointer(self))
static const char *protoId = "SoundTouch";

IoTag *IoSoundTouch_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSoundTouch_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSoundTouch_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSoundTouch_free);
	return tag;
}

IoSoundTouch *IoSoundTouch_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSoundTouch_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoSoundTouchData)));

	DATA(self)->inputBuffer  = IoSeq_new(state);
	DATA(self)->outputBuffer = IoSeq_new(state);

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"start", IoSoundTouch_start},
		{"process", IoSoundTouch_process},
		{"stop",  IoSoundTouch_stop},

		{"setSampleRate",  IoSoundTouch_setSampleRate},
		{"setChannels",  IoSoundTouch_setChannels},
		{"setTempoChange",  IoSoundTouch_setTempoChange},
		{"setTempo",  IoSoundTouch_setTempo},
		{"setPitchSemitones",  IoSoundTouch_setPitchSemitones},

		{"inputBuffer",  IoSoundTouch_inputBuffer},
		{"outputBuffer",  IoSoundTouch_outputBuffer},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoSoundTouch *IoSoundTouch_rawClone(IoSoundTouch *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoSoundTouchData)));

	DATA(self)->inputBuffer  = IoSeq_new(IOSTATE);
	DATA(self)->outputBuffer = IoSeq_new(IOSTATE);

	return self;
}

IoSoundTouch *IoSoundTouch_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

void *IoSoundTouch_soundTouch(IoSoundTouch *self)
{
	if (DATA(self)->soundTouch == NULL)
	{
		DATA(self)->soundTouch = SoundTouch_init();
	}

	return DATA(self)->soundTouch;
}

void IoSoundTouch_freeSoundTouchIfNeeded(IoSoundTouch *self)
{
	if (DATA(self)->soundTouch)
	{
		SoundTouch_free(DATA(self)->soundTouch);
	}
}

void IoSoundTouch_free(IoSoundTouch *self)
{
	IoSoundTouch_freeSoundTouchIfNeeded(self);
	free(IoObject_dataPointer(self));
}

void IoSoundTouch_mark(IoSoundTouch *self)
{
	IoObject_shouldMark(DATA(self)->inputBuffer);
	IoObject_shouldMark(DATA(self)->outputBuffer);
}

/* ----------------------------------------------------------- */


IoObject *IoSoundTouch_process(IoSoundTouch *self, IoObject *locals, IoMessage *m)
{
	/*doc SoundTouch process
	Processes a chunk of the inputBuffer and appends the results to the outputBuffer.
	*/
	
	 void *soundTouch = IoSoundTouch_soundTouch(self);

	 UArray *inba  = IoSeq_rawUArray(DATA(self)->inputBuffer);
	 UArray *outba = IoSeq_rawUArray(DATA(self)->outputBuffer);

	 size_t inSize = UArray_size(inba);
	 size_t oldOutSize = UArray_size(outba);
	 size_t sampleSize = 2 * sizeof(float);
	 size_t inSamples = inSize / sampleSize;
	 unsigned int outSamples;

	 UArray_setSize_(outba, oldOutSize + inSize);

	 SoundTouch_putSamples(soundTouch, (float *)(UArray_bytes(inba)), inSamples);
	 outSamples = SoundTouch_receiveSamples(soundTouch, (float *)(UArray_bytes(inba) + oldOutSize), inSamples);

	 UArray_removeRange(inba, 0, outSamples * sampleSize);
	 UArray_setSize_(outba, oldOutSize + (outSamples * sampleSize));

	 return self;
}

IoObject *IoSoundTouch_start(IoSoundTouch *self, IoObject *locals, IoMessage *m)
{
	/*doc SoundTouch start
	Initializes SoundTouch.
	*/
	
	IoSoundTouch_soundTouch(self);
	return self;
}

IoObject *IoSoundTouch_stop(IoSoundTouch *self, IoObject *locals, IoMessage *m)
{
	/*doc SoundTouch start
	Cleans up SoundTouch.
	*/
	
	IoSoundTouch_freeSoundTouchIfNeeded(self);
	return self;
}

// --- options -------------------------------------------------

IoObject *IoSoundTouch_setSampleRate(IoSoundTouch *self, IoObject *locals, IoMessage *m)
{
	/*doc SoundTouch setSampleRate(aNumber)
	Sets the input sample rate in Hz.
	*/
	
	void *soundTouch = IoSoundTouch_soundTouch(self);
	IoNumber *r = IoMessage_locals_numberArgAt_(m, locals, 0);
	SoundTouch_setSampleRate(soundTouch, (unsigned int)CNUMBER(r));
	return self;
}

IoObject *IoSoundTouch_setChannels(IoSoundTouch *self, IoObject *locals, IoMessage *m)
{
	/*doc SoundTouch setChannels(aNumber)
	Sets the number of input channels.
	*/
	
	void *soundTouch = IoSoundTouch_soundTouch(self);
	IoNumber *r = IoMessage_locals_numberArgAt_(m, locals, 0);
	SoundTouch_setChannels(soundTouch, (unsigned int)CNUMBER(r));
	return self;
}

IoObject *IoSoundTouch_setTempoChange(IoSoundTouch *self, IoObject *locals, IoMessage *m)
{
	/*doc SoundTouch setTempoChange(aNumber)
	Sets the tempo change amount.
	*/
	
	void *soundTouch = IoSoundTouch_soundTouch(self);
	IoNumber *r = IoMessage_locals_numberArgAt_(m, locals, 0);
	SoundTouch_setTempoChange(soundTouch, CNUMBER(r));
	return self;
}

IoObject *IoSoundTouch_setTempo(IoSoundTouch *self, IoObject *locals, IoMessage *m)
{
	/*doc SoundTouch setTempo(aNumber)
	Sets the tempo.
	*/
	
	void *soundTouch = IoSoundTouch_soundTouch(self);
	IoNumber *r = IoMessage_locals_numberArgAt_(m, locals, 0);
	SoundTouch_setTempo(soundTouch, (float)CNUMBER(r));
	return self;
}

IoObject *IoSoundTouch_setPitchSemitones(IoSoundTouch *self, IoObject *locals, IoMessage *m)
{
	/*doc SoundTouch setPitchSemitones(aNumber)
	Sets the output increase in pitch semitones.
	*/
	
	void *soundTouch = IoSoundTouch_soundTouch(self);
	IoNumber *r = IoMessage_locals_numberArgAt_(m, locals, 0);
	SoundTouch_setPitchSemiTones(soundTouch, (float)CNUMBER(r));
	return self;
}

IoObject *IoSoundTouch_inputBuffer(IoSoundTouch *self, IoObject *locals, IoMessage *m)
{
	/*doc SoundTouch inputBuffer
	Returns the input buffer.
	*/
	
	return DATA(self)->inputBuffer;
}

// --- buffers -------------------------------------------------------


IoObject *IoSoundTouch_outputBuffer(IoSoundTouch *self, IoObject *locals, IoMessage *m)
{
	/*doc SoundTouch outputBuffer
	Returns the output buffer.
	*/
	
	return DATA(self)->outputBuffer;
}
