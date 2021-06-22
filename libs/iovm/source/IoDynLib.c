
// metadoc DynLib category Core
// metadoc DynLib copyright Steve Dekorte 2002
// metadoc DynLib license BSD revised
/*metadoc DynLib description
A DLL Loader by Kentaro A. Kurahone.
*/

#include "IoObject.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoBlock.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoMessage.h"
#include "IoDynLib.h"
#include "DynLib.h"

static const char *protoId = "DynLib";

#define DATA(self) ((DynLib *)IoObject_dataPointer(self))

static IoTag *IoDynLib_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoDynLib_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoDynLib_free);
    return tag;
}

IoObject *IoDynLib_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"setPath", IoDynLib_setPath},
        {"path", IoDynLib_path},
        {"setInitFuncName", IoDynLib_setInitFuncName},
        {"initFuncName", IoDynLib_initFuncName},
        {"setFreeFuncName", IoDynLib_setFreeFuncName},
        {"freeFuncName", IoDynLib_freeFuncName},
        {"open", IoDynLib_open},
        {"close", IoDynLib_close},
        {"isOpen", IoDynLib_isOpen},
        {"call", IoDynLib_call},
        {"voidCall", IoDynLib_voidCall},
        {"callPluginInit", IoDynLib_callPluginInitFunc},
        //{"returnsString", IoDynLib_returnsString},
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoDynLib_newTag(state));
    IoObject_setDataPointer_(self, DynLib_new());
    DynLib_setInitArg_(DATA(self), state);
    DynLib_setFreeArg_(DATA(self), state);
    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    IoObject_addMethodTable_(self, methodTable);
    return self;
}

IoDynLib *IoDynLib_new(void *state) {
    IoDynLib *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

IoDynLib *IoDynLib_rawClone(IoDynLib *proto) {
    /*
    Note that due to the nature of this object, a clone will *NOT* inherit
    its parent's dynamically loaded object.
    */

    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, DynLib_new());
    DynLib_setInitArg_(DATA(self), IOSTATE);
    DynLib_setFreeArg_(DATA(self), IOSTATE);
    return self;
}

void IoDynLib_free(IoDynLib *self) { DynLib_free(DATA(self)); }

IoDynLib *IoDynLib_setPath(IoDynLib *self, IoObject *locals, IoMessage *m) {
    /*doc DynLib setPath(aString)
    Sets the path to the dynamic library. Returns self.
    */

    DynLib_setPath_(DATA(self),
                    CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0)));
    return self;
}

IoDynLib *IoDynLib_path(IoDynLib *self, IoObject *locals, IoMessage *m) {
    /*doc DynLib path
    Returns the path to the dynamic library.
    */

    return IOSYMBOL(DynLib_path(DATA(self)));
}

IoDynLib *IoDynLib_setInitFuncName(IoDynLib *self, IoObject *locals,
                                   IoMessage *m) {
    /*doc DynLib setInitFuncName(aString)
    Sets the initialization function name for the dynamic library. Returns self.
    */

    DynLib_setInitFuncName_(
        DATA(self), CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0)));
    return self;
}

IoDynLib *IoDynLib_initFuncName(IoDynLib *self, IoObject *locals,
                                IoMessage *m) {
    /*doc DynLib initFuncName
    Returns the initialization function name.
    */

    return IOSYMBOL(DynLib_initFuncName(DATA(self)));
}

IoDynLib *IoDynLib_setFreeFuncName(IoDynLib *self, IoObject *locals,
                                   IoMessage *m) {
    /*doc DynLib setFreeFuncName(aString)
    Sets the io_free function name. Returns self.
    */

    DynLib_setFreeFuncName_(
        DATA(self), CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0)));
    return self;
}

IoDynLib *IoDynLib_freeFuncName(IoDynLib *self, IoObject *locals,
                                IoMessage *m) {
    /*doc DynLib freeFuncName
    Returns the io_free function name.
    */

    return IOSYMBOL(DynLib_freeFuncName(DATA(self)));
}

