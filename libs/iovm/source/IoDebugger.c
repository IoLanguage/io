
// metadoc Debugger category Debug
// metadoc Debugger copyright Steve Dekorte 2002
// metadoc Debugger license BSD revised
/*metadoc Debugger description
Contains methods related to the IoVM debugger.
*/

/*cmetadoc Debugger description
C side of the Debugger object is intentionally tiny — it is a plain
Io object with only a "type" slot stamped here; all stepping logic
lives in Io-level code. The real wire-up happens in IoCoroutine.c:
when a coroutine has debuggingOn, IoObject_performWithDebugger
populates slots like message, messageSelf, messageLocals,
messageCoroutine on this proto and resumes the coroutine stored in
the Io-level `debuggerCoroutine` slot. That makes the debugger a
regular Io coroutine paused in a receive loop, woken once per message
send of the target coroutine.
*/

#include "IoDebugger.h"
#include "IoMessage_parser.h"

/*cdoc Debugger IoDebugger_proto(state)
Creates the Debugger proto: an empty IoObject with a `type` slot set
to "Debugger". The method table is empty because all debugger methods
are defined in Io (libs/iovm/io/Debugger.io). IoState stashes a
pointer to this object in state->debugger so the coroutine trampoline
in IoCoroutine.c can find it without a slot lookup.
*/
IoObject *IoDebugger_proto(void *state) {
    IoMethodTable methodTable[] = {
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);
    IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("Debugger"));
    IoObject_addMethodTable_(self, methodTable);
    return self;
}
