/*
 * iofe.c: Io frontend.
 * Copyright (c) 2004 Kentaro A. Kurahone.  All Rights Reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <histedit.h>

#include "IoState.h"
#include "IoNumber.h"

#define IOFE_VERSION 20041002

static const char fePrompt[] = "Io> ";
static EditLine *editCtxt = NULL;
static History *histCtxt = NULL;

static char* promptCallback(EditLine *edit)
{
	return (char *) fePrompt;
}

static void printCallback(void *state, char *s)
{
	fprintf(stdout, "%s", s);
}

static void exceptionCallback(void *state, IoException *e)
{
	IoException_printBackTrace(e);
}

static void exitCallback(void *state)
{
	/* IoState_free(state); */
	history_end(histCtxt);
	el_end(editCtxt);
	exit(0);
}

int main(int argc, const char *argv[])
{
	IoState *state;
	IoNumber *version;
	IoNumber *distribution;
	IoObject *lobby;
	HistEvent ev;
	
	/* Initialise libedit. */
	editCtxt = el_init(argv[0], stdin, stdout, stderr);
	histCtxt = history_init();
	history(histCtxt, &ev, H_SETSIZE, 10);
	el_set(editCtxt, EL_PROMPT, promptCallback);
	el_set(editCtxt, EL_HIST, history, histCtxt);
	el_set(editCtxt, EL_EDITOR, "emacs");
	
	/* Initialise IoVM. */
	state = IoState_new();
	IoState_callbackContext_(state, NULL);
	IoState_printCallback_(state, printCallback);
	IoState_exceptionCallback_(state, exceptionCallback);
	IoState_exitCallback_(state, exitCallback);
	IoState_argc_argv_(state, argc, argv);
	
	lobby = state->lobby;
	version = IoObject_getSlot_(lobby, IoState_stringWithCString_(state, "version"));
	distribution = IoObject_getSlot_(lobby, IoState_stringWithCString_(state, "distribution"));
	
	IoState_print_(state, "IoFE version %i (%s version %i)\n",
				IOFE_VERSION,
				CSTRING(distribution),
				IoNumber_asInt(version));
	
	while(1) 
	{
		IoObject *result;
		char *buf;
		int count;
		
		buf = (char *) el_gets(editCtxt, &count);
		if (buf) 
		{
			result = IoState_doCString_(state, buf);
			
			if (result) 
			{
				history(histCtxt, &ev, H_ENTER, buf);
				fprintf(stdout, "==> ");
				IoObject_print(result);
				fprintf(stdout, "\n");
			}
		} 
		else 
		{
			fprintf(stdout, "\n");
			exitCallback(state);
		}
	}
	return 0;
}
