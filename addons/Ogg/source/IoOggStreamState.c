//metadoc OggStreamState copyright Chris Double, 2004
//metadoc OggStreamState license BSD revised
//metadoc OggStreamState category Audio
/*metadoc OggStreamState description
A wrapper around the libogg ogg_stream_state object.
*/

#include "IoOggStreamState.h"
#include "IoOggPacket.h"
#include "IoOggPage.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <errno.h>
#include <assert.h>

#define DATA(self) ((ogg_stream_state*)(IoObject_dataPointer(self)))
static const char *protoId = "OggStreamState";

IoObject *IoMessage_locals_oggStreamStateArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISOGGSTREAMSTATE(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, protoId);
  return v;
}

IoTag *IoOggStreamState_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoOggStreamState_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoOggStreamState_rawClone);
	return tag;
}

IoOggStreamState *IoOggStreamState_proto(void *state)
{
	IoOggStreamState *self = IoObject_new(state);
	IoObject_tag_(self, IoOggStreamState_newTag(state));

	ogg_stream_state* data = calloc(1, sizeof(ogg_stream_state));
	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"setSerialNumber", IoOggStreamState_setSerialNumber},
		{"clear", IoOggStreamState_clear},
		{"reset", IoOggStreamState_clear},
		{"eos", IoOggStreamState_clear},
		{"pagein", IoOggStreamState_pagein},
		{"packetout", IoOggStreamState_packetout},
		{"packetpeek", IoOggStreamState_packetpeek},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoOggStreamState *IoOggStreamState_rawClone(IoOggStreamState *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	ogg_stream_state* data = calloc(1, sizeof(ogg_stream_state));
	IoObject_setDataPointer_(self, data);
	return self;
}

IoOggStreamState *IoOggStreamState_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoOggStreamState_free(IoOggStreamState *self)
{
        int r = ogg_stream_clear(DATA(self));
	assert(r == 0);
	free(DATA(self));
}

/* ----------------------------------------------------------- */
IoObject *IoOggStreamState_setSerialNumber(IoOggStreamState *self, IoObject *locals, IoMessage *m)
{
	/*doc OggStreamState setSerialNumber
	Sets the serial number for this stream.
	*/
        int serial = IoMessage_locals_intArgAt_(m, locals, 0);
	int ret = ogg_stream_init(DATA(self), serial);	
	IOASSERT(ret == 0, "ogg_stream_init returned non-zero value");

	return self;
}

IoObject *IoOggStreamState_clear(IoOggStreamState *self, IoObject *locals, IoMessage *m)
{
	/*doc OggStreamState clear
	Clears the storage within the Ogg stream.
	*/

	int ret = ogg_stream_clear(DATA(self));	
	IOASSERT(ret == 0, "ogg_stream_clear returned non-zero value");

	return self;
}

IoObject *IoOggStreamState_reset(IoOggStreamState *self, IoObject *locals, IoMessage *m)
{
	/*doc OggStreamState reset
	  Resets the stream status to its initial position.
	*/

	int ret = ogg_stream_reset(DATA(self));	
	IOASSERT(ret == 0, "ogg_stream_reset returned non-zero value");

	return self;
}

IoObject *IoOggStreamState_eos(IoOggStreamState *self, IoObject *locals, IoMessage *m)
{
	/*doc OggStreamState eos
	  Indicates whether we are at the end of the stream.
	*/

	int ret = ogg_stream_reset(DATA(self));	

	return ret == 0 ? IOFALSE(self) : IOTRUE(self);
}

IoObject *IoOggStreamState_pagein(IoOggStreamState *self, IoObject *locals, IoMessage *m)
{
	/*doc OggStreamState pagein(page)
	  Submits a complete page to the stream layer.
	*/

        IoOggPage *page = IoMessage_locals_oggPageArgAt_(m, locals, 0);
	int ret = ogg_stream_pagein(DATA(self), (ogg_page*)(IoObject_dataPointer(page)));	

	return IONUMBER(ret);
}

IoObject *IoOggStreamState_packetout(IoOggStreamState *self, IoObject *locals, IoMessage *m)
{
	/*doc OggStreamState packetout(packet)
	  Outputs a packet to the codec-specific decoding engine.
	*/

        IoOggPacket *packet = IoMessage_locals_oggPacketArgAt_(m, locals, 0);
	int ret = ogg_stream_packetout(DATA(self), (ogg_packet*)(IoObject_dataPointer(packet)));	

	return IONUMBER(ret);
}

IoObject *IoOggStreamState_packetpeek(IoOggStreamState *self, IoObject *locals, IoMessage *m)
{
	/*doc OggStreamState packetpeek(packet)
	  Provides access to the next packet in the bitstream
	  without advancing decoding.
	*/

        IoOggPacket *packet = IoMessage_locals_oggPacketArgAt_(m, locals, 0);
	int ret = ogg_stream_packetpeek(DATA(self), (ogg_packet*)(IoObject_dataPointer(packet)));	

	return IONUMBER(ret);
}

