/*#io
Regex ioDoc(
	docCopyright("Steve Dekorte", 2005)
	docCopyright("Daniel Rosengren", 2007)
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

#include "IoRegex.h"
#include "IoMatch.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include <stdlib.h>

#define DATA(self) ((IoRegexData *)IoObject_dataPointer(self))

static void IoRegex_justUpdatePattern(IoRegex *self, IoMessage *m);
static void IoRegex_compilePattern(IoRegex *self, IoMessage *m);
static void IoRegex_createOutputVector(IoRegex *self);


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

	IoState_registerProtoWithFunc_(state, self, IoRegex_proto);
	
	{
		IoMethodTable methodTable[] = {
			/* Pattern */
		
			{"setPattern", IoRegex_setPattern},
			{"pattern", IoRegex_pattern},
		
			/* String */ 
		
			{"setString", IoRegex_setString},
			{"string", IoRegex_string},
		
			/* Matching */
		
			{"resetSearch", IoRegex_resetSearch},
			{"nextMatch", IoRegex_nextMatch},
			{"currentMatch", IoRegex_currentMatch},
			{"captureCount", IoRegex_captureCount},
				
			/* Compile options */
			
			{"anchoredOn", IoRegex_anchoredOn},
			{"anchoredOff", IoRegex_anchoredOff},
		
			{"caselessOn", IoRegex_caselessOn},
			{"caselessOff", IoRegex_caselessOff},

			{"dotAllOn", IoRegex_dotAllOn},
			{"dotAllOff", IoRegex_dotAllOff},

			{"extendedOn", IoRegex_extendedOn},
			{"extendedOff", IoRegex_extendedOff},

			{"multilineOn", IoRegex_multilineOn},
			{"multilineOff", IoRegex_multilineOff},

			{"UTF8On", IoRegex_UTF8On},
			{"UTF8Off", IoRegex_UTF8Off},
		
			/* Match options */ 
		
			{"matchEmptyOn", IoRegex_matchEmptyOn},
			{"matchEmptyOff", IoRegex_matchEmptyOff},
		
			/* Misc */
		
			{"version", IoRegex_version},

			{0, 0},
		};
		
		IoObject_addMethodTable_(self, methodTable);
	}

	IoRegex_justUpdatePattern(self, 0);

	return self;
}

IoRegex *IoRegex_rawClone(IoRegex *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoRegexData)));
	
	DATA(self)->regex = 0;
	DATA(self)->studyData = 0;
	DATA(self)->outputVector = 0;
	DATA(self)->outputVectorSize = 0;
	
	DATA(self)->pattern = DATA(proto)->pattern;
	DATA(self)->string = DATA(proto)->string;
	
	IoRegex_justUpdatePattern(self, 0);
	
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
	if (DATA(self)->regex)
		pcre_free(DATA(self)->regex);
	
	free(DATA(self)->outputVector);
	free(DATA(self));
}

void IoRegex_mark(IoRegex *self)
{
	IoObject_shouldMark((IoObject *)DATA(self)->pattern);
	IoObject_shouldMark((IoObject *)DATA(self)->string);
	if (DATA(self)->currentMatch)
		IoObject_shouldMark((IoObject *)DATA(self)->currentMatch);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoRegex_setPattern(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setPattern(aString)",
		"Sets the pattern to be used for finding matches. Returns self.")
	*/
	DATA(self)->pattern = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
	IoRegex_justUpdatePattern(self, m);
	return self;
}

IoObject *IoRegex_pattern(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("pattern",
		"Returns the current pattern string. Returns self.")
	*/
	return DATA(self)->pattern;
}


IoObject *IoRegex_setString(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setString(aString)",
		"Sets the string to find matches in. Returns self.")
	*/
	IoRegex_resetSearch(self, locals, m);
	DATA(self)->string = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
	return self;
}

IoObject *IoRegex_string(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("string",
		"Returns the string that the Regex finds matches in.")
	*/
	return DATA(self)->string;
}


