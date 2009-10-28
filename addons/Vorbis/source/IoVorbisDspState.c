//metadoc VorbisDspState copyright Chris Double, 2004
//metadoc VorbisDspState license BSD revised
//metadoc VorbisDspState category Multimedia
/*metadoc VorbisDspState description
A wrapper around the libvorbis vorbis_comment object.
*/

#include "IoVorbisDspState.h"
#include "IoVorbisInfo.h"
#include "IoVorbisComment.h"
#include "IoVorbisBlock.h"
#include "IoOggPacket.h"
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
		{"setup", IoVorbisDspState_setup},
		{"headerin", IoVorbisDspState_headerin},
		{"blockin", IoVorbisDspState_blockin},
		{"pcmout", IoVorbisDspState_pcmout},
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

IoObject *IoVorbisDspState_setup(IoVorbisDspState *self, IoObject *locals, IoMessage *m)
{
	/*doc VorbisDspState setup(info)
	Initialize for decoding using the information obtained
	from reading the Vorbis headers.
	*/
        IoVorbisInfo *info = IoMessage_locals_vorbisInfoArgAt_(m, locals, 0);
	int ret = vorbis_synthesis_init(DATA(self), ((vorbis_info*)(IoObject_dataPointer(info))));
	IOASSERT(ret == 0, "vorbis_synthesis_init returned non-zero value");

	return self;
}

IoObject *IoVorbisDspState_headerin(IoVorbisDspState *self, IoObject *locals, IoMessage *m)
{
	/*doc VorbisDspState headerin(info, comment, packet)
	Try to decode a vorbis header from the packet.
	*/
        IoVorbisInfo *info = IoMessage_locals_vorbisInfoArgAt_(m, locals, 0);
        IoVorbisComment *comment = IoMessage_locals_vorbisCommentArgAt_(m, locals, 1);
        IoOggPacket *packet = IoMessage_locals_oggPacketArgAt_(m, locals, 2);
	int ret = vorbis_synthesis_headerin(((vorbis_info*)(IoObject_dataPointer(info))),
					    ((vorbis_comment*)(IoObject_dataPointer(comment))),
					    ((ogg_packet*)(IoObject_dataPointer(packet))));

	return IONUMBER(ret);
}

IoObject *IoVorbisDspState_blockin(IoVorbisDspState *self, IoObject *locals, IoMessage *m)
{
	/*doc VorbisDspState blockin(block)
	Decodes that data from the block, storing it in the dsp state.
	*/
        IoVorbisBlock *block = IoMessage_locals_vorbisBlockArgAt_(m, locals, 0);
	int ret = vorbis_synthesis_blockin(DATA(self), ((vorbis_block*)(IoObject_dataPointer(block))));
	IOASSERT(ret == 0, "vorbis_synthesis_blockin returned non-zero value");

	return self;
}

IoObject *IoVorbisDspState_pcmout(IoVorbisDspState *self, IoObject *locals, IoMessage *m)
{
	/*doc VorbisDspState pcmout
	Returns array of audio data
	*/
        float** pcm;
	int samples = vorbis_synthesis_pcmout(DATA(self), &pcm);
	float sound[samples * 2];
	float* p = sound;
	int i = 0;
	int j = 0;
	for (i=0; i < samples; i++)
	  for (j=0; j < 2; j++)
	    *p++ = pcm[j][i];

	IoObject* data = IOSEQ((const unsigned char*)sound, samples * sizeof(float) * 2);
	//int ret = 
	vorbis_synthesis_read(DATA(self), samples);

	return data;
}