IoDynLib *IoDynLib_open(IoDynLib *self, IoObject *locals, IoMessage *m) {
    /*doc DynLib open
    Opens the dynamic library and returns self or raises a DynLoad.open Error if
    there is an error.
    */

    if (IoMessage_argCount(m)) {
        IoDynLib_setPath(self, locals, m);
    }

    DynLib_open(DATA(self));

    if (DynLib_error(DATA(self))) {
        IoState_error_(IOSTATE, m, "Error loading object '%s': '%s'",
                       DynLib_path(DATA(self)), DynLib_error(DATA(self)));
    }

    return self;
}

IoDynLib *IoDynLib_close(IoDynLib *self, IoObject *locals, IoMessage *m) {
    /*doc DynLib close
    Closes the library. Returns self.
    */

    DynLib_close(DATA(self));
    return self;
}

IoDynLib *IoDynLib_isOpen(IoDynLib *self, IoObject *locals, IoMessage *m) {
    /*doc DynLib isOpen
    Returns true if the library is open, or false otherwise.
    */

    return IOBOOL(self, DynLib_isOpen(DATA(self)));
}

intptr_t bouncer(IoBlock *self, intptr_t ret, intptr_t a, intptr_t b,
                 intptr_t c, intptr_t d, intptr_t e) {
    IoObject *lobby = IoState_lobby(IOSTATE);
    IoNumber *n;
    static IoMessage *m = NULL;
    List *argNames = ((IoBlockData *)IoObject_dataPointer(self))->argNames;

    if (m == NULL)
        m = IoMessage_new(IOSTATE);
    if (0 < argNames->size)
        IoMessage_setCachedArg_toInt_(m, 0, (int)a);
    if (1 < argNames->size)
        IoMessage_setCachedArg_toInt_(m, 1, (int)b);
    if (2 < argNames->size)
        IoMessage_setCachedArg_toInt_(m, 2, (int)c);
    if (3 < argNames->size)
        IoMessage_setCachedArg_toInt_(m, 3, (int)d);
    if (4 < argNames->size)
        IoMessage_setCachedArg_toInt_(m, 4, (int)e);

    n = IoBlock_activate(self, lobby, lobby, m, lobby);

    if (ISNUMBER(n)) {
        return (intptr_t)IoNumber_asInt(n);
    }

    return 0;
}

intptr_t marshal(IoDynLib *self, IoObject *arg) {
    intptr_t n = 0;

    if (ISNUMBER(arg)) {
        n = IoNumber_asInt(arg);
    } else if (ISSYMBOL(arg)) {
        n = (intptr_t)CSTRING(arg);
    } else if (ISLIST(arg)) {
        int i;
        intptr_t *l = io_calloc(1, IoList_rawSize(arg) * sizeof(intptr_t));
        for (i = 0; i < IoList_rawSize(arg); i++)
            l[i] = marshal(self, List_rawAt_(IoList_rawList(arg), i));
        n = (intptr_t)l;
    } else if (ISBUFFER(arg)) {
        n = (intptr_t)IoSeq_rawBytes(arg);
    } else if (ISBLOCK(arg)) {
        unsigned char *blk = io_calloc(1, 20), *p = blk;
        // FIXME: need trampoline code for other architectures
        *p++ = 0x68;
        *((intptr_t *)p) = (intptr_t)arg;
        p += sizeof(intptr_t);
        *p++ = 0xb8;
        *((intptr_t *)p) = (intptr_t)bouncer;
        p += sizeof(intptr_t);
        *p++ = 0xff;
        *p++ = 0xd0;
        *p++ = 0x83;
        *p++ = 0xc4;
        *p++ = 0x04;
        *p++ = 0xc3;
        n = (intptr_t)blk;
    } else {
        n = (intptr_t)arg; // IONIL(self);
    }

    return n;
}

