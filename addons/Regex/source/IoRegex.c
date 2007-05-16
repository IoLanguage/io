/*#io
 Regex ioDoc(
		   docCopyright("Steve Dekorte", 2005)
		   docLicense("BSD revised")
		   docCategory("RegularExpressions")
		   docDescription("""The Regex addon adds support for Perl regular expressions 
 using the <a href=http://www.pcre.org/>PCRE</a> library. (PCRE has a BSD-friendly license.)
 Example use;
 <pre>
 r := Regex clone setPattern("aa*")
 matches := r setString("11aabb") allMatches // returns "aa"
 Regex rSubstitute ("wombats are cuddly", "(wom)(bat)", "\\2\\1!") //
 returns "batwom!s are cuddly"
 </pre>

 <br>
 <i>
 Some people, when confronted with a problem, think
 "I know, I'll use regular expressions." <br>
 Now they have two problems.</i><br>
 - Jamie Zawinski

 credits: PCRE library by Philip Hazel; rSubstitute method by Jason
 Grossman""")
*/


/*#io
 docSlot("allMatches", "Returns a List of strings containing all matches of 
 receiver's pattern within its string.")
 */



/*#io
 docSlot("caselessOn", "Turns case insensitive searching on.")
 */

/*#io
 docSlot("caselessOff", "Turns case insensitive searching off.")
 */

/*#io
 docSlot("dollarEndOnlyOn", """When on, a dollar metacharacter in the 
 pattern matches 
 only at the end of the subject string. Without this option, a dollar 
 also matches immediately before the final character if it is a newline 
 (but not before any other newlines). The PCRE_DOLLAR_ENDONLY option is 
 ignored if PCRE_MULTILINE is set. There is no equivalent to this option 
 in Perl.""")
 */


/*#io
 docSlot("dollarEndOnlyOff", "Turns the dollarEndOnly search mode off.")
 */


/*#io
 docSlot("firstMatch", "Returns a String containing the first match or a List 
 containing the first match group or Nil if no match is found.")
 */


/*#io
 docSlot("firstMatchRange", "Returns a List containing two numbers that are the index 
 of the first match and its length. Nil is returned if no match is 
 found.")
 */

/*#io
 docSlot("setIsGreedy(trueOrFalse)","""Turns the greedy search mode on or off.
 This option inverts the greediness of the quantifiers so
 that they are not greedy by default, but become greedy if followed by
 ?. It is not compatible with Perl. It can also be set by a (?U)
""")
 */

/*#io
 docSlot("greedyOn", """This option inverts the greediness of the quantifiers so 
 that they are not greedy by default, but become greedy if followed by 
 ?. It is not compatible with Perl. It can also be set by a (?U) 
 option setting within the pattern.""")
 */


/*#io
 docSlot("setCanMatchEmpty(trueOrFalse)", """Turns matchEmpty searching on or off."
 An empty string is not considered to be a valid match if
 this option is set. If there are alternatives in the pattern, they are
 tried. If all the alternatives match the empty string, the entire match.""")
*/

 /*#io
 docSlot("hasMatch", "Returns true if there are any matches or false otherwise.")
 */


/*#io
 docSlot("setIsMultiline(trueOrFalse)", 
	    """Turns the multiline search mode on or off.
 By default, PCRE treats the subject string as consisting
 of a single "line" of characters (even if it actually contains several
							newlines). The "start of line" metacharacter (^) matches only at the
 start of the string, while the "end of line" metacharacter ($) matches
 only at the end of the string, or before a terminating newline (unless
 PCRE_DOLLAR_ENDONLY is set). This is the same as Perl. When
 PCRE_MULTILINE it is set, the "start of line" and "end of line"
 constructs match immediately following or immediately before any
 newline in the subject string, respectively, as well as at the very
 start and end. This is equivalent to Perl's /m option. If there are no
 "\n" characters in a subject string, or no occurrences of ^ or $ in a
 pattern, setting PCRE_MULTILINE has no effect.""")
*/


/*#io
 docSlot("nextMatch", "Returns a String containing the next match or a List 
 containing the next match group or Nil if no match is found.")
 */

/*#io
 docSlot("currentMatch", "Returns the current match result. If there is no match, nil
 is returned.")
*/

 /*#io
 docSlot("nextMatchRange", "Returns a List containing two numbers that are the
 index of the next match and its length. Nil is returned if no match is found.")
 */

/*#io
  docSlot("numberOfCaptures", "Returns the number of captures defined by the pattern.")
*/

/*#io
 docSlot("pattern", "Returns the current pattern string.")
 */