IoObject *IoRegex_nextMatch(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("nextMatch",
		"Returns the next match, or nil if there is none.")
	*/
	char *string = CSTRING(DATA(self)->string);
	int length = IoSeq_rawSize(DATA(self)->string);
	int returnCode;
	
	if (DATA(self)->compileOptions != DATA(self)->previousCompileOptions)
	{
		/* The compile options have changed since last call pcre_exec; recompile the regex */
		IoRegex_compilePattern(self, m);
	}
	
	if (!DATA(self)->regex)
		IoState_error_(IOSTATE, 0, "Regex nextMatch: no valid pattern set\n");

	returnCode = pcre_exec(
		DATA(self)->regex,
		DATA(self)->studyData,
		(const char *)string,
		length,
		DATA(self)->lastIndex,
		DATA(self)->execOptions,
		DATA(self)->outputVector, 
		DATA(self)->outputVectorSize
	);  
	
	if (returnCode < 0)
	{
		DATA(self)->matchGroupSize = 0;
		if (returnCode == PCRE_ERROR_NOMATCH) return IONIL(self);
		IoState_error_(IOSTATE, 0, "Regex nextMatch: Matching error %d\n", returnCode);
	}
	DATA(self)->matchGroupSize = returnCode;
	
	if (DATA(self)->outputVectorSize >= 2)
		DATA(self)->lastIndex = DATA(self)->outputVector[1];
	
	DATA(self)->currentMatch = 0;
	return IoRegex_currentMatch(self, locals, m);
}

IoObject *IoRegex_currentMatch(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("currentMatch",
		"Returns the current match, or nil if there is none.")
	*/
	int matchGroupSize = DATA(self)->matchGroupSize;
	IoObject *match = 0;
	
	if (DATA(self)->currentMatch) return DATA(self)->currentMatch;
	if (matchGroupSize <= 0) return IONIL(self);
	
	match = IoMatch_newFromRegex_(IOSTATE, self);
	DATA(self)->currentMatch = IOREF(match);
	return match;
}

