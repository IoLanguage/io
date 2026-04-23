// metadoc Error category Core
// metadoc Error copyright Rich Collins 2008
// metadoc Error license BSD revised
/*metadoc Error description
An object that contains error information and flow control based on errors.
*/

/*cmetadoc Error description
Minimal C scaffolding for the Error proto. The object carries no C
payload of its own — error information lives entirely in Io-level
slots such as "message" — so this file only supplies the tag, proto
registration, and a pair of convenience constructors used by C code
that wants to hand an Error up through the VM. The richer exception
flow (raise, catch, pass) is implemented in Io on top of Exception;
IoError is the lightweight structural representative returned by
primitives that report failure without raising.
*/

#include "IoError.h"
#include "IoState.h"
#include "IoSeq.h"

static const char *protoId = "Error";

/*cdoc Error IoError_newTag(state)
Builds the Error tag. Only a cloneFunc is installed — Error has no
owned C memory, so no free/mark/compare hooks are needed; the GC
treats it like any plain IoObject.
*/
IoTag *IoError_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_("Error");
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoError_rawClone);
    return tag;
}

/*cdoc Error IoError_proto(state)
Creates and registers the Error proto. The method table is empty;
all user-visible behavior is attached from Io-level code in the
standard library, so this just establishes the type identity so
IoObject_tag comparisons and ISERROR-style predicates work.
*/
IoError *IoError_proto(void *state) {
    IoError *self = IoObject_new(state);
    IoObject_tag_(self, IoError_newTag(state));

    IoState_registerProtoWithId_(state, self, protoId);

    {
        IoMethodTable methodTable[] = {{NULL, NULL}};
        IoObject_addMethodTable_(self, methodTable);
    }
    return self;
}

/*cdoc Error IoError_rawClone(proto)
Tag cloneFunc. Delegates entirely to IoObject_rawClonePrimitive —
there is no Error-specific C data to duplicate; slot inheritance
already carries the "message" field and anything else the caller set.
*/
IoError *IoError_rawClone(IoError *proto) {
    IoError *self = IoObject_rawClonePrimitive(proto);
    return self;
}

/*cdoc Error IoError_new(state)
Convenience constructor: clone the registered proto. Callers who
want a message should use IoError_newWithMessageFormat_ or
IoError_newWithCStringMessage_, both of which populate the "message"
slot that Io-level code inspects.
*/
IoError *IoError_new(void *state) {
    IoError *proto = IoState_protoWithId_(state, protoId);
    return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

/*cdoc Error IoError_newWithMessageFormat_(state, format, ...)
printf-style Error constructor. Formats the message via
UArray_newWithVargs_, interns it as a Symbol, and stores it on the
new Error's "message" slot. Used by primitives that want to return
a descriptive Error without going through IoState_error_ / raise.
*/
IoError *IoError_newWithMessageFormat_(void *state, const char *format, ...) {
    IoSymbol *message;

    va_list ap;
    va_start(ap, format);
    message = IoState_symbolWithUArray_copy_(
        state, UArray_newWithVargs_(format, ap), 0);
    va_end(ap);

    {
        IoError *error = IoError_new(state);
        IoObject_setSlot_to_(
            error,
            IoState_symbolWithCString_((IoState *)state, (char *)("message")),
            message);
        return error;
    }
}

/*cdoc Error IoError_newWithCStringMessage_(state, cString)
Simpler variant: wraps a plain C string as an IoSeq and stores it on
"message". Differs from IoError_newWithMessageFormat_ in that the
message becomes a mutable Sequence rather than an interned Symbol,
which matters when callers append context before propagating.
*/
IoError *IoError_newWithCStringMessage_(IoState *state, char *cString) {
    IoError *error = IoError_new(state);
    IoObject_setSlot_to_(error,
                         IoState_symbolWithCString_(state, (char *)("message")),
                         IoSeq_newWithCString_(state, cString));
    return error;
}