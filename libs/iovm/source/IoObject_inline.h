
//metadoc Object copyright Steve Dekorte 2002
//metadoc Object license BSD revised

#ifdef IOOBJECT_C
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

#include "IoVMApi.h"
#include "IoState.h"

#define IOOBJECT_FOREACHPROTO(self, pvar, code) \
{\
	IoObject **_proto = IoObject_protos(self); \
	while (*_proto) \
	{ \
		IoObject *pvar = *_proto; \
		code; \
		_proto++; \
	} \
}

/*
IOINLINE PHash *IoObject_slots(IoObject *self)
{
	IoObjectData *od = IoObject_deref(self);
	return od->slots;
}

IOINLINE void IoObject_slots_(IoObject *self, PHash *v)
{
	IoObjectData *od = IoObject_deref(self);
	od->slots = v;
}

IOINLINE IoObject **IoObject_protos(IoObject *self)
{
	IoObjectData *od = IoObject_deref(self);
	return od->protos;
}

IOINLINE void IoObject_protos_(IoObject *self, IoObject **v)
{
	IoObjectData *od = IoObject_deref(self);
	od->protos = v;
}
*/

/*
IOINLINE int IoObject_isWhite(IoObject *self)
{
	return Collector_markerIsWhite_(IOCOLLECTOR, (CollectorMarker *)self);
}

IOINLINE int IoObject_isGray(IoObject *self)
{
	return Collector_markerIsGray_(IOCOLLECTOR, (CollectorMarker *)self);
}

IOINLINE int IoObject_isBlack(IoObject *self)
{
	return Collector_markerIsBlack_(IOCOLLECTOR, (CollectorMarker *)self);
}
*/

IOINLINE void IoObject_createSlotsIfNeeded(IoObject *self)
{
	if (!IoObject_ownsSlots(self))
	{
		/*printf("creating slots for %s %p\n", IoObject_tag(self)->name, (void *)self);*/
		IoObject_createSlots(self);
	}
}

IOINLINE void IoObject_rawRemoveAllProtos(IoObject *self)
{
	//IoObject_createSlotsIfNeeded(self);
	memset(IoObject_protos(self), 0, IoObject_rawProtosCount(self) * sizeof(IoObject *));
}

IOINLINE void IoObject_shouldMark(IoObject *self)
{
	Collector_shouldMark_(IOCOLLECTOR, self);
}

IOINLINE void IoObject_shouldMarkIfNonNull(IoObject *self)
{
	if (self)
	{
		IoObject_shouldMark(self);
	}
}

IOINLINE void IoObject_freeIfUnreferenced(IoObject *self)
{
	if (!IoObject_isReferenced(self) && !Collector_isPaused(IOSTATE->collector))
	{
		CollectorMarker_remove((CollectorMarker *)self);
		IoObject_free(self);
	}
}

IOINLINE IoObject *IoObject_addingRef_(IoObject *self, IoObject *ref)
{
	#ifdef IO_BLOCK_LOCALS_RECYCLING
	//printf("IoObject_addingRef_\n");
	if(IoObject_isLocals(self))
	{
		//printf("IoObject_isReferenced_\n");
		IoObject_isReferenced_(ref, 1);
	}
	#endif

	Collector_value_addingRefTo_(IOCOLLECTOR, self, ref);
	//IoObject_isDirty_(self, 1);

	return ref;
}

IOINLINE void IoObject_inlineSetSlot_to_(IoObject *self,
										 IoSymbol *slotName,
										 IoObject *value)
{
	IoObject_createSlotsIfNeeded(self);
	/*
	if (!slotName->isSymbol)
	{
		printf("Io System Error: setSlot slotName not symbol\n");
		exit(1);
	}
	*/

	PHash_at_put_(IoObject_slots(self), IOREF(slotName), IOREF(value));

	/*
	if(PHash_at_put_(IoObject_slots(self), IOREF(slotName), IOREF(value)))
	{
		IoObject_isDirty_(self, 1);
	}
	*/
}

IOINLINE IoObject *IoObject_rawGetSlot_context_(IoObject *self,
												IoSymbol *slotName,
												IoObject **context)
{
	register IoObject *v = (IoObject *)NULL;

	if (IoObject_ownsSlots(self))
	{
		v = (IoObject *)PHash_at_(IoObject_slots(self), slotName);

		if (v)
		{
			*context = self;
			return v;
		}
	}

	IoObject_hasDoneLookup_(self, 1);

	{
		register IoObject **protos = IoObject_protos(self);

		for (; *protos; protos ++)
		{
			if (IoObject_hasDoneLookup((*protos)))
			{
				continue;
			}

			v = IoObject_rawGetSlot_context_(*protos, slotName, context);

			if (v)
			{
				break;
			}
		}
	}

	IoObject_hasDoneLookup_(self, 0);

	return v;
}

