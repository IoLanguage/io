/*#io
Match ioDoc(
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("RegularExpressions")
	docDescription("""Match instances hold the subject string, and the ranges of each capture.
	The ranges are used to extract the entire match and each capture.""")
*/

#include "IoMatch.h"
#include "IoRegex.h"
#include "IoRange.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include <stdlib.h>

#define DATA(self) ((IoMatchData *)IoObject_dataPointer(self))

static void IoMatch_getCapturesFromRegex_(IoMatch *self, IoRegex *regex);
static void IoMatch_getCaptureNamesFromRegex_(IoMatch *self, IoRegex *regex);


void IoRegex_MatchInit(IoObject *context)
{
	IoState *self = IoObject_state((IoObject *)context);
	
	IoObject *regex = IoObject_getSlot_(context, SIOSYMBOL("Regex"));
	IoObject_setSlot_to_(regex, SIOSYMBOL("Match"), IoMatch_proto(self));
}


/* ------------------------------------------------------------------------------------------------*/

IoTag *IoMatch_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Match");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoMatch_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoMatch_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoMatch_mark);
	return tag;
}

IoMatch *IoMatch_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoMatch_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoMatchData)));
	DATA(self)->captures = IoList_new(state);
	DATA(self)->ranges = IoList_new(state);
	DATA(self)->nameToIndexMap = IoMap_new(state);
	
	IoState_registerProtoWithFunc_(state, self, IoMatch_proto);
	
	{
		IoMethodTable methodTable[] = {
			{"subject", IoMatch_subject},
			{"captures", IoMatch_captures},
			{"ranges", IoMatch_ranges},
			{"nameToIndexMap", IoMatch_nameToIndexMap},
			{0, 0},
		};
		
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoMatch *IoMatch_rawClone(IoMatch *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoMatchData)));
	DATA(self)->captures = IOCLONE(DATA(proto)->captures);
	DATA(self)->ranges = IOCLONE(DATA(proto)->ranges);
	DATA(self)->nameToIndexMap = IOCLONE(DATA(proto)->nameToIndexMap);
	return self;
}

IoMatch *IoMatch_newFromRegex_(void *state, IoObject *regex)
{
	IoMatch *self = IOCLONE(IoState_protoWithInitFunction_(state, IoMatch_proto));
	DATA(self)->subject = ((IoRegexData *)IoObject_dataPointer(regex))->string;
	IoMatch_getCapturesFromRegex_(self, regex);
	IoMatch_getCaptureNamesFromRegex_(self, regex);
	return self;
}

void IoMatch_free(IoMatch *self)
{
}

void IoMatch_mark(IoMatch *self)
{
	IoObject_shouldMark((IoObject *)DATA(self)->captures);
	IoObject_shouldMark((IoObject *)DATA(self)->ranges);
	IoObject_shouldMark((IoObject *)DATA(self)->nameToIndexMap);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoMatch_subject(IoMatch *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("subject",
		"Returns the entire string being matched against.")
	*/
	return DATA(self)->subject;
}

IoObject *IoMatch_captures(IoMatch *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("captures",
		"Returns a list containing all the captures of the match as strings.")
	*/
	return DATA(self)->captures;
}

IoObject *IoMatch_ranges(IoMatch *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("ranges",
		"Returns a list containing a Range for each capture.")
	*/
	return DATA(self)->ranges;
}

IoObject *IoMatch_nameToIndexMap(IoMatch *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("nameToIndexMap",
		"Returns a Map which maps capture names to capture indices.")
	*/
	return DATA(self)->nameToIndexMap;
}


/* ------------------------------------------------------------------------------------------------*/
/* Private */

static void IoMatch_getCapturesFromRegex_(IoMatch *self, IoRegex *regex)
{
	IoRegexData *reData = (IoRegexData *)IoObject_dataPointer(regex);
	const char *subject = CSTRING(reData->string);
	int *range = reData->outputVector;
	int i;
	
	for (i = 0; i < reData->matchGroupSize; i++, range += 2)
	{
		int start = range[0], end = range[1]; 
		IoSymbol *symbol = 0;
		IoRange *range = 0;
		
		symbol = IoSeq_newSymbolWithData_length_(IOSTATE, subject + start, end - start);
		IoList_rawAppend_(DATA(self)->captures, symbol);
		
		range = IoRange_new(IOSTATE);
		IoRange_setFirst(range, IONUMBER(start));
		IoRange_setLast(range, IONUMBER(end));
		IoList_rawAppend_(DATA(self)->ranges, range);
	}
}

static void IoMatch_getCaptureNamesFromRegex_(IoMatch *self, IoRegex *regex)
{
	IoRegexData *reData = (IoRegexData *)IoObject_dataPointer(regex);
	const char *subject = CSTRING(reData->string);
	int nameCount = 0;
	int entrySize = 0;
	unsigned char *nameTable = 0;
	unsigned char *entry = 0;
	int i;

	pcre_fullinfo(reData->regex, reData->studyData, PCRE_INFO_NAMECOUNT, &nameCount);
	if (nameCount <= 0) return;

	pcre_fullinfo(reData->regex, reData->studyData, PCRE_INFO_NAMEENTRYSIZE, &entrySize);
	pcre_fullinfo(reData->regex, reData->studyData, PCRE_INFO_NAMETABLE, &nameTable);

	entry = nameTable;
	for (i = 0; i < nameCount; i++)
	{
		int n = (entry[0] << 8) | entry[1];
		IoMap_rawAtPut(DATA(self)->nameToIndexMap, IOSYMBOL(entry + 2), IONUMBER(n));
		entry += entrySize;
	}
}