IoObject *demarshal(IoObject *self, IoObject *arg, intptr_t n) {
    if (ISNUMBER(arg)) {
        return IONUMBER(n);
    } else if (ISSYMBOL(arg)) {
        if (n == 0)
            return IOSYMBOL("");
        return IOSYMBOL((char *)n);
    } else if (ISLIST(arg)) {
        intptr_t *values = (intptr_t *)n;
        int i;

        for (i = 0; i < IoList_rawSize(arg); i++) {
            IoObject *value = List_at_(IoList_rawList(arg), i);
            List_at_put_(IoList_rawList(arg), i,
                         demarshal(self, value, values[i]));
        }

        io_free(values);
        return arg;
    } else if (ISBUFFER(arg)) {
        return arg;
    } else if (ISBLOCK(arg)) {
        return arg;
    }

    return IONIL(self);
}

void IoDynLib_rawVoidCall(void *f, int argCount, intptr_t *params) {
    switch (argCount - 1) {
    case 0:
        ((void (*)(void))f)();
        break;
    case 1:
        ((void (*)(intptr_t))f)(params[0]);
        break;
    case 2:
        ((void (*)(intptr_t, intptr_t))f)(params[0], params[1]);
        break;
    case 3:
        ((void (*)(intptr_t, intptr_t, intptr_t))f)(params[0], params[1],
                                                    params[2]);
        break;
    case 4:
        ((void (*)(intptr_t, intptr_t, intptr_t, intptr_t))f)(
            params[0], params[1], params[2], params[3]);
        break;
    case 5:
        ((void (*)(intptr_t, intptr_t, intptr_t, intptr_t, intptr_t))f)(
            params[0], params[1], params[2], params[3], params[4]);
        break;
    case 6:
        ((void (*)(intptr_t, intptr_t, intptr_t, intptr_t, intptr_t,
                   intptr_t))f)(params[0], params[1], params[2], params[3],
                                params[4], params[5]);
        break;
    case 7:
        ((void (*)(intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t,
                   intptr_t))f)(params[0], params[1], params[2], params[3],
                                params[4], params[5], params[6]);
        break;
    case 8:
        ((void (*)(intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t,
                   intptr_t, intptr_t))f)(params[0], params[1], params[2],
                                          params[3], params[4], params[5],
                                          params[6], params[7]);
        break;
    }
}

intptr_t IoDynLib_rawNonVoidCall(void *f, int argCount, intptr_t *params) {
    intptr_t rc = 0;

    switch (argCount - 1) {
    case 0:
        rc = ((intptr_t(*)(void))f)();
        break;
    case 1:
        rc = ((intptr_t(*)(intptr_t))f)(params[0]);
        break;
    case 2:
        rc = ((intptr_t(*)(intptr_t, intptr_t))f)(params[0], params[1]);
        break;
    case 3:
        rc =
            ((intptr_t(*)(intptr_t, intptr_t, intptr_t))f)(params[0], params[1],
                                                           params[2]);
        break;
    case 4:
        rc = ((intptr_t(*)(intptr_t, intptr_t, intptr_t,
                           intptr_t))f)(params[0], params[1], params[2],
                                        params[3]);
        break;
    case 5:
        rc = ((intptr_t(*)(intptr_t, intptr_t, intptr_t, intptr_t,
                           intptr_t))f)(params[0], params[1], params[2],
                                        params[3], params[4]);
        break;
    case 6:
        rc = ((intptr_t(*)(intptr_t, intptr_t, intptr_t, intptr_t, intptr_t,
                           intptr_t))f)(params[0], params[1], params[2],
                                        params[3], params[4], params[5]);
        break;
    case 7:
        rc = ((intptr_t(*)(intptr_t, intptr_t, intptr_t, intptr_t, intptr_t,
                           intptr_t, intptr_t))f)(params[0], params[1],
                                                  params[2], params[3],
                                                  params[4], params[5],
                                                  params[6]);
        break;
    case 8:
        rc = ((intptr_t(*)(
            intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t,
            intptr_t, intptr_t))f)(params[0], params[1], params[2], params[3],
                                   params[4], params[5], params[6], params[7]);
        break;
    }

    return rc;
}

