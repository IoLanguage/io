//metadoc TheoraDecodeContext copyright Chris Double, 2004
//metadoc TheoraDecodeContext license BSD revised
//metadoc TheoraDecodeContext category Video
/*metadoc TheoraDecodeContext description
A wrapper around the libtheora th_dec_ctx object.
*/

#include "IoTheoraDecodeContext.h"
#include "IoTheoraInfo.h"
#include "IoTheoraComment.h"
#include "IoTheoraSetupInfo.h"
#include "IoOggPacket.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "IoList.h"
#include <theora/theora.h>
#include <theora/theoradec.h>
#include <errno.h>
#include <assert.h>

typedef struct
{
	th_dec_ctx* ctx;
        ogg_int64_t granulepos;
} IoTheoraDecodeContextData;

#define DATA(self) ((IoTheoraDecodeContextData*)(IoObject_dataPointer(self)))

static const char *protoId = "TheoraDecodeContext";


IoObject *IoMessage_locals_theoraDecodeContextArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISTHEORADECODECONTEXT(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, protoId);
  return v;
}

IoTag *IoTheoraDecodeContext_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoTheoraDecodeContext_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoTheoraDecodeContext_rawClone);
	return tag;
}

IoTheoraDecodeContext *IoTheoraDecodeContext_proto(void *state)
{
	IoTheoraDecodeContext *self = IoObject_new(state);
	IoObject_tag_(self, IoTheoraDecodeContext_newTag(state));

	IoTheoraDecodeContextData* data = calloc(1, sizeof(IoTheoraDecodeContextData));
	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"setup", IoTheoraDecodeContext_setup},
		{"headerin", IoTheoraDecodeContext_headerin},
		{"packetin", IoTheoraDecodeContext_packetin},
		{"ycbcr", IoTheoraDecodeContext_ycbcr},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoTheoraDecodeContext *IoTheoraDecodeContext_rawClone(IoTheoraDecodeContext *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoTheoraDecodeContextData* data = calloc(1, sizeof(IoTheoraDecodeContextData));
	IoObject_setDataPointer_(self, data);
	return self;
}

IoTheoraDecodeContext *IoTheoraDecodeContext_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoTheoraDecodeContext_free(IoTheoraDecodeContext *self)
{
        th_decode_free(DATA(self)->ctx);
	free(DATA(self));
}

/* ----------------------------------------------------------- */

IoObject *IoTheoraDecodeContext_setup(IoTheoraDecodeContext *self, IoObject *locals, IoMessage *m)
{
	/*doc IoTheoraDecodeContext setup(info, setup)
	Initialize for decoding using the information obtained
	from reading the Theora headers.
	*/
        IoTheoraInfo *info = IoMessage_locals_theoraInfoArgAt_(m, locals, 0);
        IoTheoraSetupInfo *setup = IoMessage_locals_theoraSetupInfoArgAt_(m, locals, 1);
	th_dec_ctx* ctx = th_decode_alloc((th_info*)(IoObject_dataPointer(info)),
					  *((th_setup_info**)(IoObject_dataPointer(setup))));
	IOASSERT(ctx, "th_decode_alloc failed");

	DATA(self)->ctx = ctx;
	
	return self;
}

IoObject *IoTheoraDecodeContext_headerin(IoTheoraDecodeContext *self, IoObject *locals, IoMessage *m)
{
	/*doc TheoraDecodecontext headerin(info, comment, setup, packet)
	Try to decode a theora header from the packet.
	*/
        IoTheoraInfo *info = IoMessage_locals_theoraInfoArgAt_(m, locals, 0);
        IoTheoraComment *comment = IoMessage_locals_theoraCommentArgAt_(m, locals, 1);
        IoTheoraSetupInfo *setup = IoMessage_locals_theoraSetupInfoArgAt_(m, locals, 2);
        IoOggPacket *packet = IoMessage_locals_oggPacketArgAt_(m, locals, 3);
	int ret = th_decode_headerin(((th_info*)(IoObject_dataPointer(info))),
				     ((th_comment*)(IoObject_dataPointer(comment))),
				     ((th_setup_info**)(IoObject_dataPointer(setup))),
				     ((ogg_packet*)(IoObject_dataPointer(packet))));

	return IONUMBER(ret);
}

IoObject *IoTheoraDecodeContext_packetin(IoTheoraDecodeContext *self, IoObject *locals, IoMessage *m)
{
	/*doc TheoraDecodecontext packetin(packet)
	Try to decode a theora frame from the packet.
	*/
        IoOggPacket *packet = IoMessage_locals_oggPacketArgAt_(m, locals, 3);
	int ret = th_decode_packetin(DATA(self)->ctx,
				     ((ogg_packet*)(IoObject_dataPointer(packet))),
				     &(DATA(self)->granulepos));

	return IONUMBER(ret);
}

IoObject *IoTheoraDecodeContext_ycbcr(IoTheoraDecodeContext *self, IoObject *locals, IoMessage *m)
{
	/*doc TheoraDecodecontext ycbcr
	Returns an object containing the YUV data from the decoded frame.
	*/
        th_ycbcr_buffer buffer;
	int ret = th_decode_ycbcr_out(DATA(self)->ctx, buffer);
	IOASSERT(ret == 0, "th_decode_ycbcr_out failed");
	
	IoObject* yuv0 = IoObject_new(IOSTATE);
	IoObject* yuv1 = IoObject_new(IOSTATE);
	IoObject* yuv2 = IoObject_new(IOSTATE);
	IoObject_setSlot_to_(yuv0, IOSYMBOL("width"), IONUMBER(buffer[0].width));
	IoObject_setSlot_to_(yuv0, IOSYMBOL("height"), IONUMBER(buffer[0].height));
	IoObject_setSlot_to_(yuv0, IOSYMBOL("stride"), IONUMBER(buffer[0].stride));
	IoObject_setSlot_to_(yuv0, IOSYMBOL("data"), IOSEQ(buffer[0].data, buffer[0].stride * buffer[0].height));
	IoObject_setSlot_to_(yuv1, IOSYMBOL("width"), IONUMBER(buffer[1].width));
	IoObject_setSlot_to_(yuv1, IOSYMBOL("height"), IONUMBER(buffer[1].height));
	IoObject_setSlot_to_(yuv1, IOSYMBOL("stride"), IONUMBER(buffer[1].stride));
	IoObject_setSlot_to_(yuv1, IOSYMBOL("data"), IOSEQ(buffer[1].data, buffer[1].stride * buffer[1].height));
	IoObject_setSlot_to_(yuv2, IOSYMBOL("width"), IONUMBER(buffer[2].width));
	IoObject_setSlot_to_(yuv2, IOSYMBOL("height"), IONUMBER(buffer[2].height));
	IoObject_setSlot_to_(yuv2, IOSYMBOL("stride"), IONUMBER(buffer[2].stride));
	IoObject_setSlot_to_(yuv2, IOSYMBOL("data"), IOSEQ(buffer[2].data, buffer[2].stride * buffer[2].height));

	IoList* result = IoList_new(IOSTATE);
	IoList_rawAppend_(result, yuv0);
	IoList_rawAppend_(result, yuv1);
	IoList_rawAppend_(result, yuv2);
	return result;
}
