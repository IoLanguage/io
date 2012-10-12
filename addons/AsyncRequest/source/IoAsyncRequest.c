//metadoc AsyncRequest copyright Steve Dekorte, 2004
//metadoc AsyncRequest license BSD revised
//metadoc AsyncRequest category Filesystem
/*metadoc AsyncRequest description
Used for doing asynchronous file i/o. When this addon is loaded, it will override
the File proto's readToBufferLength, readBufferOfLength and write methods to 
automatically use AsyncRequests. 
<p>
Note: This addon is only needed for async file requests - all socket ops are already
asynchronous in Io.
*/

#include "IoAsyncRequest.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define IOCB(self) ((struct aiocb *)(IoObject_dataPointer(self)))
#define IOCB_BUFFER(self) ((void *)(IOCB(self)->aio_buf))

static const char *protoId = "AsyncRequest";

IoTag *IoAsyncRequest_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoAsyncRequest_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoAsyncRequest_free);
	return tag;
}

IoAsyncRequest *IoAsyncRequest_proto(void *state)
{
	IoAsyncRequest *self = IoObject_new(state);
	IoObject_tag_(self, IoAsyncRequest_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(struct aiocb)));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"setDescriptor", IoAsyncRequest_setDescriptor},
		{"descriptor", IoAsyncRequest_descriptor},
		{"numberOfBytes", IoAsyncRequest_numberOfBytes},
		{"read", IoAsyncRequest_read},
		{"write", IoAsyncRequest_write},
		{"isDone", IoAsyncRequest_isDone},
		{"error", IoAsyncRequest_error},
		{"cancel", IoAsyncRequest_cancel},
		{"sync", IoAsyncRequest_sync},
		{"copyBufferTo", IoAsyncRequest_copyBufferTo},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoAsyncRequest *IoAsyncRequest_rawClone(IoAsyncRequest *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(struct aiocb)));
	return self;
}

IoAsyncRequest *IoAsyncRequest_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoAsyncRequest_free(IoAsyncRequest *self)
{
	int fd = IOCB(self)->aio_fildes;

	if (fd)
	{
		aio_cancel(fd, IOCB(self));
	}

	if (IOCB_BUFFER(self))
	{
		free(IOCB_BUFFER(self));
	}

	free(IOCB(self));
}

// -----------------------------------------------------------

IoObject *IoAsyncRequest_setDescriptor(IoAsyncRequest *self, IoObject *locals, IoMessage *m)
{
	/*doc AsyncRequest setDescriptor(aDescriptorNumber)
	Sets the descriptor for the receiver. Returns self.
	*/
	IOCB(self)->aio_fildes = IoMessage_locals_intArgAt_(m, locals, 0);
	return self;
}

IoObject *IoAsyncRequest_descriptor(IoAsyncRequest *self, IoObject *locals, IoMessage *m)
{
	/*doc AsyncRequest descriptor
	Returns the descriptor for the request.
	*/
	return IONUMBER(IOCB(self)->aio_fildes);
}

IoObject *IoAsyncRequest_numberOfBytes(IoAsyncRequest *self, IoObject *locals, IoMessage *m)
{
	/*doc AsyncRequest numberOfBytes
	Returns the number of bytes associated with the request.
	*/
	return IONUMBER(IOCB(self)->aio_nbytes);
}

IoObject *IoAsyncRequest_position(IoAsyncRequest *self, IoObject *locals, IoMessage *m)
{
	/*doc AsyncRequest position
	Returns a Number for the position of the descriptor.
	*/
	return IONUMBER(IOCB(self)->aio_offset);
}

IoObject *IoAsyncRequest_read(IoAsyncRequest *self, IoObject *locals, IoMessage *m)
{
	/*doc AsyncRequest read(aSeq, numberOfBytes)
	Submits an async read request. Returns nil on error, self otherwise. 
	*/
	int r;

	IOCB(self)->aio_offset = (size_t)CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 0));
	IOCB(self)->aio_nbytes = IoMessage_locals_intArgAt_(m, locals, 1);

	if (!IOCB_BUFFER(self))
	{
		IOCB(self)->aio_buf = calloc(1, IOCB(self)->aio_nbytes);
	}
	else
	{
		IOCB(self)->aio_buf = realloc(IOCB_BUFFER(self), IOCB(self)->aio_nbytes);
	}

	r = aio_read(IOCB(self));

	return r == 0 ? self : IONIL(self);
}

