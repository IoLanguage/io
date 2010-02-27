//metadoc TheoraSetupInfo copyright Chris Double, 2004
//metadoc TheoraSetupInfo license BSD revised
//metadoc TheoraSetupInfo category Video
/*metadoc TheoraSetupInfo description
A wrapper around the libtheora th_setup_info object.
*/

#include "IoTheoraSetupInfo.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <theora/theora.h>
#include <theora/theoradec.h>
#include <errno.h>
#include <assert.h>

#define DATA(self) ((th_setup_info**)(IoObject_dataPointer(self)))

IoObject *IoMessage_locals_theoraSetupInfoArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject* v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISTHEORASETUPINFO(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "TheoraSetupInfo");
  return v;
}

IoTag *IoTheoraSetupInfo_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("TheoraSetupInfo");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoTheoraSetupInfo_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoTheoraSetupInfo_rawClone);
	return tag;
}

IoTheoraSetupInfo *IoTheoraSetupInfo_proto(void *state)
{
	IoTheoraSetupInfo *self = IoObject_new(state);
	IoObject_tag_(self, IoTheoraSetupInfo_newTag(state));

	th_setup_info** data = calloc(1, sizeof(th_setup_info*));
	IoObject_setDataPointer_(self, data);

	IoState_registerProtoWithFunc_(state, self, IoTheoraSetupInfo_proto);

	{
		IoMethodTable methodTable[] = {
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoTheoraSetupInfo *IoTheoraSetupInfo_rawClone(IoTheoraSetupInfo *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	th_setup_info** data = calloc(1, sizeof(th_setup_info*));
	IoObject_setDataPointer_(self, data);
	return self;
}

IoTheoraSetupInfo *IoTheoraSetupInfo_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoTheoraSetupInfo_proto);
	return IOCLONE(proto);
}

void IoTheoraSetupInfo_free(IoTheoraSetupInfo *self)
{
        th_setup_info** data = DATA(self);
        th_setup_free(*data);
	free(DATA(self));
}

/* ----------------------------------------------------------- */

