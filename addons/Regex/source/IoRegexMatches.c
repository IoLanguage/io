
//metadoc RegexMatches copyright Daniel Rosengren danne.rosengren@gmail.com
//metadoc RegexMatches license BSD revised
//metadoc RegexMatches category Parsers
/*metadoc RegexMatches description
	
*/

#include "IoRegexMatches.h"
#include "IoRegexMatch.h"
#include "IoRange.h"
#include "IoNumber.h"
#include <stdlib.h>
#include <stdio.h>

#define DATA(self) ((IoRegexMatchesData *)IoObject_dataPointer(self))

static const char *protoId = "RegexMatches";

static IoRegexMatch *IoRegexMatches_search(IoRegex *self, IoMessage *m);
static IoRegexMatch *IoRegexMatches_searchWithOptions_(IoRegex *self, IoMessage *m, int options);
static IoRegexMatch *IoRegexMatches_searchFrom_withOptions_(IoRegexMatches *self, IoMessage *m, int position, int options);

static IoObject *IoRegexMatches_rawsetPosition_(IoRegexMatches *self, int position);


IoTag *IoRegexMatches_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoRegexMatches_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoRegexMatches_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoRegexMatches_free);
	return tag;
}

IoRegexMatches *IoRegexMatches_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoRegexMatches_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoRegexMatchesData)));
	DATA(self)->regex = IONIL(self);
	DATA(self)->string = IOSYMBOL("");

	DATA(self)->captureArray = UArray_new();
	UArray_setItemType_(DATA(self)->captureArray, CTYPE_uint32_t);

	IoState_registerProtoWithFunc_(state, self, IoRegexMatches_proto);

	{
		IoMethodTable methodTable[] = {
			{"setRegex", IoRegexMatches_setRegex},
			{"regex", IoRegexMatches_regex},

			{"setString", IoRegexMatches_setString},
			{"string", IoRegexMatches_string},

			{"setPosition", IoRegexMatches_setPosition},
			{"position", IoRegexMatches_position},
			{"setEndPosition", IoRegexMatches_setEndPosition},
			{"endPosition", IoRegexMatches_endPosition},

			{"next", IoRegexMatches_next},
			{"anchored", IoRegexMatches_anchored},

			{"allowEmptyMatches", IoRegexMatches_allowEmptyMatches},
			{"disallowEmptyMatches", IoRegexMatches_disallowEmptyMatches},
			{"allowsEmptyMatches", IoRegexMatches_allowsEmptyMatches},

			{0, 0},
		};

		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoRegexMatches *IoRegexMatches_rawClone(IoRegexMatches *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoRegexMatchesData)));

	if (!ISNIL(DATA(proto)->regex))
		DATA(self)->regex = IOREF(DATA(proto)->regex);
	else
		DATA(self)->regex = IONIL(self);
	DATA(self)->string = IOREF(DATA(proto)->string);

	DATA(self)->captureArray = UArray_clone(DATA(proto)->captureArray);
	return self;
}

