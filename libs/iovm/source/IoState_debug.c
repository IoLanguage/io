#include "IoState.h"
#include "IoObject.h"
#include "IoSystem.h"

void IoState_show(IoState *self) {
    printf("--- state ----------------------------------\n");
    printf("State:\n");
    /*
    printf("black:\n");
    IoObjectGroup_show(self->blackGroup);
    printf("\n");

    printf("gray:\n");
    IoObjectGroup_show(self->grayGroup);
    printf("\n");

    printf("white:\n");
    IoObjectGroup_show(self->whiteGroup);
    printf("\n");
    */
    printf("stacks:\n");
    printf("\n");
}

IoObject *IoState_replacePerformFunc_with_(IoState *self,
                                           IoTagPerformFunc *oldFunc,
                                           IoTagPerformFunc *newFunc) {
    POINTERHASH_FOREACH(self->primitives, k, v, {
        IoObject *proto = v;
        IoTag *tag = IoObject_tag(proto);
        if (tag->performFunc == oldFunc || !tag->performFunc) {
            tag->performFunc = newFunc;
        }
    });

    return NULL;
}

void IoState_debuggingOn(IoState *self) {
    IoState_replacePerformFunc_with_(
        self, (IoTagPerformFunc *)IoObject_perform,
        (IoTagPerformFunc *)IoObject_performWithDebugger);
}

void IoState_debuggingOff(IoState *self) {
    IoState_replacePerformFunc_with_(
        self, (IoTagPerformFunc *)IoObject_performWithDebugger,
        (IoTagPerformFunc *)IoObject_perform);
}

int IoState_hasDebuggingCoroutine(IoState *self) {
    return 1; // hack awaiting decision on how to change this
}

void IoState_updateDebuggingMode(IoState *self) {
    if (IoState_hasDebuggingCoroutine(self)) {
        IoState_debuggingOn(self);
    } else {
        IoState_debuggingOff(self);
    }
}

#include <signal.h>

static IoState *stateToReceiveControlC = NULL;
static int multipleIoStates = 0;

void IoState_UserInterruptHandler(int sig) {
    printf("\nIOVM:\n");

    if (multipleIoStates) {
        // what could we do here to tell which IoState we are in?
        // send the interrupt to all of them? interactively let the user choose
        // one? interrupt the first stat created
        printf("	Received signal but since multiple Io states are in "
               "use\n");
        printf(
            "	we don't know which state to send the signal to. Exiting.\n");
        exit(0);
    } else {
        IoState *self = stateToReceiveControlC;

        if (self->receivedSignal) {
            printf("	Second signal received before first was handled. \n");
            printf("	Assuming control is stuck in a C call and isn't "
                   "returning\n");
            printf("	to Io so we're exiting without stack trace.\n\n");
            exit(-1);
        } else {
            self->receivedSignal = 1;

            /*
            IoObject *system = IoState_protoWithName_(self, "System");
            if (system)
            {
                    IoObject *handlerMethod = IoObject_rawGetSlot_(system,
            IOSYMBOL("interuptHandler")); if (handlerMethod)
                    {
                            return;
                    }
            }
            */

            printf("	Received signal. Setting interrupt flag.\n");
        }
    }
}

void IoState_callUserInterruptHandler(IoState *self) {
    self->receivedSignal = 0;

    {
        IoObject *system = IoState_doCString_(self, "System");
        IoMessage *m =
            IoMessage_newWithName_(self, SIOSYMBOL("userInterruptHandler"));
        IoMessage_locals_performOn_(m, system, system);
    }
}

void IoState_setupUserInterruptHandler(IoState *self) {
    if (stateToReceiveControlC) {
        multipleIoStates = 1;
    }

    stateToReceiveControlC = self;
    signal(SIGINT, IoState_UserInterruptHandler);
    signal(SIGTERM, IoState_UserInterruptHandler);
}
