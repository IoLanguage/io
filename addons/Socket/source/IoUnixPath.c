#include "IoUnixPath.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define UNIXPATH(self) ((UnixPath *)IoObject_dataPointer(self))

IoTag *IoUnixPath_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("UnixPath");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoUnixPath_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoUnixPath_free);
	return tag;
}

IoUnixPath *IoUnixPath_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoUnixPath_newTag(state));
#if !defined(_WIN32) || defined(__CYGWIN__)
	IoObject_setDataPointer_(self, UnixPath_new());
#endif

	IoState_registerProtoWithFunc_((IoState *)state, self, IoUnixPath_proto);

	{
		IoMethodTable methodTable[] = {
		{"path", IoUnixPath_path},
		{"setPath", IoUnixPath_setPath},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoUnixPath *IoUnixPath_rawClone(IoUnixPath *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
#if !defined(_WIN32) || defined(__CYGWIN__)
	IoObject_setDataPointer_(self, UnixPath_new());
#endif
	return self;
}

IoUnixPath *IoUnixPath_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoUnixPath_proto);
	return IOCLONE(proto);
}

void IoUnixPath_free(IoUnixPath *self)
{
#if !defined(_WIN32) || defined(__CYGWIN__)
	UnixPath_free(UNIXPATH(self));
#endif
}

// -----------------------------------------------------------

// path

IoObject *IoUnixPath_setPath(IoUnixPath *self, IoObject *locals, IoMessage *m)
{
#if !defined(_WIN32) || defined(__CYGWIN__)
	char *pathString = IoSeq_asCString(IoMessage_locals_seqArgAt_(m, locals, 0));
	UnixPath_setPath_(UNIXPATH(self), pathString);
	return self;
#else
	return IOSYMBOL("Sorry, no Unix Domain sockets on Windows MSCRT");
#endif
}

IoObject *IoUnixPath_path(IoUnixPath *self, IoObject *locals, IoMessage *m)
{
#if !defined(_WIN32) || defined(__CYGWIN__)
	return IOSYMBOL(UnixPath_path(UNIXPATH(self)));
#else
	return IOSYMBOL("Sorry, no Unix Domain sockets on Windows MSCRT");
#endif
}