/*#io
 docSlot("setPattern(aString)", "Sets (and compiles) the pattern to be used for finding
 matches. Returns self.")
*/

 /*#io
 docSlot("rSubstitute (aSequence, aString, aString)", """replaces all occurrences of the second argument (a 
 pattern) with the third argument (a replacement string), with the 
 substring "\\0" in the replacement string replaced by a string matching 
 the whole pattern, and substrings "\\1", "\\2" etc. in the replacement 
 string replaced by the substrings matching the first, second etc. 
 brocketed expressions in the pattern.""")
 */

/*#io
 docSlot("setPattern(aString)", "Sets (and compiles) the pattern to be used for finding
 matches. Returns self.")
*/

/*#io
 docSlot("rSubstitute (aSequence, aString, aString)", """replaces all occurrences of the second argument (a 
 pattern) with the third argument (a replacement string), with the 
 substring "\\0" in the replacement string replaced by a string matching 
 the whole pattern, and substrings "\\1", "\\2" etc. in the replacement 
 string replaced by the substrings matching the first, second etc. 
 brocketed expressions in the pattern.""")
 */

/*#io
 docSlot("setPattern(aString)", """Sets (and compiles) the pattern to be used for finding 
 matches. The default pattern is "*". Returns self.""")
 */

/*#io
 docSlot("setString(aString)", "Sets the string to find matches within.")
 */

/*#io
 docSlot("string", "Returns the string to find matches within.")
 */

/*#io
 docSlot("type", "Returns Regex.")
 */

/*
 docSlot("setIsUTF8(trueOrFalse)", "Turns the UTF8 search mode on or off.
 This option causes PCRE to regard both the pattern and the
 subject as strings of UTF-8 characters instead of just byte strings.
 However, it is available only if PCRE has been built to include UTF-8
 support. If not, the use of this option provokes an error. Support for
 UTF-8 is new, experimental, and incomplete. ")
*/

 /*#io
 docSlot("UTFOff", "Turns the UTF8 search mode off.")
 */

/*#io
 docSlot("UTF8On", """This option causes PCRE to regard both the pattern and the 
 subject as strings of UTF-8 characters instead of just byte strings. 
 However, it is available only if PCRE has been built to include UTF-8 
 support. If not, the use of this option provokes an error. Support for 
 UTF-8 is new, experimental, and incomplete. Details of exactly what it 
 entails are given below.""")
 */

/*#io
 docSlot("version", "Returns a String with the version info of the underlying 
 library.")
 */


#include "IoRegex.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include <stdlib.h>

#define DATA(self) ((IoRegexData *)IoObject_dataPointer(self))

IoTag *IoRegex_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Regex");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoRegex_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoRegex_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoRegex_mark);
	return tag;
}

IoRegex *IoRegex_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoRegex_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoRegexData)));
	
	DATA(self)->pattern = IOSYMBOL("^.*$");
	DATA(self)->string = IOSYMBOL("");
	DATA(self)->numberOfCaptures = 0;
	DATA(self)->compileOptions = PCRE_MULTILINE;
	DATA(self)->getMatchProtoMessage = IoMessage_newWithName_label_(state, IOSYMBOL("Match"), IOSYMBOL("IoRegex.c"));
	DATA(self)->cloneMatchMessage = IoMessage_newWithName_label_(state, IOSYMBOL("cloneWithSubject"), IOSYMBOL("IoRegex.c"));
	DATA(self)->addCaptureRangeMessage = IoMessage_newWithName_label_(state, IOSYMBOL("addCaptureRange"), IOSYMBOL("IoRegex.c"));
	
	IoState_registerProtoWithFunc_(state, self, IoRegex_proto);
	
	{
		IoMethodTable methodTable[] = {
		
		// pattern
		
		{"setPattern", IoRegex_setPattern},
		{"pattern", IoRegex_pattern},
		
		// string 
		
		{"setString", IoRegex_setString},
		{"string", IoRegex_string},
		
		// matching
		
		{"resetSearch", IoRegex_resetSearch},
		{"nextMatch", IoRegex_nextMatch},
		{"currentMatch", IoRegex_currentMatch},
		{"numberOfCaptures", IoRegex_numberOfCaptures},
				
		// compile options
		
		{"isCaseInsensitive", IoRegex_isCaseInsensitive},
		{"setIsCaseInsensitive", IoRegex_setIsCaseInsensitive},
		{"isDollarEndOnly", IoRegex_isDollarEndOnly},
		{"setIsDollarEndOnly", IoRegex_setIsDollarEndOnly},
		{"isDotAll", IoRegex_isDotAll},
		{"setIsDotAll", IoRegex_setIsDotAll},
		{"isExtended", IoRegex_isExtended},
		{"setIsExtended", IoRegex_setIsExtended},
		{"isMultiline", IoRegex_isMultiline},
		{"setIsMultiline", IoRegex_setIsMultiline},
		{"isGreedy", IoRegex_isGreedy},
		{"setIsGreedy", IoRegex_setIsGreedy},
		{"isUTF8", IoRegex_isUTF8},
		{"setIsUTF8", IoRegex_setIsUTF8},
		
		// exec options 
		
		{"canMatchEmpty", IoRegex_canMatchEmpty},
		{"setCanMatchEmpty", IoRegex_setCanMatchEmpty},
		
		// misc
		
		{"version", IoRegex_version},
		{NULL, NULL},
		};
		
		IoObject_addMethodTable_(self, methodTable);
	}
	IoRegex_justUpdatePattern(self, NULL);
	
	return self;
}

