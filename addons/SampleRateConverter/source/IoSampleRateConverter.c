
//metadoc SampleRateConverter copyright Steve Dekorte, 2004
//metadoc SampleRateConverter license BSD revised
//metadoc SampleRateConverter category Media
/*metadoc description
	A binding for lib sample rate.
*/
			   */
#include "IoSampleRateConverter.h"
#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define DATA(self) ((IoSampleRateConverterData *)IoObject_dataPointer(self))

IoTag *IoSampleRateConverter_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("SampleRateConverter");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSampleRateConverter_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSampleRateConverter_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSampleRateConverter_free);
	return tag;
}

IoSampleRateConverter *IoSampleRateConverter_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSampleRateConverter_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoSampleRateConverterData)));

	DATA(self)->inputBuffer  = IoSeq_new(state);
	DATA(self)->outputBuffer = IoSeq_new(state);

	IoState_registerProtoWithFunc_(state, self, IoSampleRateConverter_proto);

	{
		IoMethodTable methodTable[] = {
		{"start", IoSampleRateConverter_start},
		{"process", IoSampleRateConverter_process},
		{"stop",  IoSampleRateConverter_stop},
		{"setOutputToInputRatio",  IoSampleRateConverter_setOutputToInputRatio},
		{"setEndOFInput",  IoSampleRateConverter_setEndOFInput},
		{"inputBuffer",  IoSampleRateConverter_inputBuffer},
		{"outputBuffer",  IoSampleRateConverter_outputBuffer},
		{"outputToInputRatio",  IoSampleRateConverter_outputToInputRatio},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoSampleRateConverter *IoSampleRateConverter_rawClone(IoSampleRateConverter *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoSampleRateConverterData)));

	DATA(self)->inputBuffer  = IoSeq_new(IOSTATE);
	DATA(self)->outputBuffer = IoSeq_new(IOSTATE);

	return self;
}

IoSampleRateConverter *IoSampleRateConverter_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoSampleRateConverter_proto);
	return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

void IoSampleRateConverter_freeSampleRateStateIfNeeded(IoSampleRateConverter *self)
{
	if (DATA(self)->srcData)
	{
		src_delete(DATA(self)->srcState);
		DATA(self)->srcState = NULL;

		free(DATA(self)->srcData);
		DATA(self)->srcData = NULL;
	}
}

void IoSampleRateConverter_free(IoSampleRateConverter *self)
{
	IoSampleRateConverter_freeSampleRateStateIfNeeded(self);
	free(IoObject_dataPointer(self));
}

void IoSampleRateConverter_mark(IoSampleRateConverter *self)
{
	IoObject_shouldMark(DATA(self)->inputBuffer);
	IoObject_shouldMark(DATA(self)->outputBuffer);
}

/* ----------------------------------------------------------- */

SRC_DATA *IoSampleRateConverter_srcData(IoSampleRateConverter *self)
{
	if (DATA(self)->srcData == NULL)
	{
		int error;
		DATA(self)->srcState = src_new(SRC_SINC_MEDIUM_QUALITY, 2, &error);
		DATA(self)->srcData = calloc(1, sizeof(SRC_DATA));
		DATA(self)->srcData->src_ratio = 1.0;
		DATA(self)->srcData->end_of_input = 0;
	}

	return DATA(self)->srcData;
}

/*
IoObject *IoSampleRateConverter_process(IoSampleRateConverter *self, IoObject *locals, IoMessage *m)
{
	SRC_DATA *srcData = IoSampleRateConverter_srcData(self);

	UArray *inba  = IoSeq_rawUArray(DATA(self)->inputBuffer);
	UArray *outba = IoSeq_rawUArray(DATA(self)->outputBuffer);

	size_t inSize = UArray_size(inba);

	UArray_setSize_(outba, inSize);

	srcData->data_in  = (float *)UArray_bytes(inba);
	srcData->input_frames  = inSize / ( 2 * sizeof(float));

	srcData->data_out = (float *)UArray_bytes(outba);
	srcData->output_frames = srcData->input_frames;

	src_process(DATA(self)->srcState, srcData);

	UArray_setSize_(inba, 0);
	//UArray_removeRange(inba, 0, srcData->input_frames_used * 2 * sizeof(float));
	//UArray_setSize_(outba, srcData->output_frames_gen * 2 * sizeof(float));

	return self;
}
*/

IoObject *IoSampleRateConverter_process(IoSampleRateConverter *self, IoObject *locals, IoMessage *m)
{
	 SRC_DATA *srcData = IoSampleRateConverter_srcData(self);

	 UArray *inba  = IoSeq_rawUArray(DATA(self)->inputBuffer);
	 UArray *outba = IoSeq_rawUArray(DATA(self)->outputBuffer);

	 size_t inSize = UArray_size(inba);
	 size_t oldOutSize = UArray_size(outba);

	 UArray_setSize_(outba, oldOutSize + inSize);

	 srcData->data_in  = (float *)(UArray_bytes(inba));
	 srcData->input_frames  = inSize / ( 2 * sizeof(float));

	 srcData->data_out = (float *)(UArray_bytes(outba) + oldOutSize);
	 srcData->output_frames = srcData->input_frames;

	 src_process(DATA(self)->srcState, srcData);

	 UArray_removeRange(inba, 0, srcData->input_frames_used * 2 * sizeof(float));
	 UArray_setSize_(outba, oldOutSize + (srcData->output_frames_gen * 2 * sizeof(float)));

	 return self;
}

IoObject *IoSampleRateConverter_start(IoSampleRateConverter *self, IoObject *locals, IoMessage *m)
{
	SRC_DATA *srcData = IoSampleRateConverter_srcData(self);

	if (!srcData)
	{
		int error;
		DATA(self)->srcState = src_new(SRC_SINC_MEDIUM_QUALITY, 2, &error);
		srcData = calloc(1, sizeof(srcData));
		srcData->src_ratio = 1.0;
		srcData->end_of_input = 0;
	}

	return self;
}

IoObject *IoSampleRateConverter_stop(IoSampleRateConverter *self, IoObject *locals, IoMessage *m)
{
	IoSampleRateConverter_freeSampleRateStateIfNeeded(self);

	return self;
}

IoObject *IoSampleRateConverter_setOutputToInputRatio(IoSampleRateConverter *self, IoObject *locals, IoMessage *m)
{
	SRC_DATA *srcData = IoSampleRateConverter_srcData(self);
	IoNumber *r = IoMessage_locals_numberArgAt_(m, locals, 0);
	srcData->src_ratio = CNUMBER(r);
	return self;
}

IoObject *IoSampleRateConverter_outputToInputRatio(IoSampleRateConverter *self, IoObject *locals, IoMessage *m)
{
	SRC_DATA *srcData = IoSampleRateConverter_srcData(self);
	return IONUMBER(srcData->src_ratio);
}

IoObject *IoSampleRateConverter_setEndOFInput(IoSampleRateConverter *self, IoObject *locals, IoMessage *m)
{
	SRC_DATA *srcData = IoSampleRateConverter_srcData(self);
	IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
	srcData->end_of_input = ISTRUE(v) ? 1 : 0;
	return self;
}

IoObject *IoSampleRateConverter_inputBuffer(IoSampleRateConverter *self, IoObject *locals, IoMessage *m)
{
	return DATA(self)->inputBuffer;
}

IoObject *IoSampleRateConverter_outputBuffer(IoSampleRateConverter *self, IoObject *locals, IoMessage *m)
{
	return DATA(self)->outputBuffer;
}
