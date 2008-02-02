
//metadoc Fnmatch copyright Steve Dekorte, 2004
//metadoc Fnmatch license BSD revised
/*metadoc Fnmatch description
The fnmatch add on adds support for the unix fnmatch function. 
(See fnmatch man page for details). 
Note: not all options are supported on all platforms.
*/

//metadoc Fnmatch category RegularExpressions

#include "IoFnmatch.h"
#include "IoState.h"
#include "IoSeq.h"
#include <fnmatch.h>

#define DATA(self) ((IoFnmatchData *)IoObject_dataPointer(self))

IoTag *IoFnmatch_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Fnmatch");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoFnmatch_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoFnmatch_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoFnmatch_rawClone);
	return tag;
}

IoFnmatch *IoFnmatch_proto(void *state)
{
	IoFnmatch *self = IoObject_new(state);
	IoObject_tag_(self, IoFnmatch_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoFnmatchData)));
	DATA(self)->string = IOSYMBOL("");
	DATA(self)->pattern = DATA(self)->string;
	IoState_registerProtoWithFunc_(state, self, IoFnmatch_proto);

	{
	IoMethodTable methodTable[] = {
	{"pattern", IoFnmatch_pattern},
	{"setPattern", IoFnmatch_setPattern},

	{"string", IoFnmatch_string},
	{"setString", IoFnmatch_setString},

	{"hasMatch", IoFnmatch_hasMatch},
	{"matchFor", IoFnmatch_matchFor},

#ifdef FNM_NOESCAPE
	{"noEscapeOn", IoFnmatch_noEscapeOn},
	{"noEscapeOff", IoFnmatch_noEscapeOff},
#endif

#ifdef FNM_PATHNAME
	{"pathNameOn", IoFnmatch_pathNameOn},
	{"pathNameOff", IoFnmatch_pathNameOff},
#endif

#ifdef FNM_PERIOD
	{"periodOn", IoFnmatch_periodOn},
	{"periodOff", IoFnmatch_periodOff},
#endif

#ifdef FNM_LEADING_DIR
	{"leadingDirOn", IoFnmatch_leadingDirOn},
	{"leadingDirOff", IoFnmatch_leadingDirOff},
#endif

#ifdef FNM_CASEFOLD
	{"caseFoldOn", IoFnmatch_caseFoldOn},
	{"caseFoldOff", IoFnmatch_caseFoldOff},
#endif
	{NULL, NULL},
	};
	IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

/* ----------------------------------------------------------- */

IoFnmatch *IoFnmatch_rawClone(IoFnmatch *proto)
{
	IoFnmatch *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoFnmatchData)));
	/*
	DATA(self)->pattern = DATA(proto)->pattern;
	DATA(self)->string  = DATA(proto)->string;
	*/
	return self;
}

IoFnmatch *IoFnmatch_new(void *state)
{
	IoFnmatch *proto = IoState_protoWithInitFunction_(state, IoFnmatch_proto);
	return IOCLONE(proto);
}

void IoFnmatch_free(IoFnmatch *self)
{
	free(IoObject_dataPointer(self));
}

void IoFnmatch_mark(IoFnmatch *self)
{
	IoObject_shouldMark(DATA(self)->pattern);
	IoObject_shouldMark(DATA(self)->string);
}

/* ----------------------------------------------------------- */

IoObject *IoFnmatch_pattern(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch pattern
	Returns the pattern string.
	*/

	return DATA(self)->pattern;
}

IoObject *IoFnmatch_setPattern(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch setPattern(aString)
	Sets the pattern string. Returns self.
	*/

	DATA(self)->pattern = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
	return self;
}

IoObject *IoFnmatch_string(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch string
	The string to do matching on.
	*/

	return DATA(self)->string;
}

IoObject *IoFnmatch_setString(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch setString(aString)
	Sets the string to do matching on.
	*/

	DATA(self)->string = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
	return self;
}

