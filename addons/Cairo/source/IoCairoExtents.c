//metadoc CairoExtents copyright Daniel Rosengren, 2007
//metadoc CairoExtents license BSD revised
//metadoc CairoExtents category Graphics

#include "IoCairoExtents.h"
#include "IoNumber.h"

#define DATA(self) ((IoCairoExtentsData *)IoObject_dataPointer(self))


static IoTag *IoCairoExtents_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoExtents");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoExtents_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoExtents_free);
	return tag;
}

IoCairoExtents *IoCairoExtents_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoExtents_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCairoExtentsData)));

	IoState_registerProtoWithFunc_(state, self, IoCairoExtents_proto);

	{
		IoMethodTable methodTable[] = {
			{"x1", IoCairoExtents_x1},
			{"y1", IoCairoExtents_y1},
			{"x2", IoCairoExtents_x2},
			{"y2", IoCairoExtents_y2},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoExtents *IoCairoExtents_rawClone(IoCairoExtents *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(DATA(proto), sizeof(IoCairoExtentsData)));
	return self;
}

IoCairoExtents *IoCairoExtents_newSet(void *state, double x1, double y1, double x2, double y2)
{
	IoCairoExtents *self = IOCLONE(IoState_protoWithInitFunction_(state, IoCairoExtents_proto));
	DATA(self)->x1 = x1;
	DATA(self)->y1 = y1;
	DATA(self)->x2 = x2;
	DATA(self)->y2 = y2;
	return self;
}

void IoCairoExtents_free(IoCairoExtents *self)
{
	free(DATA(self));
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoExtents_x1(IoCairoExtents *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->x1);
}

IoObject *IoCairoExtents_y1(IoCairoExtents *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->y1);
}

IoObject *IoCairoExtents_x2(IoCairoExtents *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->x2);
}

IoObject *IoCairoExtents_y2(IoCairoExtents *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->y2);
}
