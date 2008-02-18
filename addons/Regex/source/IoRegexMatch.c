/*
	Written by Daniel Rosengren
	danne.rosengren@gmail.com
*/

#include "IoRegexMatch.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include <stdlib.h>

#define DATA(self) ((IoRegexMatchData *)IoObject_dataPointer(self))


IoTag *IoRegexMatch_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("RegexMatch");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoRegexMatch_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoRegexMatch_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoRegexMatch_free);
	return tag;
}

IoRegexMatch *IoRegexMatch_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoRegexMatch_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoRegexMatchData)));
	DATA(self)->regex = IONIL(self);
	DATA(self)->subject = IOSYMBOL("");
	DATA(self)->ranges = IoList_new(state);

	IoState_registerProtoWithFunc_(state, self, IoRegexMatch_proto);

	{
		IoMethodTable methodTable[] = {
			{"regex", IoRegexMatch_regex},
			{"subject", IoRegexMatch_subject},
			{"ranges", IoRegexMatch_ranges},
			{0, 0},
		};

		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoRegexMatch *IoRegexMatch_rawClone(IoRegexMatch *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoRegexMatchData)));
	DATA(self)->subject = IOREF(DATA(proto)->subject);
	DATA(self)->regex = IOREF(DATA(proto)->regex);
	DATA(self)->ranges = IOREF(DATA(proto)->ranges);
	return self;
}

IoRegexMatch *IoRegexMatch_newWithRegex_subject_captureRanges_(void *state, IoRegex *regex, IoSymbol *subject, IoList *captureRanges)
{
	IoRegexMatch *self = IOCLONE(IoState_protoWithInitFunction_(state, IoRegexMatch_proto));
	DATA(self)->regex = IOREF(regex);
	DATA(self)->subject = IOREF(subject);
	DATA(self)->ranges = captureRanges;
	return self;
}

void IoRegexMatch_free(IoRegexMatch *self)
{
	free(DATA(self));
}

void IoRegexMatch_mark(IoRegexMatch *self)
{
	IoObject_shouldMark(DATA(self)->regex);
	IoObject_shouldMark(DATA(self)->subject);
	IoObject_shouldMark(DATA(self)->ranges);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoRegexMatch_regex(IoRegexMatch *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatch regex
	Returns the Regex that was used to find this match.
	*/
	return DATA(self)->regex;
}

IoObject *IoRegexMatch_subject(IoRegexMatch *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatch subject
	Returns the string that this match was found in.
	*/
	return DATA(self)->subject;
}

IoObject *IoRegexMatch_ranges(IoRegexMatch *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatch ranges
	Returns a list containing the range of each capture.
	*/
	return DATA(self)->ranges;
}
