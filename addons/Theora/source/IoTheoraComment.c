//metadoc TheoraComment copyright Chris Double, 2004
//metadoc TheoraComment license BSD revised
//metadoc TheoraComment category Multimedia
/*metadoc TheoraComment description
A wrapper around the libtheora th_comment object.
*/

#include "IoTheoraComment.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <theora/theora.h>
#include <theora/theoradec.h>
#include <errno.h>
#include <assert.h>

#define DATA(self) ((th_comment*)(IoObject_dataPointer(self)))

IoObject *IoMessage_locals_theoraCommentArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISTHEORACOMMENT(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "TheoraComment");
  return v;
}

IoTag *IoTheoraComment_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("TheoraComment");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoTheoraComment_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoTheoraComment_rawClone);
	return tag;
}

IoTheoraComment *IoTheoraComment_proto(void *state)
{
	IoTheoraComment *self = IoObject_new(state);
	IoObject_tag_(self, IoTheoraComment_newTag(state));

	th_comment* data = calloc(1, sizeof(th_comment));
        th_comment_init(data);
	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithFunc_(state, self, IoTheoraComment_proto);

	{
		IoMethodTable methodTable[] = {
		{"count", IoTheoraComment_count},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoTheoraComment *IoTheoraComment_rawClone(IoTheoraComment *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	th_comment* data = calloc(1, sizeof(th_comment));
        th_comment_init(data);
	IoObject_setDataPointer_(self, data);
	return self;
}

IoTheoraComment *IoTheoraComment_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoTheoraComment_proto);
	return IOCLONE(proto);
}

void IoTheoraComment_free(IoTheoraComment *self)
{
	free(DATA(self));
}

/* ----------------------------------------------------------- */

IoObject *IoTheoraComment_count(IoTheoraComment *self, IoObject *locals, IoMessage *m)
{
	/*doc TheoraComment count
	Returns the number of comments.
	*/
	return IONUMBER(DATA(self)->comments);
}

