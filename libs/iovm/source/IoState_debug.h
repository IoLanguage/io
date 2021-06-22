

void IoState_show(IoState *self);

IoObject *IoState_replacePerformFunc_with_(IoState *self,
                                           IoTagPerformFunc *oldFunc,
                                           IoTagPerformFunc *newFunc);

void IoState_debuggingOn(IoState *self);
void IoState_debuggingOff(IoState *self);

int IoState_hasDebuggingCoroutine(IoState *self);

void IoState_updateDebuggingMode(IoState *self);

void IoState_setupUserInterruptHandler(IoState *self);

void IoState_callUserInterruptHandler(IoState *self);
