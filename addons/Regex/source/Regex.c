#include "Regex.h"

static int Regex_put_in_(Regex *self, int what, void *where);
//static void Regex_createOutputVector(Regex *self);
static void Regex_error_(Regex *self, char *format, ...);


Regex *Regex_newFromPattern_withOptions_(const char *pattern, int options)
{
	Regex *self = calloc(1, sizeof(Regex));
	const char *error = 0;
	int errorOffset = 0;

	self->code = pcre_compile(
		pattern,
		options,
		&error,
		&errorOffset,
		0
	);

	if (!self->code) {
		Regex_error_(self, "Unable to compile \"%s\" - %s", pattern, error);
		return self;
	}

	self->studyData = pcre_study(self->code, 0, &error);
	if (error) {
		Regex_error_(self, "Unable to study \"%s\" - %s", pattern, error);
		return self;
	}

	Regex_put_in_(self, PCRE_INFO_CAPTURECOUNT, &self->captureCount);
	return self;
}

void Regex_free(Regex *self)
{
	pcre_free(self->code);

	if (self->error)
		UArray_free(self->error);

	free(self);
}

int Regex_search_from_to_withOptions_captureArray_(
	Regex *self,
	const char *string, int start, int end, int options,
	UArray *captureArray
)
{
	int returnCode = pcre_exec(
		self->code,
		self->studyData,

		string,
		end,
		start,

		options,

		(int *)UArray_data(captureArray),
		UArray_size(captureArray)
	);

	if (returnCode < 0) {
		if (returnCode != PCRE_ERROR_NOMATCH)
			Regex_error_(self, "Matching error %d", returnCode);
		returnCode = 0;
	}

	return returnCode;
}


NamedCapture *Regex_namedCaptures(Regex *self)
{
	NamedCapture *namedCaptures = 0;
	NamedCapture *capture = 0;
	int nameCount = 0;
	int entrySize = 0;
	unsigned char *entry = 0;
	int i = 0;

	Regex_put_in_(self, PCRE_INFO_NAMECOUNT, &nameCount);
	if (nameCount <= 0)
		return 0;

	capture = namedCaptures = calloc(nameCount + 1, sizeof(NamedCapture));

	Regex_put_in_(self, PCRE_INFO_NAMEENTRYSIZE, &entrySize);
	Regex_put_in_(self, PCRE_INFO_NAMETABLE, &entry);

	for (i = 0; i < nameCount; i++) {
		capture->name = (const char*)(entry + 2);
		capture->index = (entry[0] << 8) | entry[1];
		capture++;

		entry += entrySize;
	}

	return namedCaptures;
}


const char *Regex_error(Regex *self)
{
	if (!self->error)
		return 0;
	return (const char *)UArray_bytes(self->error);
}


/* ------------------------------------------------------------------------------------------------*/
/* Private */

static int Regex_put_in_(Regex *self, int what, void *where)
{
	return pcre_fullinfo(self->code, self->studyData, what, where);
}

static void Regex_error_(Regex *self, char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	self->error = UArray_newWithVargs_(format, ap);
	va_end(ap);
}
