
//metadoc Regex copyright Daniel Rosengren danne.rosengren@gmail.com
//metadoc Regex license BSD revised
//metadoc Regex category Parsers
/*metadoc Regex description
	Binding for regular expressions.
*/

#include "IoRegex.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include <stdlib.h>
#include <stdio.h>

#define DATA(self) ((IoRegexData *)IoObject_dataPointer(self))

static const char *protoId = "Regex";


static IoRegex *IoRegex_cloneWithOptions_(IoRegex *self, int options);

IoTag *IoRegex_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
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
	DATA(self)->pattern = IOSYMBOL("");

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
			{"with", IoRegex_with},

			{"pattern", IoRegex_pattern},
			{"captureCount", IoRegex_captureCount},
			{"namedCaptures", IoRegex_namedCaptures},

			{"version", IoRegex_version},

			/* Options */

			{"caseless", IoRegex_caseless},
			{"notCaseless", IoRegex_notCaseless},
			{"isCaseless", IoRegex_isCaseless},

			{"dotAll", IoRegex_dotAll},
			{"notDotAll", IoRegex_notDotAll},
			{"isDotAll", IoRegex_isDotAll},

			{"extended", IoRegex_extended},
			{"notExtended", IoRegex_notExtended},
			{"isExtended", IoRegex_isExtended},

			{"multiline", IoRegex_multiline},
			{"notMultiline", IoRegex_notMultiline},
			{"isMultiline", IoRegex_isMultiline},

			{0, 0},
		};

		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoRegex *IoRegex_rawClone(IoRegex *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoRegexData)));
	DATA(self)->pattern = IOREF(DATA(proto)->pattern);
	return self;
}

IoRegex *IoRegex_newWithPattern_(void *state, IoSymbol *pattern)
{
	IoRegex *self = IOCLONE(IoState_protoWithId_(state, protoId));
	DATA(self)->pattern = IOREF(pattern);
	return self;
}

void IoRegex_free(IoRegex *self)
{
	if (DATA(self)->regex)
	{
		Regex_free(DATA(self)->regex);
	}
	
	free(DATA(self));
}

void IoRegex_mark(IoRegex *self)
{
	IoObject_shouldMark(DATA(self)->pattern);
	
	if (DATA(self)->namedCaptures)
	{
		IoObject_shouldMark(DATA(self)->namedCaptures);
	}
}


