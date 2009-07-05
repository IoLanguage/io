//metadoc OggPacket copyright Chris Double, 2004
//metadoc OggPacket license BSD revised
//metadoc OggPacket category Multimedia
/*metadoc OggPacket description
A wrapper around the libogg ogg_packet object.
*/

#include "IoOggPacket.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <errno.h>
#include <assert.h>

#define DATA(self) ((ogg_packet*)(IoObject_dataPointer(self)))

IoObject *IoMessage_locals_oggPacketArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISOGGPACKET(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "OggPacket");
  return v;
}

IoTag *IoOggPacket_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("OggPacket");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoOggPacket_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoOggPacket_rawClone);
	return tag;
}

IoOggPacket *IoOggPacket_proto(void *state)
{
	IoOggPacket *self = IoObject_new(state);
	IoObject_tag_(self, IoOggPacket_newTag(state));

	ogg_packet* data = calloc(1, sizeof(ogg_packet));
	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithFunc_(state, self, IoOggPacket_proto);

	{
		IoMethodTable methodTable[] = {
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoOggPacket *IoOggPacket_rawClone(IoOggPacket *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	ogg_packet* data = calloc(1, sizeof(ogg_packet));
	IoObject_setDataPointer_(self, data);
	return self;
}

IoOggPacket *IoOggPacket_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoOggPacket_proto);
	return IOCLONE(proto);
}

void IoOggPacket_free(IoOggPacket *self)
{
	free(DATA(self));
}

/* ----------------------------------------------------------- */

