#include "IoUnixPath.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define UNIXPATH(self) ((UnixPath *)IoObject_dataPointer(self))

static const char *protoId = "UnixPath";

IoTag *IoUnixPath_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
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

	IoState_registerProtoWithId_((IoState *)state, self, protoId);

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
	IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
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
  IoObject * path = IoMessage_locals_seqArgAt_(m, locals, 0);
	char *pathString = IoSeq_asCString(path);
	size_t pathlen = IoSeq_rawSizeInBytes(path);
	UnixPath_setPath_(UNIXPATH(self), pathString, pathlen);
	return self;
#else
	return IOSYMBOL("Sorry, no Unix Domain sockets on Windows MSCRT");
#endif
}

IoObject *IoUnixPath_path(IoUnixPath *self, IoObject *locals, IoMessage *m)
{
#if !defined(_WIN32) || defined(__CYGWIN__)
  size_t pathlen;
  char* path = UnixPath_path(UNIXPATH(self), &pathlen);
  return IoState_symbolWithCString_length_(IOSTATE, path, pathlen);
#else
	return IOSYMBOL("Sorry, no Unix Domain sockets on Windows MSCRT");
#endif
}

