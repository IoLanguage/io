//metadoc CairoPath copyright Daniel Rosengren, 2007
//metadoc CairoPath license BSD revised
//metadoc CairoPath category Graphics

#include "IoCairoPath.h"
#include "IoCairoPathElement.h"
#include "IoList.h"
#include "tools.h"
#include <stdlib.h>

#define DATA(self) ((IoCairoPathData *)IoObject_dataPointer(self))
#define PATH(self) (DATA(self)->path)

static const char *protoId = "CairoPath";

void *IoMessage_locals_cairoPathArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *arg = IoMessage_locals_valueArgAt_(self, locals, n);
	if (!ISCAIROPATH(arg))
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "CairoPath");
	return arg;
}


static IoTag *IoCairoPath_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoPath_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPath_free);
	return tag;
}

IoCairoPath *IoCairoPath_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoPath_newTag(state));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
			{"foreach", IoCairoPath_foreach},
		{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoCairoPath *IoCairoPath_rawClone(IoCairoPath *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (DATA(proto))
	{
		DATA(proto)->refCount += 1;
		IoObject_setDataPointer_(self, DATA(proto));
	}
	return self;
}

IoCairoPath *IoCairoPath_newWithRawPath_(void *state, cairo_path_t *path)
{
	IoCairoPath *self = 0;

	checkStatus_(state, 0, path->status);

	//self = IOCLONE(IoState_protoWithInitId_(state, protoId));
	self = IOCLONE(IoState_protoWithId_(state, protoId));
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCairoPathData)));
	DATA(self)->path = path;
	DATA(self)->refCount = 1;
	return self;
}

void IoCairoPath_free(IoCairoPath *self)
{
	if (!DATA(self))
		return;

	DATA(self)->refCount -= 1;
	if (DATA(self)->refCount > 0)
		return;

	cairo_path_destroy(PATH(self));
	free(DATA(self));
}


cairo_path_t *IoCairoPath_rawPath(IoCairoPath *self)
{
	return PATH(self);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoPath_foreach(IoCairoPath *self, IoObject *locals, IoMessage *m)
{
	cairo_path_t *path = PATH(self);
	IoList *elementList = IoList_new(IOSTATE);
	int i;

	for (i = 0; i < path->num_data; i += path->data[i].header.length)
		IoList_rawAppend_(elementList, IoCairoPathElement_newWithPath_dataOffset_(IOSTATE, self, i));

	return IoList_foreach(elementList, locals, m);
}
