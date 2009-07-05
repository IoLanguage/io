//metadoc VorbisInfo copyright Chris Double, 2004
//metadoc VorbisInfo license BSD revised
//metadoc VorbisInfo category Multimedia
/*metadoc VorbisInfo description
A wrapper around the libvorbis vorbis_info object.
*/

#include "IoVorbisInfo.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <vorbis/codec.h>
#include <errno.h>
#include <assert.h>

#define DATA(self) ((vorbis_info*)(IoObject_dataPointer(self)))

IoObject *IoMessage_locals_vorbisInfoArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISVORBISINFO(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "VorbisInfo");
  return v;
}

IoTag *IoVorbisInfo_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("VorbisInfo");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoVorbisInfo_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoVorbisInfo_rawClone);
	return tag;
}

IoVorbisInfo *IoVorbisInfo_proto(void *state)
{
	IoVorbisInfo *self = IoObject_new(state);
	IoObject_tag_(self, IoVorbisInfo_newTag(state));

	vorbis_info* data = calloc(1, sizeof(vorbis_info));
        vorbis_info_init(data);
	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithFunc_(state, self, IoVorbisInfo_proto);

	{
		IoMethodTable methodTable[] = {
		{"version", IoVorbisInfo_version},
		{"channels", IoVorbisInfo_channels},
		{"rate", IoVorbisInfo_rate},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoVorbisInfo *IoVorbisInfo_rawClone(IoVorbisInfo *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	vorbis_info* data = calloc(1, sizeof(vorbis_info));
        vorbis_info_init(data);
	IoObject_setDataPointer_(self, data);
	return self;
}

IoVorbisInfo *IoVorbisInfo_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoVorbisInfo_proto);
	return IOCLONE(proto);
}

void IoVorbisInfo_free(IoVorbisInfo *self)
{
        vorbis_info_clear(DATA(self));
	free(DATA(self));
}

/* ----------------------------------------------------------- */

IoObject *IoVorbisInfo_version(IoVorbisInfo *self, IoObject *locals, IoMessage *m)
{
	/*doc VorbisInfo version
	Returns the vorbis version required for this data.
	*/
	return IONUMBER(DATA(self)->version);
}

IoObject *IoVorbisInfo_channels(IoVorbisInfo *self, IoObject *locals, IoMessage *m)
{
	/*doc VorbisInfo channels
	Returns the number of channels in the vorbis data.
	*/
	return IONUMBER(DATA(self)->channels);
}

IoObject *IoVorbisInfo_rate(IoVorbisInfo *self, IoObject *locals, IoMessage *m)
{
	/*doc VorbisInfo rate
	Returns the sample rate of the vorbis data.
	*/
	return IONUMBER(DATA(self)->rate);
}