IOINLINE IoObject *IoObject_rawGetSlot_(IoObject *self, IoSymbol *slotName)
{
	register IoObject *v = (IoObject *)NULL;

	if (IoObject_ownsSlots(self))
	{
		v = (IoObject *)PHash_at_(IoObject_slots(self), slotName);

		if (v) return v;
	}

	IoObject_hasDoneLookup_(self, 1);

	{
		register IoObject **protos = IoObject_protos(self);

		for (; *protos; protos ++)
		{
			if (IoObject_hasDoneLookup((*protos)))
			{
				continue;
			}

			v = IoObject_rawGetSlot_(*protos, slotName);

			if (v) break;
		}
	}

	IoObject_hasDoneLookup_(self, 0);

	return v;
}

IOINLINE int IoObject_mark(IoObject *self)
{
	/*
	if (IoObject_isLocals(self))
	{
		printf("mark %p locals\n", (void *)self);
	}
	else
	{
		printf("mark %p %s\n", (void *)self, IoObject_name(self));
	}
	*/

	if (IoObject_ownsSlots(self))
	{
		PHASH_FOREACH(IoObject_slots(self), k, v,
			IoObject_shouldMark((IoObject *)k);
			IoObject_shouldMark((IoObject *)v);
		);
	}

	// mark protos

	IOOBJECT_FOREACHPROTO(self, proto, IoObject_shouldMark(proto));

	{
		IoTagMarkFunc *func = IoTag_markFunc(IoObject_tag(self));

		if (func)
		{
			(func)(self);
		}
	}

	return 1;
}

//IoObject *IoObject_addingRef_(IoObject *self, IoObject *ref);
IOVM_API IOINLINE int IoObject_hasCloneFunc_(IoObject *self, IoTagCloneFunc *func);

IOVM_API IOINLINE IoObject *IoObject_activate(IoObject *self,
									 IoObject *target,
									 IoObject *locals,
									 IoMessage *m,
									 IoObject *slotContext)
{
	//TagActivateFunc *act = IoObject_tag(self)->activateFunc;
	//return act ? (IoObject *)((*act)(self, target, locals, m, slotContext)) : self;
	//printf("activate %s %i\n", IoObject_tag(self)->name, IoObject_isActivatable(self));

	return IoObject_isActivatable(self) ? (IoObject *)((IoObject_tag(self)->activateFunc)(self, target, locals, m, slotContext)) : self;
	//return IoObject_tag(self)->activateFunc ? (IoObject *)((IoObject_tag(self)->activateFunc)(self, target, locals, m, slotContext)) : self;
}

IOINLINE IoObject *IoObject_forward(IoObject *self, IoObject *locals, IoMessage *m)
{
	IoState *state = IOSTATE;
	IoObject *context;
	IoObject *forwardSlot = IoObject_rawGetSlot_context_(self, state->forwardSymbol, &context);

	/*
	if (Coro_stackSpaceAlmostGone((Coro*)IoCoroutine_cid(state->currentCoroutine)))
	{

		IoState_error_(IOSTATE, m, "stack overflow in forward while sending '%s' message to a '%s' object",
					CSTRING(IoMessage_name(m)), IoObject_name(self));
	}
	*/

	if (forwardSlot)
	{
		return IoObject_activate(forwardSlot, self, locals, m, context);
	}

	IoState_error_(state, m, "'%s' does not respond to message '%s'",
				IoObject_name(self), CSTRING(IoMessage_name(m)));
	return self;
}

IOINLINE IoObject *IoObject_perform(IoObject *self, IoObject *locals, IoMessage *m)
{
	IoObject *context;
	IoObject *slotValue = IoObject_rawGetSlot_context_(self, IoMessage_name(m), &context);

	if (slotValue)
	{
		return IoObject_activate(slotValue, self, locals, m, context);
	}

	if (IoObject_isLocals(self))
	{
		return IoObject_localsForward(self, locals, m);
	}

	return IoObject_forward(self, locals, m);
}

#undef IO_IN_C_FILE
#endif
