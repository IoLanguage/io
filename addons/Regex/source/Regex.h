#ifndef REGEX_DEFINED
#define REGEX_DEFINED 1

#include "UArray.h"
#include <pcre.h>

typedef struct
{
	pcre *code;
	pcre_extra *studyData;
	int captureCount;
	UArray *error;
} Regex;

typedef struct
{
	const char *name;
	int index;
} NamedCapture;

Regex *Regex_newFromPattern_withOptions_(const char *pattern, int options);
int Regex_search_from_to_withOptions_captureArray_(
	Regex *self,
	const char *string, int start, int end, int options,
	UArray *captureArray
);

NamedCapture *Regex_namedCaptures(Regex *self);

const char *Regex_error(Regex *self);

#endif
