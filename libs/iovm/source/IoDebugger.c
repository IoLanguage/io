/*#io
Debugger ioDoc(
			docCopyright("Steve Dekorte", 2002)
			docLicense("BSD revised")
			docDescription("Contains methods related to the IoVM debugger.")
			docCategory("Core")
*/

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

