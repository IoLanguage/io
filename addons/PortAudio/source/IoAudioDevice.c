
//metadoc AudioDevice copyright Steve Dekorte, 2004
//metadoc AudioDevice license BSD revised
//metadoc AudioDevice category Audio
//metadoc AudioDevice module PortAudio
/*metadoc AudioDevice description
The AudioDevice object can used to write audio data as if directly to the audio buffer.
*/

#include "IoAudioDevice.h"
#include "List.h"
#include "UArray.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "IoList.h"
#include <math.h>

#define DATA(self) ((IoAudioDeviceData *)IoObject_dataPointer(self))

static const char *protoId = "AudioDevice";

IoTag *IoAudioDevice_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoAudioDevice_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoAudioDevice_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoAudioDevice_mark);
	return tag;
}

IoAudioDevice *IoAudioDevice_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoAudioDevice_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoAudioDeviceData)));

	DATA(self)->writeBuffer = IoSeq_new(state);
	DATA(self)->readBuffer  = IoSeq_new(state);
	DATA(self)->audioDevice = AudioDevice_new();
	IoState_registerProtoWithId_(state, self, protoId);

	{
	IoMethodTable methodTable[] = {
	{"open", IoAudioDevice_open},
	{"openForReadingAndWriting", IoAudioDevice_openForReadingAndWriting},
	{"close", IoAudioDevice_close},
	{"asyncWrite", IoAudioDevice_asyncWrite},
	{"asyncRead", IoAudioDevice_read},
	{"error", IoAudioDevice_error},
	{"isActive", IoAudioDevice_isActive},
	{"streamTime", IoAudioDevice_streamTime},
	{"needsData", IoAudioDevice_needsData},
	{"writeBufferIsEmpty", IoAudioDevice_writeBufferIsEmpty},
	{NULL, NULL},
	};
	IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoAudioDevice *IoAudioDevice_rawClone(IoAudioDevice *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoAudioDeviceData)));
	DATA(self)->writeBuffer = IOCLONE(DATA(proto)->writeBuffer);
	DATA(self)->readBuffer = IOCLONE(DATA(proto)->readBuffer);
	DATA(self)->audioDevice = AudioDevice_new();
	return self;
}

IoAudioDevice *IoAudioDevice_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

void IoAudioDevice_free(IoAudioDevice *self)
{
	AudioDevice_free(DATA(self)->audioDevice);
	free(IoObject_dataPointer(self));
}

void IoAudioDevice_mark(IoAudioDevice *self)
{
	IoObject_shouldMark(DATA(self)->writeBuffer);
	IoObject_shouldMark(DATA(self)->readBuffer);
}

AudioDevice *IoAudioDevice_rawAudioDevice(IoAudioDevice *self)
{
	return DATA(self)->audioDevice;
}

void IoAudioDevice_clearBuffers(IoAudioDevice *self)
{
	AudioDevice_clearBuffers(DATA(self)->audioDevice);
}

/* ----------------------------------------------------------- */

IoObject *IoAudioDevice_openForReadingAndWriting(IoAudioDevice *self, IoObject *locals, IoMessage *m)
{
	/*doc AudioDevice openForReadingAndWriting
	Open audio output and input streams.
	*/

	AudioDevice_openForReadingAndWriting(DATA(self)->audioDevice);
	AudioDevice_start(DATA(self)->audioDevice);
	return self;
}

IoObject *IoAudioDevice_open(IoAudioDevice *self, IoObject *locals, IoMessage *m)
{
	/*doc AudioDevice open
	Open an audio output stream.
	*/

	AudioDevice_open(DATA(self)->audioDevice);
	AudioDevice_start(DATA(self)->audioDevice);
	return self;
}

IoObject *IoAudioDevice_close(IoAudioDevice *self, IoObject *locals, IoMessage *m)
{
	/*doc AudioDevice close
	Close the audio stream.
	*/

	AudioDevice_close(DATA(self)->audioDevice);
	return self;
}

IoObject *IoAudioDevice_needsData(IoAudioDevice *self, IoObject *locals, IoMessage *m)
{
	/*doc AudioDevice needsData
	Returns true if the receiver can read more data.
	*/
	
	return IOBOOL(self, DATA(self)->audioDevice->needsData == 1);
}

IoObject *IoAudioDevice_asyncWrite(IoAudioDevice *self, IoObject *locals, IoMessage *m)
{
	/*doc AudioDevice asyncWrite(aBuffer)
	Writes aBuffer to the audio output buffer and returns immediately.
	*/

	IoSeq *buf = IoMessage_locals_seqArgAt_(m, locals, 0);

	if (IoSeq_rawSize(buf) == 0)
	{
		return self;
	}

	AudioDevice_checkForError(DATA(self)->audioDevice);
	AudioDevice_write_(DATA(self)->audioDevice, IoSeq_rawUArray(buf));
	return self;
}

IoObject *IoAudioDevice_read(IoAudioDevice *self, IoObject *locals, IoMessage *m)
{
	/*doc AudioDevice asyncRead
	Returns the audio read buffer.
	*/

	AudioDevice *device = DATA(self)->audioDevice;
	UArray *rba = IoSeq_rawUArray(DATA(self)->readBuffer);
	UArray *ba;

	while (device->locked)
	{
		AudioDevice_nanoSleep(device);
	}

	ba = AudioDevice_read(device);
	UArray_copy_(rba, ba);

	return DATA(self)->readBuffer;
}

IoObject *IoAudioDevice_error(IoAudioDevice *self, IoObject *locals, IoMessage *m)
{
	/*doc AudioDevice error
	Returns an error string if an error has occurred, Nil otherwise.
	*/

	const char *e = AudioDevice_error(DATA(self)->audioDevice);
	return e ? IOSYMBOL(e) : IONIL(self);
}

IoObject *IoAudioDevice_isActive(IoAudioDevice *self, IoObject *locals, IoMessage *m)
{
	/*doc AudioDevice isActive
	Returns self if the receiver is active, Nil otherwise.
	*/

	return AudioDevice_isActive(DATA(self)->audioDevice) ? self : IONIL(self);
}

IoObject *IoAudioDevice_streamTime(IoAudioDevice *self, IoObject *locals, IoMessage *m)
{
	/*doc AudioDevice streamTime
	Returns the audio stream time as a number.
	*/

	return IONUMBER(AudioDevice_streamTime(DATA(self)->audioDevice));
}

IoObject *IoAudioDevice_writeBufferIsEmpty(IoAudioDevice *self, IoObject *locals, IoMessage *m)
{
	/*doc AudioDevice writeBufferIsEmpty
	Returns the true if the audio buffer is empty, false otherwise.
	*/
	
	return IOBOOL(self, DATA(self)->audioDevice->writeBufferIsEmpty);
}

