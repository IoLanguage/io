/*#io
ReadLine ioDoc(
	docCopyright("Jonas Eschenburg", 2007)
	docLicense("BSD revised")
	docObject("ReadLine")
	docDescription("Binding to GNU readline.")
	docCategory("Core")
*/

#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "IoReadLine.h"


IoTag *IoReadLine_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ReadLine");
	IoTag_state_(tag, state);
	return tag;
}

IoReadLine *IoReadLine_proto(void *state)
{
	IoMethodTable methodTable[] = {
		{"readLine", IoReadLine_readLine},
		{"addHistory", IoReadLine_addHistory},
		{"loadHistory", IoReadLine_loadHistory},
		{"saveHistory", IoReadLine_saveHistory},
		{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoReadLine_newTag(state));

	IoObject_setSlot_to_(self, IOSYMBOL("prompt"), IOSYMBOL(""));

	/* Make sure readline returns characters in the multi-byte charset
	   of the locale */
	setlocale(LC_CTYPE, "");
	/* Actually initialize GNU readly and history */
	rl_initialize();
	using_history();

	IoState_registerProtoWithFunc_((IoState *)state, self, IoReadLine_proto);

	IoObject_addMethodTable_(self, methodTable);

	return self;
}


/* ----------------------------------------------------------- */

IoObject *IoReadLine_readLine(IoReadLine *self, IoObject *locals, IoMessage *m)
{
	const char *prompt = NULL;

	if (IoMessage_argCount(m) == 0) {
		IoObject *p = IoObject_rawGetSlot_(self, IOSYMBOL("prompt"));
		if (p && ISSEQ(p)) {
			prompt = CSTRING(p);
		} else {
			prompt = "";
		}
	} else {
		prompt = IoMessage_locals_cStringArgAt_(m, locals, 0);
	}

	const char *line = readline(prompt);

	if (line)
		return IOSEQ((const unsigned char*)line, strlen(line));
	else
		return IONIL(self);
}

IoObject *IoReadLine_addHistory(IoReadLine *self, IoObject *locals, IoMessage *m)
{
	IoSeq *line = IoMessage_locals_seqArgAt_(m, locals, 0);

	add_history(CSTRING(line));

	return self;
}

IoObject *IoReadLine_loadHistory(IoReadLine *self, IoObject *locals, IoMessage *m)
{
	char *filename = IoMessage_argCount(m) >= 1 ? IoMessage_locals_cStringArgAt_(m, locals, 0) : NULL;
	int errno = read_history(filename);

	if (errno != 0)
		IoState_error_(IOSTATE, m, "while loading history file '%s', reason: %s", filename, strerror(errno));

	return self;
}

IoObject *IoReadLine_saveHistory(IoReadLine *self, IoObject *locals, IoMessage *m)
{
	char *filename = IoMessage_argCount(m) >= 1 ? IoMessage_locals_cStringArgAt_(m, locals, 0) : NULL;
	int errno = write_history(filename);

	if (errno != 0)
		IoState_error_(IOSTATE, m, "while saving history file '%s', reason: %s", filename, strerror(errno));

	return self;
}