/* ----------------------------------------------------------- */

IoRegex *IoRegex_rawClone(IoRegex *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoRegexData)));
	
	DATA(self)->re = NULL;
	DATA(self)->pe = NULL;
	DATA(self)->outputVector = NULL;
	DATA(self)->outputVectorSize = 0;
	
	DATA(self)->pattern = DATA(proto)->pattern;
	DATA(self)->string = DATA(proto)->string;
	
	IoRegex_justUpdatePattern(self, NULL);
	
	if (DATA(proto)->currentMatch)
		DATA(self)->currentMatch = IoObject_rawClone(DATA(proto)->currentMatch);
	
	return self;
}

IoRegex *IoRegex_new(void *state)
{
	IoRegex *proto = IoState_protoWithInitFunction_(state, IoRegex_proto);
	return IOCLONE(proto);
}

void IoRegex_free(IoRegex *self)
{
	if (DATA(self)->re)
	{
		pcre_free(DATA(self)->re);
	}
	
	free(DATA(self)->outputVector);
	free(IoObject_dataPointer(self));
}

void IoRegex_mark(IoRegex *self)
{
	IoObject_shouldMark((IoObject *)DATA(self)->pattern);
	IoObject_shouldMark((IoObject *)DATA(self)->string);
	IoObject_shouldMark((IoObject *)DATA(self)->getMatchProtoMessage);
	IoObject_shouldMark((IoObject *)DATA(self)->cloneMatchMessage);
	IoObject_shouldMark((IoObject *)DATA(self)->addCaptureRangeMessage);
	if (DATA(self)->currentMatch)
		IoObject_shouldMark((IoObject *)DATA(self)->currentMatch);
}

/* ----------------------------------------------------------- */

IoObject *IoRegex_setPattern(IoRegex *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->pattern = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
	IoRegex_justUpdatePattern(self, m);
	return self;
}

void IoRegex_justUpdatePattern(IoRegex *self, IoMessage *m)
{
	char *ex;
	const char *error;
	int erroffset;
	int captures = 0;
	
	IoRegex_resetSearch(self, NULL, m);
	
	ex = IoSeq_asCString(DATA(self)->pattern);
	
	if (DATA(self)->re)
	{
		pcre_free(DATA(self)->re);
	}
	
	DATA(self)->re = pcre_compile(ex, // the pattern
		0,                            // default execOptions
		&error,                       // for error message
		&erroffset,                   // for error offset
		NULL);                        // use default character tables
	
	if (!DATA(self)->re)
	{
		IoState_error_(IOSTATE, m, "RegularExpression compile: Unable to compile '%s' - ", ex, error);
	}
	
	// study pattern
	
	DATA(self)->pe = pcre_study(DATA(self)->re, 0, &error);
	
	if (error)
	{
		IoState_error_(IOSTATE, m, "RegularExpression study: Unable to study '%s' - %s", ex, error);
	}
	
	/* -- create output vector --- */
	pcre_fullinfo(DATA(self)->re, DATA(self)->pe, PCRE_INFO_CAPTURECOUNT, &captures);
	DATA(self)->numberOfCaptures = captures;
	
	{
		int newSize = (captures + 1) * 3;
		
		if (DATA(self)->outputVectorSize != newSize)
		{
			DATA(self)->outputVectorSize = newSize;
			DATA(self)->outputVector = (int *)realloc(DATA(self)->outputVector, sizeof(int)*newSize);
			memset(DATA(self)->outputVector, 0, sizeof(int)*newSize);
		}
	}
}