IoRegexMatches *IoRegexMatches_new(void *state)
{
	IoRegexMatches *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoRegexMatches_free(IoRegexMatches *self)
{
	UArray_free(DATA(self)->captureArray);
	free(DATA(self));
}

void IoRegexMatches_mark(IoRegexMatches *self)
{
	IoObject_shouldMark(DATA(self)->regex);
	IoObject_shouldMark(DATA(self)->string);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoRegexMatches_setRegex(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches setRegex(aRegexOrString)
	Sets the regex to find matches in. Returns self.
	*/
	IoObject *arg = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISREGEX(arg))
		DATA(self)->regex = IOREF(arg);
	else if(ISSYMBOL(arg))
		DATA(self)->regex = IoRegex_newWithPattern_(IOSTATE, arg);
	else
		IoState_error_(IOSTATE, m, "The argument to setRegex must be either a Regex or a Sequence");

	{
		/* Make the capture array big enough to hold the capture information and any other data
		pcre_exec may want to put in it. */
		int size = (IoRegex_rawRegex(DATA(self)->regex)->captureCount + 1) * 3;
		UArray_setSize_(DATA(self)->captureArray, size);
	}

	IoRegexMatches_rawsetPosition_(self, 0);
	return self;
}

IoObject *IoRegexMatches_regex(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches regex
	Returns the Regex that the receiver uses for finding matching.
	*/
	return DATA(self)->regex;
}


IoObject *IoRegexMatches_setString(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches setString(aString)
	Sets the string to find matches in. Returns self.
	*/
	DATA(self)->string = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
	DATA(self)->endPosition = IoSeq_rawSize(DATA(self)->string);
	IoRegexMatches_rawsetPosition_(self, 0);
	return self;
}

IoObject *IoRegexMatches_string(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches string
	Returns the string that the receiver finds matches in.
	*/
	return DATA(self)->string;
}


IoObject *IoRegexMatches_setPosition(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches setPosition(aRegexOrString)
	Sets the search position to the given index in the string. Returns self.
	*/
	IoRegexMatches_rawsetPosition_(self, IoMessage_locals_intArgAt_(m, locals, 0));
	return self;
}

IoObject *IoRegexMatches_position(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches position
	Returns the search position as an index in the string.
	*/
	return IONUMBER(DATA(self)->position);
}

IoObject *IoRegexMatches_setEndPosition(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches setEndPosition(anIndex)
	Sets the index in the string where the receiver should stop searching. It will be as
	if the string ends at that index. If <em>index</em> is nil, the end position will be set
	to the end of string.
	Returns self.

	<pre>
	Io> "funkadelic" matchesOfRegex("\\w+") setEndPosition(4) next string
	==> funk

	Io> "funkadelic" matchesOfRegex("\\w+") setEndPosition(nil) next string
	==> funkadelic
	</pre>
	*/
	IoObject *arg = IoMessage_locals_valueArgAt_(m, locals, 0);
	int stringLength = IoSeq_rawSize(DATA(self)->string);
	int endPos = stringLength;

	if (ISNIL(arg)) {
		DATA(self)->endPosition = endPos;
		return self;
	}

	if (!ISNUMBER(arg))
		IoState_error_(IOSTATE, m, "The argument to setEndPosition must be either a Number or nil");

	endPos = IoNumber_asInt(arg);
	if (endPos < 0)
		endPos = 0;
	else if (endPos > stringLength)
		endPos = stringLength;
	DATA(self)->endPosition = endPos;
	return self;
}

IoObject *IoRegexMatches_endPosition(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches endPosition
	Returns the index in the string where the receiver stops searching.
	*/
	return IONUMBER(DATA(self)->endPosition);
}


IoObject *IoRegexMatches_next(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches next
	Returns the next match, or nil if there is none.
	*/
	IoRegexMatch *match = 0;

	if (DATA(self)->position >= DATA(self)->endPosition)
		/* We've passed the end position, so we're done. */
		return IONIL(self);

	if (!DATA(self)->currentMatchIsEmpty)
		/* The previous match was not a zero length match, so we can just continue searching
		from the end of that match. */
		return IoRegexMatches_search(self, m);

	/* The last match was a zero length match. If we just continue searching as normal,
	we'll get the same empty match again, and we'll end up in an infinite loop when trying
	to iterate through all matches. Instead we try to find an alternative match by performing
	a search using the options PCRE_NOTEMPTY and PCRE_ANCHORED: */
	match = IoRegexMatches_searchWithOptions_(self, m, PCRE_NOTEMPTY | PCRE_ANCHORED);
	if (!ISNIL(match))
		return match;

	/* No alternative match was found, so we do what Perl does: we advance our position
	by one character, and continue searching from there: */
	++DATA(self)->position;
	return IoRegexMatches_search(self, m);
}

IoObject *IoRegexMatches_anchored(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches anchored
	Like <code>next</code>, but will only match at the current search position.
	*/
	return IoRegexMatches_searchWithOptions_(self, m, DATA(self)->options | PCRE_ANCHORED);
}


IoObject *IoRegexMatches_allowEmptyMatches(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches allowEmptyMatches
	Tells the receiver to allow zero length matches. Empty matches are allowed by default.
	Returns self.
	*/
	DATA(self)->options &= ~PCRE_NOTEMPTY;
	return self;
}

IoObject *IoRegexMatches_disallowEmptyMatches(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches disallowEmptyMatches
	Tells the receiver not to allow zero length matches. Returns self.
	*/
	DATA(self)->options |= PCRE_NOTEMPTY;
	return self;
}

IoObject *IoRegexMatches_allowsEmptyMatches(IoRegexMatches *self, IoObject *locals, IoMessage *m)
{
	/*doc RegexMatches allowsEmptyMatches
	Returns true if the receiver allows empty matches, false if not.
	*/
	return IOBOOL(self, DATA(self)->options & PCRE_NOTEMPTY);
}


/* ------------------------------------------------------------------------------------------------*/
/* Private */

static IoRegexMatch *IoRegexMatches_search(IoRegex *self, IoMessage *m)
{
	return IoRegexMatches_searchWithOptions_(self, m, DATA(self)->options);
}

static IoRegexMatch *IoRegexMatches_searchWithOptions_(IoRegex *self, IoMessage *m, int options)
{
	return IoRegexMatches_searchFrom_withOptions_(self, m, DATA(self)->position, options);
}

static IoRegexMatch *IoRegexMatches_searchFrom_withOptions_(IoRegexMatches *self, IoMessage *m, int position, int options)
{
	Regex *regex = IoRegex_rawRegex(DATA(self)->regex);
	int *captures = 0;
	int *capture = 0;
	IoList *rangeList = 0;
	int i = 0;

	int captureCount = Regex_search_from_to_withOptions_captureArray_(
		regex,
		CSTRING(DATA(self)->string),
		position,
		DATA(self)->endPosition,
		options,
		DATA(self)->captureArray
	);

	if (Regex_error(regex))
		IoState_error_(IOSTATE, m, Regex_error(regex));

	if (captureCount == 0)
		return IONIL(self);

	/* The search function puts information about captured substrings in captureArray.
	There's a pair of integers for each capture. The first element of the pair is the
	start index of the substring, and the second element is the end index.
	The first pair represents the entire match. */
	captures = (int *)UArray_data(DATA(self)->captureArray);
	DATA(self)->position = captures[1];
	DATA(self)->currentMatchIsEmpty = (captures[0] == captures[1]);

	capture = captures;
	rangeList = IoList_new(IOSTATE);
	for (i = 0; i < captureCount; i++) {
		IoObject *element = 0;
		// unsure about this locals initialization ...
		IoObject *locals = NULL;
		IoMessage *message = IoMessage_new(IOSTATE);

		if (capture[0] == -1 && capture[1] == -1) {
			/* This capture was not matched. */
			element = IONIL(self);
		} else {
			element = IoRange_new(IOSTATE);
			IoMessage_setCachedArg_to_(message, 0, IONUMBER(capture[0]));
			IoMessage_setCachedArg_to_(message, 1, IONUMBER(capture[1]));
			IoRange_setRange(element, locals, message);
			IoRange_setFirst(element, IONUMBER(capture[0]));
			IoRange_setLast(element, IONUMBER(capture[1]));
		}

		IoList_rawAppend_(rangeList, element);
		capture += 2;
	}

	return IoRegexMatch_newWithRegex_subject_captureRanges_(IOSTATE, DATA(self)->regex, DATA(self)->string, rangeList);
}


static IoObject *IoRegexMatches_rawsetPosition_(IoRegexMatches *self, int position)
{
	if (position < 0)
		position = 0;
	if (position > DATA(self)->endPosition)
		position = DATA(self)->endPosition;
	DATA(self)->position = position;

	DATA(self)->currentMatchIsEmpty = 0;
	return self;
}