Regex *IoRegex_rawRegex(IoRegex *self)
{
	Regex *regex = DATA(self)->regex;
	char *error = 0;

	if (regex)
	{
		return regex;
	}
	
	DATA(self)->regex = regex = Regex_newFromPattern_withOptions_(
		CSTRING(DATA(self)->pattern),
		DATA(self)->options
	);

	error = (char *)Regex_error(regex);
	if(error)
	{
		IoState_error_(IOSTATE, 0, error);
	}
	
	return regex;
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoRegex_with(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex with(pattern)
	Returns a new Regex created from the given pattern string.
	*/
	
	return IoRegex_newWithPattern_(IOSTATE, IoMessage_locals_symbolArgAt_(m, locals, 0));
}


IoObject *IoRegex_pattern(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex pattern
	Returns the pattern string that the receiver was created from.
	*/
	
	return DATA(self)->pattern;
}

IoObject *IoRegex_captureCount(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex captureCount
	Returns the number of captures defined by the pattern.
	*/
	
	return IONUMBER(IoRegex_rawRegex(self)->captureCount);
}

IoObject *IoRegex_namedCaptures(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex namedCaptures
	Returns a Map that contains the index of each named group.
	*/
	
	IoMap *map = DATA(self)->namedCaptures;
	NamedCapture *namedCaptures = 0, *capture = 0;

	if (map)
		return map;

	map = DATA(self)->namedCaptures = IOREF(IoMap_new(IOSTATE));

	capture = namedCaptures = Regex_namedCaptures(IoRegex_rawRegex(self));
	
	if (!namedCaptures)
		return map;

	while (capture->name) 
	{
		IoMap_rawAtPut(map, IOSYMBOL(capture->name), IONUMBER(capture->index));
		capture++;
	}
	
	free(namedCaptures);
	return map;
}


IoObject *IoRegex_version(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex version
	Returns a string with PCRE version information.
	*/
	
	return IOSYMBOL(pcre_version());
}


/* ------------------------------------------------------------------------------------------------*/
/* Options */

IoObject *IoRegex_caseless(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex caseless
	Returns a case insensitive clone of the receiver, or self if the receiver itself is
	case insensitive:

	<pre>	
	Io> "WORD" matchesRegex("[a-z]+")
	==> false

	Io> "WORD" matchesRegex("[a-z]+" asRegex caseless)
	==> true
	</pre>	
*/
	
	return IoRegex_cloneWithOptions_(self, DATA(self)->options | PCRE_CASELESS);
}

IoObject *IoRegex_notCaseless(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex notCaseless
	The reverse of caseless.
	*/
	
	return IoRegex_cloneWithOptions_(self, DATA(self)->options & ~PCRE_CASELESS);
}

IoObject *IoRegex_isCaseless(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex isCaseless
	Returns true if the receiver is case insensitive, false if not.
	*/
	
	return IOBOOL(self, DATA(self)->options & PCRE_CASELESS);
}


IoObject *IoRegex_dotAll(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex dotAll
	<p>Returns a clone of the receiver with the dotall option turned on,
	or self if the receiver itself has the option turned on.</p>

	<p>In dotall mode, "." matches any character, including newline. By default
	it matches any character <em>except</em> newline.</p>

	<pre>	
	Io> "A\nB" matchesOfRegex(".+") next string
	==> A

	Io> "A\nB" matchesOfRegex(".+" asRegex dotAll) next string
	==> A\nB
	</pre>	
*/
	return IoRegex_cloneWithOptions_(self, DATA(self)->options | PCRE_DOTALL);
}

IoObject *IoRegex_notDotAll(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex notDotAll
	The reverse of dotAll.
	*/
	
	return IoRegex_cloneWithOptions_(self, DATA(self)->options & ~PCRE_DOTALL);
}

IoObject *IoRegex_isDotAll(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex isDotAll
	Returns true if the receiver is in dotall mode, false if not.
	*/
	
	return IOBOOL(self, DATA(self)->options & PCRE_DOTALL);
}


IoObject *IoRegex_extended(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex extended
	<p>Returns a clone of the receiver with the extended option turned on,
	or self if the receiver itself has the option turned on.</p>

	<p>In extended mode, a Regex ignores any whitespace character in the pattern	except
	when escaped or inside a character class. This allows you to write clearer patterns
	that may be broken up into several lines.</p>

	<p>Additionally, you can put comments in the pattern. A comment starts with a "#"
	character and continues to the end of the line, unless the "#" is escaped or is
	inside a character class.</p>
	*/
	
	return IoRegex_cloneWithOptions_(self, DATA(self)->options | PCRE_EXTENDED);
}

IoObject *IoRegex_notExtended(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex notExtended
	The reverse of extended.
	*/
	
	return IoRegex_cloneWithOptions_(self, DATA(self)->options & ~PCRE_EXTENDED);
}

IoObject *IoRegex_isExtended(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex isExtended
	Returns true if the receiver is in extended mode, false if not.
	*/
	return IOBOOL(self, DATA(self)->options & PCRE_EXTENDED);
}


IoObject *IoRegex_multiline(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex multiline
	<p>Returns a clone of the receiver with the multiline option turned on,
	or self if the receiver itself has the option turned on.</p>

	<p>In multiline mode, "^" matches at the beginning of the string and at
	the beginning of each line; and "$" matches at the end of the string,
	and at the end of each line.
	By default "^" only matches at the beginning of the string, and "$"
	only matches at the end of the string.</p>

	<pre>	
	Io> "A\nB\nC" allMatchesForRegex("^.")
	==> list("A")

	Io> "A\nB\nC" allMatchesForRegex("^." asRegex multiline)
	==> list("A", "B", "C")
	</pre>	
*/
	
	return IoRegex_cloneWithOptions_(self, DATA(self)->options | PCRE_MULTILINE);
}

IoObject *IoRegex_notMultiline(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex notMultiline
	The reverse of multiline.
	*/
	
	return IoRegex_cloneWithOptions_(self, DATA(self)->options & ~PCRE_MULTILINE);
}

IoObject *IoRegex_isMultiline(IoRegex *self, IoObject *locals, IoMessage *m)
{
	/*doc Regex isMultiline
	Returns true if the receiver is in multiline mode, false if not.
	*/
	
	return IOBOOL(self, DATA(self)->options & PCRE_MULTILINE);
}


/* Private */

static IoRegex *IoRegex_cloneWithOptions_(IoRegex *self, int options)
{
	IoRegex *clone = 0;

	if (options == DATA(self)->options)
		return self;

	clone = IOCLONE(self);
	DATA(clone)->options = options;
	return clone;
}