IoObject *IoFnmatch_hasMatch(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch hasMatch
	Returns true if a match is found, false otherwise.
	*/

	char *pattern = IoSeq_asCString(DATA(self)->pattern);
	char *string = IoSeq_asCString(DATA(self)->string);
	//int e = fnmatch(pattern, string, DATA(self)->flags) == 0;
	//return IOBOOL(self, e);
	return IOBOOL(self, fnmatch(pattern, string, DATA(self)->flags) == 0);
}

IoObject *IoFnmatch_matchFor(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch matchFor(aString)
	Returns self if aString matches the pattern, otherwise returns nil.
	*/

	IoFnmatch_setString(self, locals, m);
	return IoFnmatch_hasMatch(self, locals, m);
}

#ifdef FNM_NOESCAPE
IoObject *IoFnmatch_noEscapeOn(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch noEscapeOn
	If not set, a backslash character (\) in pattern followed by any other 
	character will match that second character in string. 
	In particular, "\\" will match a backslash in string. 
	If set, a backslash character will be treated as an ordinary character.
	*/

	DATA(self)->flags |= FNM_NOESCAPE;
	return self;
}

IoObject *IoFnmatch_noEscapeOff(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch noEscapeOff
	See noEscapeOn.
	*/

	DATA(self)->flags &= ~FNM_NOESCAPE;
	return self;
}

#endif

#ifdef FNM_PATHNAME
IoObject *IoFnmatch_pathNameOn(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch pathNameOn
	If set, a slash (/) character in string will be explicitly matched 
	by a slash in pattern; it will not be matched by either the asterisk (*) 
	or question-mark (?) special characters, nor by a bracket ([]) expression. 
	If not set, the slash character is treated as an ordinary character.
	*/

	DATA(self)->flags |= FNM_PATHNAME;
	return self;
}

IoObject *IoFnmatch_pathNameOff(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch pathNameOff
	See pathNameOn.
	*/

	DATA(self)->flags &= ~FNM_PATHNAME;
	return self;
}
#endif

#ifdef FNM_PERIOD
IoObject *IoFnmatch_periodOn(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch periodOn
	If set, a leading period in string will match a period in pattern; where the location of ``leading'' is indicated by the value of FNM_PATHNAME:
<UL>
<LI>If FNM_PATHNAME is set, a period is ``leading'' if it is the first character in string or if it immediately follows a slash.

<LI>If FNM_PATHNAME is not set, a period is ``leading'' only if it is the first character of string.
</UL>
<p>
If not set, no special restrictions are placed on matching a period.
	*/

	DATA(self)->flags |= FNM_PERIOD;
	return self;
}

IoObject *IoFnmatch_periodOff(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch periodOff
	"See periodOn.
	*/

	DATA(self)->flags &= ~FNM_PERIOD;
	return self;
}
#endif

#ifdef FNM_LEADING_DIR
IoObject *IoFnmatch_leadingDirOn(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch leadingDirOn
	Ignore rest after successful pattern matching.
	*/

	DATA(self)->flags |= FNM_LEADING_DIR;
	return self;
}

IoObject *IoFnmatch_leadingDirOff(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch leadingDirOff
	See leadingDirOn.
	*/

	DATA(self)->flags &= ~FNM_LEADING_DIR;
	return self;
}
#endif

#ifdef FNM_CASEFOLD
IoObject *IoFnmatch_caseFoldOn(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch caseFoldOn
	Ignore case distinctions in both the pattern and the string.
	*/

	DATA(self)->flags |= FNM_CASEFOLD;
	return self;
}

IoObject *IoFnmatch_caseFoldOff(IoFnmatch *self, IoObject *locals, IoMessage *m)
{
	/*doc Fnmatch caseFoldOff
	See caseFoldOn.
	*/

	DATA(self)->flags &= ~FNM_CASEFOLD;
	return self;
}

#endif


