/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOCONTEXT_DEFINED 
#define IOCONTEXT_DEFINED 1

#include "IoObject_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void *self;       // activationContext
    void *locals;     // currentContext
    void *message;
    void *slotContext;
} IoContext;

#define IOCONTEXT(self, locals, message) ((IoContext *){self, locals, message})

/*

#define SELF	(context->self)
#define LOCALS	(context->locals)
#define MESSAGE (context->message)

example:

    IoObject *IoDuration_days(IoDuration *self, IoObject *locals, IoMessage *m);

becomes:

    IoObject *IoDuration_days(IoContext *context);
*/

#ifdef __cplusplus
}
#endif
#endif


