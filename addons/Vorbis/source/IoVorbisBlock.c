//metadoc VorbisBlock copyright Chris Double, 2004
//metadoc VorbisBlock license BSD revised
//metadoc VorbisBlock category Video
/*metadoc VorbisBlock description
A wrapper around the libvorbis vorbis_comment object.
*/

#include "IoVorbisBlock.h"
#include "IoVorbisDspState.h"
#include "IoOggPacket.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <vorbis/codec.h>
#include <errno.h>
#include <assert.h>

#define DATA(self) ((vorbis_block*)(IoObject_dataPointer(self)))

IoObject *IoMessage_locals_vorbisBlockArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISVORBISBLOCK(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "VorbisBlock");
  return v;
}

IoTag *IoVorbisBlock_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("VorbisBlock");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoVorbisBlock_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoVorbisBlock_rawClone);
	return tag;
}

IoVorbisBlock *IoVorbisBlock_proto(void *state)
{
	IoVorbisBlock *self = IoObject_new(state);
	IoObject_tag_(self, IoVorbisBlock_newTag(state));

	vorbis_block* data = calloc(1, sizeof(vorbis_block));
	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithFunc_(state, self, IoVorbisBlock_proto);

	{
		IoMethodTable methodTable[] = {
		{"setup", IoVorbisBlock_setup},
		{"synthesis", IoVorbisBlock_synthesis},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoVorbisBlock *IoVorbisBlock_rawClone(IoVorbisBlock *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	vorbis_block* data = calloc(1, sizeof(vorbis_block));
	IoObject_setDataPointer_(self, data);
	return self;
}

IoVorbisBlock *IoVorbisBlock_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoVorbisBlock_proto);
	return IOCLONE(proto);
}

void IoVorbisBlock_free(IoVorbisBlock *self)
{
        vorbis_block_clear(DATA(self));
	free(DATA(self));
}

/* ----------------------------------------------------------- */

IoObject *IoVorbisBlock_setup(IoVorbisDspState *self, IoObject *locals, IoMessage *m)
{
	/*doc VorbisBlock setup
	Initialize for decoding using the information obtained
	from reading the Vorbis headers.
	*/
        IoVorbisDspState *dsp = IoMessage_locals_vorbisDspStateArgAt_(m, locals, 0);
	int ret = vorbis_block_init(((vorbis_dsp_state*)(IoObject_dataPointer(dsp))), DATA(self));
	IOASSERT(ret == 0, "vorbis_synthesis_init returned non-zero value");

	return self;
}

IoObject *IoVorbisBlock_synthesis(IoVorbisDspState *self, IoObject *locals, IoMessage *m)
{
	/*doc VorbisBlock synthesis(packet)
	Decode the vorbis data from the packet, storing it in the
	block.
	*/
        IoOggPacket *packet = IoMessage_locals_oggPacketArgAt_(m, locals, 0);
	int ret = vorbis_synthesis(DATA(self), ((ogg_packet*)(IoObject_dataPointer(packet))));

	return IONUMBER(ret);
}
