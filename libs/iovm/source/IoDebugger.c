
//metadoc Debugger copyright Steve Dekorte 2002
//metadoc Debugger license BSD revised
/*metadoc Debugger description
Contains methods related to the IoVM debugger.
*/
//metadoc Debugger category Core

#include "IoDebugger.h"
#include "IoMessage_parser.h"

IoObject *IoDebugger_proto(void *state)
{
	IoMethodTable methodTable[] =
	{
	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("Debugger"));
	IoObject_addMethodTable_(self, methodTable);
	return self;
}

