//metadoc Linker copyright Marc Fauconneau, 2006
//metadoc Linker license BSD revised
/*metadoc Linker description
	An object that enables low level introspection into a running Io VM.")
*/

#include "IoLinker.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoMap.h"

double fptrToDouble(void* fptr)
{
	double x;
	long y = (long)fptr;
	x = y;
	return x;
}


IoLinker *IoLinker_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoState_registerProtoWithFunc_(state, self, IoLinker_proto);

	{
		IoMethodTable methodTable[] = {
			{"makeCFunction", IoLinker_makeCFunction},
			{"bytesToHexSeq", IoLinker_bytesToHexSeq},
			{"hexSeqToBytes", IoLinker_hexSeqToBytes},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	IoObject_setSlot_to_(self, IOSYMBOL("IoState_numberWithDouble_"), IONUMBER(fptrToDouble(IoState_numberWithDouble_)));
	IoObject_setSlot_to_(self, IOSYMBOL("IoMessage_locals_valueArgAt_"), IONUMBER(fptrToDouble(IoMessage_locals_valueArgAt_)));

	return self;
}

IoObject *IoLinker_makeCFunction(IoLinker *self, IoObject *locals, IoMessage *m)
{
	IoSeq *buffer = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoSeq *slotName = IoMessage_locals_seqArgAt_(m, locals, 1);
	IoObject *object = IoMessage_locals_valueArgAt_(m, locals, 2);
	IoCFunction *f;
	IoUserFunction* fp = (IoUserFunction*)IoSeq_rawBytes(buffer);

	f = IoCFunction_newWithFunctionPointer_tag_name_(IOSTATE, fp, IoObject_tag(object), CSTRING(slotName));
	IoObject_setSlot_to_(f, IOSYMBOL("compiledCode"), buffer);
	return f;
}

int charFromHex(int c)
{
	if (c>='0'&& c<='9') return c-'0';
	if (c>='a'&& c<='f') return c-'a'+10;
	if (c>='A'&& c<='F') return c-'A'+10;
	return 0;
}

UArray *UArray_fromHexStringUArray(UArray *self)
{
	size_t i, newSize = self->size / 2;
	UArray *ba = UArray_new();
	UArray_setSize_(ba, newSize);

	for(i = 0; i < newSize; i ++)
	{
		int h = self->data[i*2];
		int l = self->data[i*2+1];

		ba->data[i] = (charFromHex(h)<<4) + charFromHex(l);
	}

	return ba;
}

IoObject *IoLinker_bytesToHexSeq(IoLinker *self, IoObject *locals, IoMessage *m)
{
	IoSeq *buffer = IoMessage_locals_seqArgAt_(m, locals, 0);
	UArray *ba = IoSeq_rawUArray(buffer);
	return IoSeq_newWithUArray_copy_(IOSTATE, UArray_asNewHexStringUArray(ba), 0);
}

IoObject *IoLinker_hexSeqToBytes(IoLinker *self, IoObject *locals, IoMessage *m)
{
	IoSeq *buffer = IoMessage_locals_seqArgAt_(m, locals, 0);
	UArray *ba = IoSeq_rawUArray(buffer);
	return IoSeq_newWithUArray_copy_(IOSTATE, UArray_fromHexStringUArray(ba), 0);
}