IoDynLib *IoDynLib_justCall(IoDynLib *self, IoObject *locals, IoMessage *m,
                            int isVoid) {
    int n, rc = 0;
    intptr_t *params = NULL;
    IoSymbol *callName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    void *f = DynLib_pointerForSymbolName_(DATA(self), CSTRING(callName));

    // printf("DynLib calling '%s'\n", CSTRING(callName));

    if (f == NULL) {
        IoState_error_(IOSTATE, m, "Error resolving call '%s'.",
                       CSTRING(callName));
        return IONIL(self);
    }

    if (IoMessage_argCount(m) > 9) {
        IoState_error_(IOSTATE, m,
                       "Error, too many arguments (%i) to call '%s'.",
                       IoMessage_argCount(m) - 1, CSTRING(callName));
        return IONIL(self);
    }

    if (IoMessage_argCount(m) > 1) {
        params = io_calloc(1, IoMessage_argCount(m) * sizeof(unsigned int));

        for (n = 0; n < IoMessage_argCount(m) - 1; n++) {
            IoObject *arg = IoMessage_locals_valueArgAt_(m, locals, n + 1);
            intptr_t p = marshal(self, arg);

            params[n] = p;

            /*
            if (p == 0)
            {
                    IoState_error_(IOSTATE, m, "DynLib error marshalling
            argument (%i) to call '%s'.", n + 1, CSTRING(callName));
                    // FIXME this can leak memory.
                    io_free(params);
                    return IONIL(self);
            }
            */
        }
    }

#if 0
	printf("calling %s with %i arguments\n",
	CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0)),
	IoMessage_argCount(m) - 1);
#endif

    IoState_pushCollectorPause(IOSTATE);

    if (isVoid) {
        IoDynLib_rawVoidCall(f, IoMessage_argCount(m), params);
    } else {
        rc = (int)IoDynLib_rawNonVoidCall(f, IoMessage_argCount(m), params);
    }

    IoState_popCollectorPause(IOSTATE);

    if (params) {
        for (n = 0; n < IoMessage_argCount(m) - 1; n++) {
            IoObject *arg = IoMessage_locals_valueArgAt_(m, locals, n + 1);
            demarshal(self, arg, params[n]);
        }

        io_free(params);
    }

    return isVoid ? IONIL(self) : IONUMBER(rc);
}

IoDynLib *IoDynLib_call(IoDynLib *self, IoObject *locals, IoMessage *m) {
    /*doc DynLib call(functionName, <arg1>, <arg2>, ...)
    Call's the dll function of the specified name with the arguments provided.
    Returns a Number with the result value.
    */
    return IoDynLib_justCall(self, locals, m, 0);
}

IoDynLib *IoDynLib_voidCall(IoDynLib *self, IoObject *locals, IoMessage *m) {
    /*doc DynLib voidCall(functionName, <arg1>, <arg2>, ...)
    Same as call but for functions with no return value. Returns nil.
    */

    return IoDynLib_justCall(self, locals, m, 1);
}

IoDynLib *IoDynLib_callPluginInitFunc(IoDynLib *self, IoObject *locals,
                                      IoMessage *m) {
    /*doc DynLib callPluginInit(functionName)
    Call's the dll function of the specified name.
    Returns the result as a Number or raises an exception on error.
    */

    intptr_t rc = 0;
    intptr_t *params = NULL;
    void *f = DynLib_pointerForSymbolName_(
        DATA(self), CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0)));
    if (f == NULL) {
        IoState_error_(IOSTATE, m, "Error resolving call '%s'.",
                       CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0)));
        return IONIL(self);
    }

    if (IoMessage_argCount(m) < 1) {
        IoState_error_(
            IOSTATE, m,
            "Error, you must give an init function name to check for.");
        return IONIL(self);
    }

    params = io_calloc(1, sizeof(intptr_t) * 2);

    params[0] = (intptr_t)IOSTATE;
    params[1] = (intptr_t)IOSTATE->lobby;
    rc = ((intptr_t(*)(intptr_t, intptr_t))f)(params[0], params[1]);
    io_free(params);

    return IONUMBER(rc);
}

/*
IoSeq *IoDynLib_returnsString(IoDynLib *self, IoObject *locals, IoMessage *m)
{
        intptr_t n = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals,
0));

        if (n == 0)
        {
                return IOSYMBOL("");
        }

        return IOSYMBOL((char *)n);
}
*/
