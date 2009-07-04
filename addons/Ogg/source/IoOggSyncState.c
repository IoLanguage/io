//metadoc OggSyncState copyright Chris Double, 2004
//metadoc OggSyncState license BSD revised
//metadoc OggSyncState category Multimedia
/*metadoc OggSyncState description
A wrapper around the libogg ogg_sync_state object.
*/

#include "IoOggSyncState.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoMessage.h"
#include "IoSeq.h"
#include "IoOggPage.h"
#include <errno.h>
#include <assert.h>

#define DATA(self) ((ogg_sync_state*)(IoObject_dataPointer(self)))

IoObject *IoMessage_locals_oggSyncStateArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISOGGSYNCSTATE(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "OggSyncState");
  return v;
}

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
		{"reset", IoOggSyncState_reset},
		{"write", IoOggSyncState_write},
		{"pageseek", IoOggSyncState_pageseek},
		{"pageout", IoOggSyncState_pageout},
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

IoObject *IoOggSyncState_reset(IoOggSyncState *self, IoObject *locals, IoMessage *m)
{
	/*doc OggSyncState reset
	Resets the synchronization status to initial values.
	*/

	int ret = ogg_sync_reset(DATA(self));	
	IOASSERT(ret == 0, "ogg_sync_reset returned non-zero value");

	return self;
}

IoObject *IoOggSyncState_write(IoOggSyncState *self, IoObject *locals, IoMessage *m)
{
	/*doc OggSyncState write(seq)
        Copies the data from the sequence into the synchronisation
	layer.
	*/

        IoSeq *bufferSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
        unsigned char const* source = IOSEQ_BYTES(bufferSeq);
        size_t size = IOSEQ_LENGTH(bufferSeq);

        char* dest = ogg_sync_buffer(DATA(self), size);       
	IOASSERT(dest, "ogg_sync_buffer could not allocate memory");
        
	memcpy(dest, source, size);
        int ret = ogg_sync_wrote(DATA(self), size);	
	IOASSERT(ret == 0, "ogg_sync_write returned non-zero value");

	return self;
}

IoObject *IoOggSyncState_pageseek(IoOggSyncState *self, IoObject *locals, IoMessage *m)
{
	/*doc OggSyncState pageseek(page)
	Finds the borders of pages and resynchronises the stream.
	*/

        IoOggPage *page = IoMessage_locals_oggPageArgAt_(m, locals, 0);
	int ret = ogg_sync_pageseek(DATA(self), (ogg_page*)(IoObject_dataPointer(page)));

	return IONUMBER(ret);
}

IoObject *IoOggSyncState_pageout(IoOggSyncState *self, IoObject *locals, IoMessage *m)
{
	/*doc OggSyncState pageout(page)
	Outputs a page from the synchronisation layer.
	*/

        IoOggPage *page = IoMessage_locals_oggPageArgAt_(m, locals, 0);
	int ret = ogg_sync_pageout(DATA(self), (ogg_page*)(IoObject_dataPointer(page)));

	return IONUMBER(ret);
}

