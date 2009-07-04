//metadoc OggPage copyright Chris Double, 2004
//metadoc OggPage license BSD revised
//metadoc OggPage category Multimedia
/*metadoc OggPage description
A wrapper around the libogg ogg_sync_state object.
*/

#include "IoOggPage.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <errno.h>
#include <assert.h>

#define DATA(self) ((ogg_page*)(IoObject_dataPointer(self)))

IoObject *IoMessage_locals_oggPageArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISOGGPAGE(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "OggPage");
  return v;
}

IoTag *IoOggPage_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("OggPage");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoOggPage_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoOggPage_rawClone);
	return tag;
}

IoOggPage *IoOggPage_proto(void *state)
{
	IoOggPage *self = IoObject_new(state);
	IoObject_tag_(self, IoOggPage_newTag(state));

	ogg_sync_state* data = calloc(1, sizeof(ogg_page));
	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithFunc_(state, self, IoOggPage_proto);

	{
		IoMethodTable methodTable[] = {
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoOggPage *IoOggPage_rawClone(IoOggPage *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	ogg_page* data = calloc(1, sizeof(ogg_page));
	IoObject_setDataPointer_(self, data);
	return self;
}

IoOggPage *IoOggPage_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoOggPage_proto);
	return IOCLONE(proto);
}

void IoOggPage_free(IoOggPage *self)
{
	free(DATA(self));
}

/* ----------------------------------------------------------- */

