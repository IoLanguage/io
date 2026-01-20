
// metadoc EvalFrame copyright Steve Dekorte 2002, 2025
// metadoc EvalFrame license BSD revised
// metadoc EvalFrame category Core

#include "IoEvalFrame.h"
#include <stdlib.h>
#include <string.h>

// Create a new evaluation frame
IoEvalFrame *IoEvalFrame_new(void) {
    IoEvalFrame *self = (IoEvalFrame *)io_calloc(1, sizeof(IoEvalFrame));
    IoEvalFrame_reset(self);
    return self;
}

// Free an evaluation frame
void IoEvalFrame_free(IoEvalFrame *self) {
    if (!self) {
        return;
    }

    // Free argument values array if allocated
    if (self->argValues) {
        io_free(self->argValues);
        self->argValues = NULL;
    }

    io_free(self);
}

// Mark frame contents for garbage collection
void IoEvalFrame_mark(IoEvalFrame *self) {
    if (!self) {
        return;
    }

    // Mark all IoObject pointers that might be in the frame
    IoObject_shouldMarkIfNonNull(self->message);
    IoObject_shouldMarkIfNonNull(self->target);
    IoObject_shouldMarkIfNonNull(self->locals);
    IoObject_shouldMarkIfNonNull(self->cachedTarget);
    IoObject_shouldMarkIfNonNull(self->result);
    IoObject_shouldMarkIfNonNull(self->slotValue);
    IoObject_shouldMarkIfNonNull(self->slotContext);
    IoObject_shouldMarkIfNonNull(self->call);
    IoObject_shouldMarkIfNonNull(self->blockLocals);

    // Mark evaluated arguments
    if (self->argValues) {
        int i;
        for (i = 0; i < self->currentArgIndex; i++) {
            IoObject_shouldMarkIfNonNull(self->argValues[i]);
        }
    }

    // Recursively mark parent frame
    if (self->parent) {
        IoEvalFrame_mark(self->parent);
    }
}

// Reset frame to initial state (for reuse)
void IoEvalFrame_reset(IoEvalFrame *self) {
    if (!self) {
        return;
    }

    self->message = NULL;
    self->target = NULL;
    self->locals = NULL;
    self->cachedTarget = NULL;
    self->parent = NULL;
    self->state = FRAME_STATE_START;
    self->argCount = 0;
    self->currentArgIndex = 0;
    self->result = NULL;
    self->slotValue = NULL;
    self->slotContext = NULL;
    self->call = NULL;
    self->blockLocals = NULL;
    self->passStops = 0;

    // Free argument values if allocated
    if (self->argValues) {
        io_free(self->argValues);
        self->argValues = NULL;
    }
}