IoObject *IoRegex_pattern(IoRegex *self, IoObject *locals, IoMessage *m)
{
	return DATA(self)->pattern;
}

IoObject *IoRegex_setString(IoRegex *self, IoObject *locals, IoMessage *m)
{
	IoRegex_resetSearch(self, locals, m);
	DATA(self)->string = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
	return self;
}

IoObject *IoRegex_string(IoRegex *self, IoObject *locals, IoMessage *m)
{
	return DATA(self)->string;
}

IoObject *IoRegex_nextMatch(IoRegex *self, IoObject *locals, IoMessage *m)
{
	char *s = CSTRING(DATA(self)->string);
	int length = IoSeq_rawSize(DATA(self)->string);
	int rc;
	
	if (!DATA(self)->re)
	{
		IoState_error_(IOSTATE, NULL, "Regex findNextMatch: no valid pattern set\n");
	}
	
	rc = pcre_exec(
		DATA(self)->re,                 /* the compiled pattern */
		DATA(self)->pe,                 /* studied pattern */
		(const char *)s,                /* the subject string */
		length,                         /* the length of the subject */
		DATA(self)->lastIndex,          /* start at offset in the subject */
		DATA(self)->execOptions,        /* default execOptions */
		DATA(self)->outputVector,       /* output vector for substring information */
		DATA(self)->outputVectorSize);  /* number of elements in the output vector */
	
	if (rc < 0)
	{
		DATA(self)->matchGroupSize = 0;
		if (rc == PCRE_ERROR_NOMATCH) return IONIL(self);
		IoState_error_(IOSTATE, NULL, "Regex findNextMatch: Matching error %d\n", rc);
	}
	DATA(self)->matchGroupSize = rc;
	
	if (DATA(self)->outputVectorSize >= 2)
	{
		DATA(self)->lastIndex = DATA(self)->outputVector[1];
	}
	
	DATA(self)->currentMatch = NULL;
	return IoRegex_currentMatch(self, locals, m);
}

IoObject *IoRegex_currentMatch(IoRegex *self, IoObject *locals, IoMessage *m)
{
	IoObject *proto, *match;
	IoMessage *addRangeMessage = IOREF(DATA(self)->addCaptureRangeMessage);
	int i, matchGroupSize = DATA(self)->matchGroupSize;
	int *ov = DATA(self)->outputVector, *range;
	
	if (DATA(self)->currentMatch) return DATA(self)->currentMatch;
	if (matchGroupSize <= 0) return IONIL(self);
	
	// Get the match data prototype from self
	proto = IoMessage_locals_performOn_(DATA(self)->getMatchProtoMessage, self, self);
	
	// Clone the prototype and supply the string using cloneWithSubject
	IoMessage_setCachedArg_to_(DATA(self)->cloneMatchMessage, 0, DATA(self)->string);
	match = IoMessage_locals_performOn_(DATA(self)->cloneMatchMessage, proto, proto);
	
	// Notify the match of the ranges for each capture
	range = ov;
	for (i = 0; i < matchGroupSize; i++)
	{
		IoMessage_setCachedArg_toInt_(addRangeMessage, 0, range[0]); // Start index
		IoMessage_setCachedArg_toInt_(addRangeMessage, 1, range[1]); // End index
		IoMessage_locals_performOn_(addRangeMessage, match, match);
		range += 2; // Move past the start and end indices
	}
	
	DATA(self)->currentMatch = IOREF(match);
	return match;
}

IoObject *IoRegex_resetSearch(IoRegex *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->lastIndex = 0;
	DATA(self)->currentMatch = NULL;
	DATA(self)->matchGroupSize = 0;
	return self;
}

IoObject *IoRegex_numberOfCaptures(IoRegex *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->numberOfCaptures);
}

IoObject *IoRegex_version(IoRegex *self, IoObject *locals, IoMessage *m)
{
	return IOSYMBOL((char *)pcre_version());
}


// compile execOptions ----------------------------------- 

IoObject *IoRegex_isCaseInsensitive(IoRegex *self, IoObject *locals, IoMessage *m)
{
	return (DATA(self)->compileOptions & PCRE_CASELESS) ? IOTRUE(self) : IOFALSE(self);
}

IoRegex *IoRegex_setIsCaseInsensitive(IoRegex *self, IoObject *locals, IoMessage *m)
{
	if (ISTRUE((IoObject*)IoMessage_locals_valueArgAt_(m, locals, 0)))
		DATA(self)->compileOptions |= PCRE_CASELESS;
	else
		DATA(self)->compileOptions &= ~PCRE_CASELESS;
	return self;
}

