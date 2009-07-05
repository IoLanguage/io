//metadoc TheoraDecodeContext copyright Chris Double, 2004
//metadoc TheoraDecodeContext license BSD revised
//metadoc TheoraDecodeContext category Multimedia
/*metadoc TheoraDecodeContext description
A wrapper around the libtheora th_dec_ctx object.
*/

#include "IoTheoraDecodeContext.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <theora/theora.h>
#include <theora/theoradec.h>
#include <errno.h>
#include <assert.h>

#define DATA(self) ((th_dec_ctx*)(IoObject_dataPointer(self)))

IoObject *IoMessage_locals_theoraDecodeContextArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISTHEORADECODECONTEXT(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "TheoraDecodeContext");
  return v;
}

IoTag *IoTheoraDecodeContext_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("TheoraDecodeContext");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoTheoraDecodeContext_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoTheoraDecodeContext_rawClone);
	return tag;
}

IoTheoraDecodeContext *IoTheoraDecodeContext_proto(void *state)
{
	IoTheoraDecodeContext *self = IoObject_new(state);
	IoObject_tag_(self, IoTheoraDecodeContext_newTag(state));

	IoObject_setDataPointer_(self, 0);

	IoState_registerProtoWithFunc_(state, self, IoTheoraDecodeContext_proto);

	{
		IoMethodTable methodTable[] = {
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoTheoraDecodeContext *IoTheoraDecodeContext_rawClone(IoTheoraDecodeContext *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, 0);
	return self;
}

IoTheoraDecodeContext *IoTheoraDecodeContext_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoTheoraDecodeContext_proto);
	return IOCLONE(proto);
}

void IoTheoraDecodeContext_free(IoTheoraDecodeContext *self)
{
        th_decode_free(DATA(self));
	free(DATA(self));
}

/* ----------------------------------------------------------- */

