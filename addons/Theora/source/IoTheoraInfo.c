//metadoc TheoraInfo copyright Chris Double, 2004
//metadoc TheoraInfo license BSD revised
//metadoc TheoraInfo category Video
/*metadoc TheoraInfo description
A wrapper around the libtheora th_info object.
*/

#include "IoTheoraInfo.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <theora/theora.h>
#include <theora/theoradec.h>
#include <errno.h>
#include <assert.h>

#define DATA(self) ((th_info*)(IoObject_dataPointer(self)))

static const char *protoId = "TheoraInfo";

IoObject *IoMessage_locals_theoraInfoArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISTHEORAINFO(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, protoId);
  return v;
}

IoTag *IoTheoraInfo_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoTheoraInfo_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoTheoraInfo_rawClone);
	return tag;
}

IoTheoraInfo *IoTheoraInfo_proto(void *state)
{
	IoTheoraInfo *self = IoObject_new(state);
	IoObject_tag_(self, IoTheoraInfo_newTag(state));

	th_info* data = calloc(1, sizeof(th_info));
        th_info_init(data);
	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"frameWidth", IoTheoraInfo_frameWidth},
		{"frameHeight", IoTheoraInfo_frameHeight},
		{"frameRate", IoTheoraInfo_frameRate},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoTheoraInfo *IoTheoraInfo_rawClone(IoTheoraInfo *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	th_info* data = calloc(1, sizeof(th_info));
        th_info_init(data);
	IoObject_setDataPointer_(self, data);
	return self;
}

IoTheoraInfo *IoTheoraInfo_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoTheoraInfo_free(IoTheoraInfo *self)
{
	free(DATA(self));
}

/* ----------------------------------------------------------- */

IoObject *IoTheoraInfo_frameWidth(IoTheoraInfo *self, IoObject *locals, IoMessage *m)
{
	/*doc TheoraInfo frameWidth
	The encoded frame width.
	*/
	return IONUMBER(DATA(self)->frame_width);
}

IoObject *IoTheoraInfo_frameHeight(IoTheoraInfo *self, IoObject *locals, IoMessage *m)
{
	/*doc TheoraInfo frameHeight
	The encoded frame height.
	*/
	return IONUMBER(DATA(self)->frame_height);
}

IoObject *IoTheoraInfo_frameRate(IoTheoraInfo *self, IoObject *locals, IoMessage *m)
{
	/*doc TheoraInfo frameRate
	  The framerate of the video.
	*/
       float framerate = (float)(DATA(self)->fps_numerator) / (float)(DATA(self)->fps_denominator);

       return IONUMBER(framerate);
}