IoObject *IoRegex_isDollarEndOnly(IoRegex *self, IoObject *locals, IoMessage *m)
{
	return (DATA(self)->compileOptions & PCRE_DOLLAR_ENDONLY) ? IOTRUE(self) : IOFALSE(self);
}
IoRegex *IoRegex_setIsDollarEndOnly(IoRegex *self, IoObject *locals, IoMessage *m)
{
	if (ISTRUE(IoMessage_locals_valueArgAt_(m, locals, 0)))
		DATA(self)->compileOptions |= PCRE_DOLLAR_ENDONLY;
	else
		DATA(self)->compileOptions &= ~PCRE_DOLLAR_ENDONLY;
	return self;
}

IoObject *IoRegex_isDotAll(IoRegex *self, IoObject *locals, IoMessage *m)
{
	return (DATA(self)->compileOptions & PCRE_DOTALL) ? IOTRUE(self) : IOFALSE(self);
}

IoRegex *IoRegex_setIsDotAll(IoRegex *self, IoObject *locals, IoMessage *m)
{
	if (ISTRUE(IoMessage_locals_valueArgAt_(m, locals, 0)))
		DATA(self)->compileOptions |= PCRE_DOTALL;
	else
		DATA(self)->compileOptions &= ~PCRE_DOTALL;
	return self;
}

IoObject *IoRegex_isExtended(IoRegex *self, IoObject *locals, IoMessage *m)
{
	return (DATA(self)->compileOptions & PCRE_EXTENDED) ? IOTRUE(self) : IOFALSE(self);
}

IoRegex *IoRegex_setIsExtended(IoRegex *self, IoObject *locals, IoMessage *m)
{
	if (ISTRUE(IoMessage_locals_valueArgAt_(m, locals, 0)))
		DATA(self)->compileOptions |= PCRE_EXTENDED;
	else
		DATA(self)->compileOptions &= ~PCRE_EXTENDED;
	return self;
}

IoObject *IoRegex_isMultiline(IoRegex *self, IoObject *locals, IoMessage *m)
{
	return (DATA(self)->compileOptions & PCRE_MULTILINE) ? IOTRUE(self) : IOFALSE(self);
}

IoRegex *IoRegex_setIsMultiline(IoRegex *self, IoObject *locals, IoMessage *m)
{
	if (ISTRUE(IoMessage_locals_valueArgAt_(m, locals, 0)))
		DATA(self)->compileOptions |= PCRE_MULTILINE;
	else
		DATA(self)->compileOptions &= ~PCRE_MULTILINE;
	return self;
}

IoObject *IoRegex_isGreedy(IoRegex *self, IoObject *locals, IoMessage *m)
{
	return (DATA(self)->compileOptions & PCRE_UNGREEDY) ? IOTRUE(self) : IOFALSE(self);
}

IoRegex *IoRegex_setIsGreedy(IoRegex *self, IoObject *locals, IoMessage *m)
{
	if (ISTRUE(IoMessage_locals_valueArgAt_(m, locals, 0)))
		DATA(self)->compileOptions |= PCRE_UNGREEDY;
	else
		DATA(self)->compileOptions &= ~PCRE_UNGREEDY;
	return self;
}

IoObject *IoRegex_isUTF8(IoRegex *self, IoObject *locals, IoMessage *m)
{
	return (DATA(self)->compileOptions & PCRE_UTF8) ? IOTRUE(self) : IOFALSE(self);
}

IoRegex *IoRegex_setIsUTF8(IoRegex *self, IoObject *locals, IoMessage *m)
{
	if (ISTRUE(IoMessage_locals_valueArgAt_(m, locals, 0)))
		DATA(self)->compileOptions |= PCRE_UTF8;
	else
		DATA(self)->compileOptions &= ~PCRE_UTF8;
	return self;
}


// exec execOptions ----------------------------------- 

IoObject *IoRegex_canMatchEmpty(IoRegex *self, IoObject *locals, IoMessage *m)
{
	return (DATA(self)->execOptions & PCRE_NOTEMPTY) ? IOTRUE(self) : IOFALSE(self);
}
IoRegex *IoRegex_setCanMatchEmpty(IoRegex *self, IoObject *locals, IoMessage *m)
{
	if (ISTRUE(IoMessage_locals_valueArgAt_(m, locals, 0)))
		DATA(self)->execOptions |= PCRE_NOTEMPTY;
	else
		DATA(self)->execOptions &= ~PCRE_NOTEMPTY;
	return self;
}
