//metadoc OggPage copyright Chris Double, 2004
//metadoc OggPage license BSD revised
//metadoc OggPage category Audio
/*metadoc OggPage description
A wrapper around the libogg ogg_page object.
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

	ogg_page* data = calloc(1, sizeof(ogg_page));
	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithFunc_(state, self, IoOggPage_proto);

	{
		IoMethodTable methodTable[] = {
		{"version", IoOggPage_version},
		{"continued", IoOggPage_continued},
		{"packets", IoOggPage_packets},
		{"bos", IoOggPage_bos},
		{"eos", IoOggPage_eos},
		{"granulepos", IoOggPage_granulepos},
		{"serialno", IoOggPage_serialno},
		{"pageno", IoOggPage_pageno},
		{"setChecksum", IoOggPage_setChecksum},
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

IoObject *IoOggPage_version(IoOggPage *self, IoObject *locals, IoMessage *m)
{
	/*doc OggPage version
	Returns the ogg_page version that this page uses.
	*/
	int ret = ogg_page_version(DATA(self));	

	return IONUMBER(ret);
}

IoObject *IoOggPage_continued(IoOggPage *self, IoObject *locals, IoMessage *m)
{
	/*doc OggPage continued
	Indicates if the current page contains a continued
        packet from the last page.
	*/
	int ret = ogg_page_continued(DATA(self));	

	return ret ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoOggPage_packets(IoOggPage *self, IoObject *locals, IoMessage *m)
{
	/*doc OggPage packets
	Returns the number of packets in the page.
	*/
	int ret = ogg_page_packets(DATA(self));	

	return IONUMBER(ret);
}

IoObject *IoOggPage_bos(IoOggPage *self, IoObject *locals, IoMessage *m)
{
	/*doc OggPage bos
	Indicates if the current page is the beginning of the stream.
	*/
	int ret = ogg_page_bos(DATA(self));	

	return ret ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoOggPage_eos(IoOggPage *self, IoObject *locals, IoMessage *m)
{
	/*doc OggPage eos
	Indicates if the current page is the end of the stream.
	*/
	int ret = ogg_page_eos(DATA(self));	

	return ret ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoOggPage_granulepos(IoOggPage *self, IoObject *locals, IoMessage *m)
{
	/*doc OggPage granulepos
	Returns the precise playback location of this page.
	*/
	int ret = ogg_page_granulepos(DATA(self));	

	return IONUMBER(ret);
}

IoObject *IoOggPage_serialno(IoOggPage *self, IoObject *locals, IoMessage *m)
{
	/*doc OggPage serialno
	Returns the unique serial number of the logical bitstream
        associated with this page.
	*/
	int ret = ogg_page_serialno(DATA(self));	

	return IONUMBER(ret);
}

IoObject *IoOggPage_pageno(IoOggPage *self, IoObject *locals, IoMessage *m)
{
	/*doc OggPage pageno
	Returns the sequential page number for this page.
	*/
	int ret = ogg_page_pageno(DATA(self));	

	return IONUMBER(ret);
}

IoObject *IoOggPage_setChecksum(IoOggPage *self, IoObject *locals, IoMessage *m)
{
	/*doc OggPage setChecksum
	Computes and sets the checksum for this page.
	*/
	ogg_page_checksum_set(DATA(self));	

	return self;
}
