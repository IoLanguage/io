//metadoc VorbisComment copyright Chris Double, 2004
//metadoc VorbisComment license BSD revised
//metadoc VorbisComment category Multimedia
/*metadoc VorbisComment description
A wrapper around the libvorbis vorbis_comment object.
*/

#include "IoVorbisComment.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <vorbis/codec.h>
#include <errno.h>
#include <assert.h>

#define DATA(self) ((vorbis_comment*)(IoObject_dataPointer(self)))

IoObject *IoMessage_locals_vorbisCommentArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISVORBISCOMMENT(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "VorbisComment");
  return v;
}

IoTag *IoVorbisComment_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("VorbisComment");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoVorbisComment_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoVorbisComment_rawClone);
	return tag;
}

IoVorbisComment *IoVorbisComment_proto(void *state)
{
	IoVorbisComment *self = IoObject_new(state);
	IoObject_tag_(self, IoVorbisComment_newTag(state));

	vorbis_comment* data = calloc(1, sizeof(vorbis_comment));
        vorbis_comment_init(data);
	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithFunc_(state, self, IoVorbisComment_proto);

	{
		IoMethodTable methodTable[] = {
		{"count", IoVorbisComment_count},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoVorbisComment *IoVorbisComment_rawClone(IoVorbisComment *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	vorbis_comment* data = calloc(1, sizeof(vorbis_comment));
        vorbis_comment_init(data);
	IoObject_setDataPointer_(self, data);
	return self;
}

IoVorbisComment *IoVorbisComment_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoVorbisComment_proto);
	return IOCLONE(proto);
}

void IoVorbisComment_free(IoVorbisComment *self)
{
        vorbis_comment_clear(DATA(self));
	free(DATA(self));
}

/* ----------------------------------------------------------- */

IoObject *IoVorbisComment_count(IoVorbisComment *self, IoObject *locals, IoMessage *m)
{
	/*doc VorbisComment count
	Returns number of comments.
	*/
	return IONUMBER(DATA(self)->comments);
}