IoObject *IoRegex_resetSearch(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("resetSearch",
		"Clears the match data and resets the search position to the beginning of
		the string. Returns self.")
	*/
	DATA(self)->lastIndex = 0;
	DATA(self)->currentMatch = 0;
	DATA(self)->matchGroupSize = 0;
	return self;
}

IoObject *IoRegex_captureCount(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("captureCount",
		"Returns the number of captures defined by the pattern.")
	*/
	return IONUMBER(DATA(self)->captureCount);
}


IoObject *IoRegex_version(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("version",
		"Returns a string with the version info of the underlying library.")
	*/
	return IOSYMBOL((char *)pcre_version());
}


/* ------------------------------------------------------------------------------------------------*/
/* Compile options */

IoObject *IoRegex_anchoredOn(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("anchoredOn",
		"Turns anchored matching on. When on, the Regex will only at the start of the string.
		Returns self.")
	*/
	DATA(self)->compileOptions |= PCRE_ANCHORED;
	return self;
}

IoObject *IoRegex_anchoredOff(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("anchoredOff",
		"Turns anchored matching off. When off, the regex can find matches anywhere in the string.
		This is the default behaviour.
		Returns self.")
	*/
	DATA(self)->compileOptions &= ~PCRE_ANCHORED;
	return self;
}


IoObject *IoRegex_caselessOn(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("caselessOn",
		"Turns case insensitive matching on. Returns self.")
	*/
	DATA(self)->compileOptions |= PCRE_CASELESS;
	return self;
}

IoObject *IoRegex_caselessOff(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("caselessOff",
		"Turns case insensitive matching off. Returns self.")
	*/
	DATA(self)->compileOptions &= ~PCRE_CASELESS;
	return self;
}


IoObject *IoRegex_dotAllOn(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("dotAllOn",
		"""Makes "." match any character, including newlines. Returns self.""")
	*/
	DATA(self)->compileOptions |= PCRE_DOTALL;
	return self;
}

IoObject *IoRegex_dotAllOff(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("dotAllOff",
		"""Makes "." match any character except newline.  This is the default behaviour.
		Returns self.""")
	*/
	DATA(self)->compileOptions &= ~PCRE_DOTALL;
	return self;
}


IoObject *IoRegex_extendedOn(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("extendedOn",
		"Turns extended mode on. When on, whitespace in patterns are ignored except when escaped
		or inside a character class. Additionally, it lets you embed comments into patterns.
		A comment starts with a "#" and extends to the end of the line, except when the "#" is
		escaped or is inside a character class. 
		
		Returns self.")
	*/
	DATA(self)->compileOptions |= PCRE_EXTENDED;
	return self;
}

IoObject *IoRegex_extendedOff(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("extendedOff",
		"Turns extended mode off. When off, whitespace in patterns is not ignored, and comments are
		not allowed. This is the default behaviour.
		
		Returns self.")
	*/
	DATA(self)->compileOptions &= ~PCRE_EXTENDED;
	return self;
}


IoObject *IoRegex_multilineOn(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("multilineOn", 
		"""Turns multiline search mode on. When on, "^" will match at the beginning of string and at the
		start of each line, and "$" will match at the end of the string and at the end of each line.

		Returns self.""")
	*/
	DATA(self)->compileOptions |= PCRE_MULTILINE;
	return self;
}

IoObject *IoRegex_multilineOff(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("multilineOff", 
		"""Turns multiline search mode off. When off, "^" will only match at the start of the string,
		and "$" will only match at the end of the string. This is the default behaviour.

		Returns self.""")
	*/
	DATA(self)->compileOptions &= ~PCRE_MULTILINE;
	return self;
}


IoObject *IoRegex_UTF8On(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*
	docSlot("UTF8On",
		"Turns UTF8 mode on. When on, the Regex will treat the pattern and the subject as UTF-8 strings.

		Returns self.")
	*/
	DATA(self)->compileOptions |= PCRE_UTF8;
	return self;
}

IoObject *IoRegex_UTF8Off(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*
	docSlot("UTF8Off",
		"Turns UTF8 mode off. Returns self.")
	*/
	DATA(self)->compileOptions &= ~PCRE_UTF8;
	return self;
}


/* ------------------------------------------------------------------------------------------------*/
/* Match options */

IoObject *IoRegex_matchEmptyOn(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*
	docSlot("matchEmptyOn",
		"Specifies that an empty string can be a valid match. This is the default behaviour.
		Returns self.")
	*/
	DATA(self)->execOptions &= ~PCRE_NOTEMPTY;
	return self;
}

IoObject *IoRegex_matchEmptyOff(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*
	docSlot("matchEmptyOff",
		"Specifies that an empty string can not be a valid match. Returns self.")
	*/
	DATA(self)->execOptions |= PCRE_NOTEMPTY;
	return self;
}


/* ------------------------------------------------------------------------------------------------*/
/* Private */

static void IoRegex_justUpdatePattern(IoRegex *self, IoMessage *m)
{
	IoRegex_resetSearch(self, 0, m);
	IoRegex_compilePattern(self, m);
}

static void IoRegex_compilePattern(IoRegex *self, IoMessage *m)
{
	char *pattern;
	const char *error;
	int errorOffset;
	int captureCount = 0;
	
	pattern = CSTRING(DATA(self)->pattern);
	
	if (DATA(self)->regex)
		pcre_free(DATA(self)->regex);
	
	DATA(self)->regex = pcre_compile(
		pattern,
		DATA(self)->compileOptions,
		&error,
		&errorOffset,
		0
	);
	
	if (!DATA(self)->regex)
		IoState_error_(IOSTATE, m, "Unable to compile '%s' - %s", pattern, error);
	
	DATA(self)->studyData = pcre_study(DATA(self)->regex, 0, &error);
	if (error)
		IoState_error_(IOSTATE, m, "Unable to study '%s' - %s", pattern, error);

	DATA(self)->previousCompileOptions = DATA(self)->compileOptions;

	IoRegex_createOutputVector(self);
}

static void IoRegex_createOutputVector(IoRegex *self)
{
	int captureCount = 0;
	int newSize = 0;

	pcre_fullinfo(DATA(self)->regex, DATA(self)->studyData, PCRE_INFO_CAPTURECOUNT, &captureCount);
	DATA(self)->captureCount = captureCount;
	
	newSize = (captureCount + 1) * 3;
	if (DATA(self)->outputVectorSize == newSize)
		return;
		
	DATA(self)->outputVectorSize = newSize;
	DATA(self)->outputVector = (int *)realloc(DATA(self)->outputVector, sizeof(int)*newSize);
	memset(DATA(self)->outputVector, 0, sizeof(int)*newSize);
}
