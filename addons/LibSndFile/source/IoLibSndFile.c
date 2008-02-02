
//metadoc LibSndFile copyright Steve Dekorte, 2004
//metadoc LibSndFile license BSD revised
//metadoc LibSndFile category Media
/*metadoc LibSndFile description
An object for encoding and decoding audio and video streams.
*/

#include "IoLibSndFile.h"
#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "IoList.h"
#include <limits.h>

#define DATA(self) ((IoLibSndFileData *)IoObject_dataPointer(self))

IoTag *IoLibSndFile_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("LibSndFile");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoLibSndFile_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoLibSndFile_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoLibSndFile_free);
	return tag;
}

IoLibSndFile *IoLibSndFile_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoLibSndFile_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoLibSndFileData)));

	DATA(self)->outputBuffer = IoSeq_new(state);
	DATA(self)->sfinfo = calloc(1, sizeof(SF_INFO));

	IoState_registerProtoWithFunc_(state, self, IoLibSndFile_proto);

	{
		IoMethodTable methodTable[] = {
		{"outputBuffer", IoLibSndFile_outputBuffer},
		{"formatNames", IoLibSndFile_formatNames},
		{"openForReading", IoLibSndFile_openForReading},
		{"openForWriting", IoLibSndFile_openForWriting},
		{"read",  IoLibSndFile_read},
		{"write",  IoLibSndFile_write},
		{"close",  IoLibSndFile_close},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoLibSndFile *IoLibSndFile_rawClone(IoLibSndFile *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoLibSndFileData)));

	DATA(self)->outputBuffer = IOCLONE(DATA(proto)->outputBuffer);
	DATA(self)->sfinfo = calloc(1, sizeof(SF_INFO));

	return self;
}

IoLibSndFile *IoLibSndFile_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoLibSndFile_proto);
	return IOCLONE(proto);
}

// -----------------------------------------------------------

void IoLibSndFile_free(IoLibSndFile *self)
{
	free(DATA(self)->sfinfo);
	free(IoObject_dataPointer(self));
}

void IoLibSndFile_mark(IoLibSndFile *self)
{
	IoObject_shouldMark(DATA(self)->outputBuffer);
}

// -----------------------------------------------------------

IoObject *IoLibSndFile_outputBuffer(IoLibSndFile *self, IoObject *locals, IoMessage *m)
{
	/*doc LibSndFile outputBuffer
	Returns the output buffer.
	*/
	return DATA(self)->outputBuffer;
}

IoObject *IoLibSndFile_stop(IoLibSndFile *self, IoObject *locals, IoMessage *m)
{
	/*doc LibSndFile stop
	Stops processing data.
	*/
	
	DATA(self)->isRunning = 0;
	return self;
}

IoObject *IoLibSndFile_isRunning(IoLibSndFile *self, IoObject *locals, IoMessage *m)
{
	/*doc LibSndFile isRunning
	Returns true if it's running, false otherwise.
	*/
	
	return IOBOOL(self, DATA(self)->isRunning);
}

IoObject *IoLibSndFile_formatNames(IoLibSndFile *self, IoObject *locals, IoMessage *m)
{
	/*doc LibSndFile formatNames
	Returns a list of strings with the names of the supported codecs.")
	*/
	
	IoList *names = IoList_new(IOSTATE);

	/*
	SF_FORMAT_INFO format_info;
	int k, count;

	sf_command(DATA(self)->sndfile, SFC_GET_SIMPLE_FORMAT_COUNT, &count, sizeof(int));

	for (k = 0 ; k < count ; k++)
	{
		format_info.format = k ;
		sf_command (sndfile, SFC_GET_SIMPLE_FORMAT, &format_info, sizeof(format_info));
		printf ("%08x  %s %s\n", format_info.format, format_info.name, format_info.extension);
		IoList_rawAppend_(names, IOSYMBOL(format_info.name));
	}
	*/

	return names;
}

int IoLibSndFile_IdOfFormat(char *f)
{
	char *r = strrchr(f, '.');
	// add code to deal with case

	if (r)
	{
		f = r + 1;
	}

	if (!strcmp(f, "wav"))   return SF_FORMAT_WAV | SF_FORMAT_FLOAT;
	if (!strcmp(f, "aiff"))  return SF_FORMAT_AIFF;
	if (!strcmp(f, "au"))    return SF_FORMAT_AU;
	if (!strcmp(f, "raw"))   return SF_FORMAT_RAW;
	if (!strcmp(f, "paf"))   return SF_FORMAT_PAF;
	if (!strcmp(f, "svx"))   return SF_FORMAT_SVX;
	if (!strcmp(f, "nist"))  return SF_FORMAT_NIST;
	if (!strcmp(f, "voc"))   return SF_FORMAT_VOC;
	if (!strcmp(f, "ircam")) return SF_FORMAT_IRCAM;
	if (!strcmp(f, "w64"))   return SF_FORMAT_W64;
	if (!strcmp(f, "mat4"))  return SF_FORMAT_MAT4;
	if (!strcmp(f, "mat5"))  return SF_FORMAT_MAT5;

	return 0;
}