IoObject *IoAsyncRequest_write(IoAsyncRequest *self, IoObject *locals, IoMessage *m)
{
	/*doc AsyncRequest write(fileOffset, aSeq, bufferOffset, numberOfBytesToWrite)
	Submits an async write request. Returns nil on error, self otherwise. 
	*/
	
	int r;
	IoSeq *data;
	UArray *ba;
	int bufferOffset;
	int bytesToWrite;

	IOCB(self)->aio_offset = (size_t)CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 0));

	data = IoMessage_locals_seqArgAt_(m, locals, 1);
	ba = IoSeq_rawUArray(data);

	bufferOffset = IoMessage_locals_intArgAt_(m, locals, 2);
	bytesToWrite = IoMessage_locals_intArgAt_(m, locals, 3);

	if (bytesToWrite > UArray_size(ba) - bufferOffset)
	{
		bytesToWrite = UArray_size(ba) - bufferOffset;
	}

	IOCB(self)->aio_nbytes = bytesToWrite;
	IOCB(self)->aio_buf = realloc(IOCB_BUFFER(self), bytesToWrite);
	memcpy(IOCB_BUFFER(self), UArray_bytes(ba), bytesToWrite);

	r = aio_write(IOCB(self));

	return r == 0 ? self : IONIL(self);
}

IoObject *IoAsyncRequest_isDone(IoAsyncRequest *self, IoObject *locals, IoMessage *m)
{
	/*doc AsyncRequest isDone
	Returns true if the request is done, false otherwise. 
	*/
	int r = aio_error(IOCB(self));
	return (r == 0 || r != EINPROGRESS) ? IOTRUE(self) : IOFALSE(self);
}

#include <errno.h>

IoObject *IoAsyncRequest_error(IoAsyncRequest *self, IoObject *locals, IoMessage *m)
{
	/*doc AsyncRequest error
	Returns sequence containing the last error or nil otherwise. 
	*/
	int r;

	errno = 0;

	r = aio_error(IOCB(self));

	if (r == -1)
	{
		char *s = strerror(errno);

		if (errno == 22)
		{
			s = "The iocb argument does not reference an outstanding asynchronous I/O request.";
			printf("AsyncRequest error: %s\n", s);
			//return IOSYMBOL("The iocb argument does not reference an outstanding asynchronous I/O request.");
			return IONIL(self);
		}

		printf("AsyncRequest errno %i : %s' EINVAL = %i\n", errno, s, EINVAL);
		return IOSYMBOL(s);
	}

	if (r != 0 && r != EINPROGRESS)
	{
		char *s = strerror(r);
		printf("AsyncRequest error %i\n", r);
		return IOSYMBOL(s);
	}

	return IONIL(self);
}

IoObject *IoAsyncRequest_cancel(IoAsyncRequest *self, IoObject *locals, IoMessage *m)
{
	/*doc AsyncRequest cancel
	Cancels the request. Returns nil on error or self otherwise.
	*/
	int r = aio_cancel(IOCB(self)->aio_fildes, IOCB(self));
	return r == 0 ? self : IONIL(self);
}

IoObject *IoAsyncRequest_sync(IoAsyncRequest *self, IoObject *locals, IoMessage *m)
{
	/*doc AsyncRequest sync
	Waits for the request to complete. Returns nil on error or self otherwise.
	*/
	int r = aio_fsync(O_SYNC, IOCB(self));
	return r == 0 ? self : IONIL(self);
}

IoObject *IoAsyncRequest_copyBufferTo(IoAsyncRequest *self, IoObject *locals, IoMessage *m)
{
	/*doc AsyncRequest copyBufferto(aSeq)
	Copies the request buffer's data to aSeq.
	Returns nil on error or self otherwise.
	*/
	if (IOCB_BUFFER(self))
	{
		IoSeq *data = IoMessage_locals_mutableSeqArgAt_(m, locals, 0);
		UArray *ba = IoSeq_rawUArray(data);
		UArray_setData_type_size_copy_(ba, IOCB_BUFFER(self), CTYPE_uint8_t, IOCB(self)->aio_nbytes, 1);
		return self;
	}

	return IONIL(self);
}

