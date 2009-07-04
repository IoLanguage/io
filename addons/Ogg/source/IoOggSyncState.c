//metadoc OggSyncState copyright Chris Double, 2004
//metadoc OggSyncState license BSD revised
//metadoc OggSyncState category Multimedia
/*metadoc OggSyncState description
A wrapper around the libogg ogg_sync_state object.
*/

#include "IoOggSyncState.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <errno.h>
#include <assert.h>

#define DATA(self) ((ogg_sync_state*)(IoObject_dataPointer(self)))

IoTag *IoOggSyncState_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("OggSyncState");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoOggSyncState_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoOggSyncState_rawClone);
	return tag;
}

IoOggSyncState *IoOggSyncState_proto(void *state)
{
	int r = -1;
	IoOggSyncState *self = IoObject_new(state);
	IoObject_tag_(self, IoOggSyncState_newTag(state));

	ogg_sync_state* data = calloc(1, sizeof(ogg_sync_state));
	r = ogg_sync_init(data);
	assert(r == 0);

	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithFunc_(state, self, IoOggSyncState_proto);

	{
		IoMethodTable methodTable[] = {
		{"clear", IoOggSyncState_clear},
		/*
		{"reset", IoZlibDecoder_reset},
		{"check", IoZlibDecoder_check},
		{"buffer", IoZlibDecoder_buffer},
		{"wrote", IoZlibDecoder_wrote},
		{"pageseek", IoZlibDecoder_pageseek},
		{"pageout", IoZlibDecoder_pageout},
		*/
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoOggSyncState *IoOggSyncState_rawClone(IoOggSyncState *proto)
{
	int r = -1;
	IoObject *self = IoObject_rawClonePrimitive(proto);
	ogg_sync_state* data = calloc(1, sizeof(ogg_sync_state));
	r = ogg_sync_init(data);
	assert(r == 0);
	IoObject_setDataPointer_(self, data);
	return self;
}

IoOggSyncState *IoOggSyncState_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoOggSyncState_proto);
	return IOCLONE(proto);
}

void IoOggSyncState_free(IoOggSyncState *self)
{
	int r = ogg_sync_clear(DATA(self));
	assert(r == 0);
	free(DATA(self));
}

/* ----------------------------------------------------------- */

IoObject *IoOggSyncState_clear(IoOggSyncState *self, IoObject *locals, IoMessage *m)
{
	/*doc OggSyncState clear
	Free's any internal storage and resets to the initial state.
	*/

	int ret = ogg_sync_clear(DATA(self));	
	IOASSERT(ret == 0, "ogg_sync_clear returned non-zero value");

	return self;
}