// -----------------------------------------------------------

IoObject *IoLibSndFile_openForReading(IoLibSndFile *self, IoObject *locals, IoMessage *m)
{
	if (!DATA(self)->sndfile)
	{
		IoSeq *path = IoObject_symbolGetSlot_(self, IOSYMBOL("path"));
		IOASSERT(path, "missing path slot");
		DATA(self)->sndfile = sf_open(CSTRING(path), SFM_READ, DATA(self)->sfinfo);
		IOASSERT(DATA(self)->sndfile, sf_strerror(NULL));
	}

	IoObject_setSlot_to_(self, IOSYMBOL("frames"),     IONUMBER(DATA(self)->sfinfo->frames));
	IoObject_setSlot_to_(self, IOSYMBOL("sampleRate"), IONUMBER(DATA(self)->sfinfo->samplerate));
	IoObject_setSlot_to_(self, IOSYMBOL("channels"),   IONUMBER(DATA(self)->sfinfo->channels));
	IoObject_setSlot_to_(self, IOSYMBOL("format"),     IONUMBER(DATA(self)->sfinfo->format));
	IoObject_setSlot_to_(self, IOSYMBOL("seekable"),   IONUMBER(DATA(self)->sfinfo->seekable));
	//IoObject_setSlot_to_(self, IOSYMBOL("size"),       IONUMBER(DATA(self)->sfinfo->size));
	return self;
}

IoObject *IoLibSndFile_openForWriting(IoLibSndFile *self, IoObject *locals, IoMessage *m)
{
	if (!DATA(self)->sndfile)
	{
		IoSeq *path = IoObject_symbolGetSlot_(self, IOSYMBOL("path"));
		IOASSERT(path, "missing path slot");

		//DATA(self)->sfinfo->frames     = (sf_count_t)IoObject_doubleGetSlot_(self, IOSYMBOL("frames"));
		DATA(self)->sfinfo->samplerate = (sf_count_t)IoObject_doubleGetSlot_(self, IOSYMBOL("sampleRate"));
		DATA(self)->sfinfo->channels   = (sf_count_t)IoObject_doubleGetSlot_(self, IOSYMBOL("channels"));
		DATA(self)->sfinfo->format     = IoLibSndFile_IdOfFormat(CSTRING(path));

		DATA(self)->sndfile = sf_open(CSTRING(path), SFM_READ, DATA(self)->sfinfo);
		IOASSERT(DATA(self)->sndfile, sf_strerror(NULL));
	}

	return self;
}

IoObject *IoLibSndFile_close(IoLibSndFile *self, IoObject *locals, IoMessage *m)
{
	if (DATA(self)->sndfile)
	{
		sf_close(DATA(self)->sndfile);
	}

	return self;
}

IoObject *IoLibSndFile_read(IoLibSndFile *self, IoObject *locals, IoMessage *m)
{
	/*doc LibSndFile read(numberOfFrames)
	Read a given number of frames (sample pairs).")
	*/

	sf_count_t framesToRead = IoMessage_locals_intArgAt_(m, locals, 0);
	sf_count_t samplesRead = 0;

	IoLibSndFile_openForReading(self, locals, m);

	if (framesToRead)
	{
		UArray *outba = IoSeq_rawUArray(DATA(self)->outputBuffer);
		size_t samplesToRead = framesToRead * DATA(self)->sfinfo->channels;
		size_t bytesToRead = samplesToRead * sizeof(float);

		size_t oldSize = UArray_size(outba);
		float *buf;
		UArray_setSize_(outba, oldSize + bytesToRead);
		buf = (float *)(UArray_bytes(outba) + oldSize);

		samplesRead = sf_read_float(DATA(self)->sndfile, buf, samplesToRead);

		UArray_setSize_(outba, oldSize + (samplesRead * sizeof(float)));

		if (samplesRead != samplesToRead) return IONIL(self);
	}


	return self;
}

IoObject *IoLibSndFile_write(IoLibSndFile *self, IoObject *locals, IoMessage *m)
{
	size_t channels = IoObject_doubleGetSlot_(self, IOSYMBOL("channels"));
	UArray *inba = IoSeq_rawUArray(IoMessage_locals_seqArgAt_(m, locals, 0));
	sf_count_t framesToWrite = UArray_size(inba) / (channels * sizeof(float));
	sf_count_t framesWritten;

	IoLibSndFile_openForWriting(self, locals, m);

	framesWritten = sf_writef_float(DATA(self)->sndfile, (float *)UArray_bytes(inba), framesToWrite);

	return IONUMBER(framesWritten);
}
