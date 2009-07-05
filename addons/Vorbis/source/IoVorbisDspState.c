//metadoc VorbisDspState copyright Chris Double, 2004
//metadoc VorbisDspState license BSD revised
//metadoc VorbisDspState category Multimedia
/*metadoc VorbisDspState description
A wrapper around the libvorbis vorbis_comment object.
*/

#include "IoVorbisDspState.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <vorbis/codec.h>
#include <errno.h>
#include <assert.h>

#define DATA(self) ((vorbis_dsp_state*)(IoObject_dataPointer(self)))

IoObject *IoMessage_locals_vorbisDspStateArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISVORBISDSPSTATE(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "VorbisDspState");
  return v;
}

IoTag *IoVorbisDspState_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("VorbisDspState");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoVorbisDspState_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoVorbisDspState_rawClone);
	return tag;
}

IoVorbisDspState *IoVorbisDspState_proto(void *state)
{
	IoVorbisDspState *self = IoObject_new(state);
	IoObject_tag_(self, IoVorbisDspState_newTag(state));

	vorbis_dsp_state* data = calloc(1, sizeof(vorbis_dsp_state));
	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithFunc_(state, self, IoVorbisDspState_proto);

	{
		IoMethodTable methodTable[] = {
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoVorbisDspState *IoVorbisDspState_rawClone(IoVorbisDspState *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	vorbis_dsp_state* data = calloc(1, sizeof(vorbis_dsp_state));
	IoObject_setDataPointer_(self, data);
	return self;
}

IoVorbisDspState *IoVorbisDspState_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoVorbisDspState_proto);
	return IOCLONE(proto);
}

void IoVorbisDspState_free(IoVorbisDspState *self)
{
        vorbis_dsp_clear(DATA(self));
	free(DATA(self));
}

/* ----------------------------------------------------------- */

