
//metadoc Object copyright Steve Dekorte 2002
//metadoc Object license BSD revised

// loop

IoObject *IoObject_loop(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_while(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_foreachSlot(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_for(IoObject *self, IoObject *locals, IoMessage *m);

// break

IoObject *IoObject_returnIfNonNil(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_return(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_break(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_continue(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_stopStatus(IoObject *self, IoObject *locals, IoMessage *m);

// branch

IoObject *IoObject_if(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_if2(IoObject *self, IoObject *locals, IoMessage *m);

// tail call

IoObject *IoObject_tailCall(IoObject *self, IoObject *locals, IoMessage *m);
