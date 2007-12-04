/*#io
EditLine ioDoc(
  docCopyright("Jonathan Wright", 2007)
  docLicense("BSD revised")
  docObject("EditLine")
  docDescription("Binding to libedit (BSD version of readline).")
  docCategory("Core")
*/

#include "IoEditLine.h"

#ifdef IO_HAS_EDITLIB
#define DATA(self) ((IoEditLineData *)IoObject_dataPointer(self))
static char *promptCallback(EditLine *e);
#endif


IoTag *IoEditLine_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("EditLine");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoEditLine_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoEditLine_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoEditLine_free);
	return tag;
}

IoEditLine *IoEditLine_proto(void *state)
{
	IoMethodTable methodTable[] = {
		{"hasEditLib", IoEditLine_hasEditLib},
#ifdef IO_HAS_EDITLIB
		{"readLine", IoEditLine_readLine},
		{"addHistory", IoEditLine_addHistory},
#endif
		{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoEditLine_newTag(state));

#ifdef IO_HAS_EDITLIB
	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoEditLineData)));
	DATA(self)->prompt  = IOSYMBOL("");
	DATA(self)->editline = el_init("io", stdin, stdout, stderr);
	DATA(self)->history = history_init();

	el_set(DATA(self)->editline, EL_CLIENTDATA, self);
	el_set(DATA(self)->editline, EL_HIST, history, DATA(self)->history);
	el_set(DATA(self)->editline, EL_PROMPT, promptCallback);
	el_set(DATA(self)->editline, EL_SIGNAL, 1);
	el_set(DATA(self)->editline, EL_EDITOR, "emacs");

	{
		HistEvent ev;
		history(DATA(self)->history, &ev, H_SETSIZE, 300);
	}

	el_source(DATA(self)->editline, NULL);
#endif

	IoState_registerProtoWithFunc_((IoState *)state, self, IoEditLine_proto);

	IoObject_addMethodTable_(self, methodTable);

	return self;
}

IoEditLine *IoEditLine_rawClone(IoEditLine *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	return self;
}

void IoEditLine_mark(IoEditLine *self)
{
#ifdef IO_HAS_EDITLIB
    IoObject_shouldMarkIfNonNull(DATA(self)->prompt);
#endif
}

void IoEditLine_free(IoEditLine *self)
{
	if (IoObject_dataPointer(self))
	{
#ifdef IO_HAS_EDITLIB
		el_end(DATA(self)->editline);
		history_end(DATA(self)->history);
#endif
	}
}

/* ----------------------------------------------------------- */

IoObject *IoEditLine_hasEditLib(IoEditLine *self, IoObject *locals, IoMessage *m)
{
#ifdef IO_HAS_EDITLIB
	return IOTRUE(self);
#else
	return IOFALSE(self);
#endif
}

#ifdef IO_HAS_EDITLIB
char *promptCallback(EditLine *e)
{
	IoEditLine *self;
	el_get(e, EL_CLIENTDATA, &self);
	return CSTRING(DATA(self)->prompt);
}

IoObject *IoEditLine_readLine(IoEditLine *self, IoObject *locals, IoMessage *m)
{
	int count = 0;
	const char *line = NULL;

	DATA(self)->prompt = IoMessage_locals_symbolArgAt_(m, locals, 0);

	line = el_gets(DATA(self)->editline, &count);

	if (line && count >= 0)
		return IOSEQ(line, count);
	else
		return IONIL(self);
}

IoObject *IoEditLine_addHistory(IoEditLine *self, IoObject *locals, IoMessage *m)
{
	IoSeq *line = IoMessage_locals_seqArgAt_(m, locals, 0);
	HistEvent ev;

	history(DATA(self)->history, &ev, H_ENTER, CSTRING(line));

	return self;
}
#endif
