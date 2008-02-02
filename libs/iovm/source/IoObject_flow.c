
#include "IoObject.h"
#include "IoNumber.h"

// loops ---------------------------------------

IoObject *IoObject_while(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Object while(<condition>, expression)
	Keeps evaluating message until condition return Nil.
	Returns the result of the last message evaluated or Nil if none were evaluated.
	*/

	IoMessage_assertArgCount_receiver_(m, 2, self);

	{
		IoObject *result = IONIL(self);
		IoState *state = IOSTATE;
		unsigned char c;

		IoState_resetStopStatus(IOSTATE);
		IoState_pushRetainPool(IOSTATE);

		for (;;)
		{
			IoState_clearTopPool(IOSTATE);
			c = ISTRUE(IoMessage_locals_valueArgAt_(m, locals, 0));

			if (!c)
			{
				break;
			}

			result = (IoObject *)IoMessage_locals_valueArgAt_(m, locals, 1);

			if (IoState_handleStatus(IOSTATE))
			{
				goto done;
			}
		}
done:
			IoState_popRetainPoolExceptFor_(state, result);
		return result;
	}
}

IoObject *IoObject_loop(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Object loop(expression)
	Keeps evaluating message until a break.
	*/

	IoMessage_assertArgCount_receiver_(m, 1, self);
	{
		IoState *state = IOSTATE;
		IoObject *result;

		IoState_resetStopStatus(IOSTATE);
		IoState_pushRetainPool(state);

		for (;;)
		{
			IoState_clearTopPool(state);

			result = IoMessage_locals_valueArgAt_(m, locals, 0);

			if (IoState_handleStatus(IOSTATE))
			{
				goto done;
			}
		}
done:
			IoState_popRetainPoolExceptFor_(state, result);
		return result;
	}
}


IoObject *IoObject_for(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Object for(<counter>, <start>, <end>, <do message>)
	A for-loop control structure. See the io Programming Guide for a full description. 
	*/

	IoMessage_assertArgCount_receiver_(m, 4, self);

	{
		IoState *state = IOSTATE;
		IoMessage *indexMessage = IoMessage_rawArgAt_(m, 0);
		IoMessage *doMessage;
		IoObject *result = IONIL(self);
		double i;
		IoSymbol *slotName = IoMessage_name(indexMessage);
		double startValue = IoMessage_locals_doubleArgAt_(m, locals, 1);
		double endValue   = IoMessage_locals_doubleArgAt_(m, locals, 2);
		double increment  = 1;
		IoNumber *num = NULL;

		if (IoMessage_argCount(m) > 4)
		{
			increment = IoMessage_locals_doubleArgAt_(m, locals, 3);
			doMessage = IoMessage_rawArgAt_(m, 4);
		}
		else
		{
			doMessage = IoMessage_rawArgAt_(m, 3);
//			if (startValue > endValue)
//			{
//				increment = -1;
//			}
		}

		IoState_resetStopStatus(state);
		IoState_pushRetainPool(state);

		for (i = startValue; ; i += increment)
		{
			if (increment > 0)
			{
				if (i > endValue) break;
			}
			else
			{
				if (i < endValue) break;
			}

			/*if (result != locals && result != self) IoState_immediatelyFreeIfUnreferenced_(state, result);*/
			IoState_clearTopPool(state);

			{
				num = IONUMBER(i);
				IoObject_addingRef_(locals, num);
				PHash_at_put_(IoObject_slots(locals), slotName, num);

				//IoObject_setSlot_to_(self, slotName, num);
			}

			/*IoObject_setSlot_to_(locals, slotName, IONUMBER(i));*/
			result = IoMessage_locals_performOn_(doMessage, locals, self);

			if (IoState_handleStatus(IOSTATE))
			{
				goto done;
			}
		}

done:
			IoState_popRetainPoolExceptFor_(state, result);
		return result;
	}
}

IoObject *IoObject_return(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Object return(anObject)
	Return anObject from the current execution block.
	*/

	IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
	IoState_return(IOSTATE, v);
	return self;
}

IoObject *IoObject_returnIfNonNil(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Object returnIfNonNil
	Returns the receiver from the current execution block if it is non nil. 
	Otherwise returns the receiver locally.
	*/

	if(!ISNIL(self))
	{
		IoState_return(IOSTATE, self);
	}

	return self;
}

IoObject *IoObject_break(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Object break(optionalReturnValue)
	Break the current loop, if any. 
	*/

	IoObject *v = IONIL(self);

	if (IoMessage_argCount(m) > 0)
	{
		v = IoMessage_locals_valueArgAt_(m, locals, 0);
	}

	IoState_break(IOSTATE, v);
	return self;
}

IoObject *IoObject_continue(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Object continue
	Skip the rest of the current loop iteration and start on
	the next, if any. 
	*/

	IoState_continue(IOSTATE);
	return self;
}

IoObject *IoObject_stopStatus(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Object stopStatus
	Returns the internal IoState->stopStatus.
	*/

	int stopStatus;

	IoMessage_locals_valueArgAt_(m, locals, 0);

	stopStatus = IOSTATE->stopStatus;
	IoState_resetStopStatus(IOSTATE);

	return IoState_stopStatusObject(IOSTATE, stopStatus);
}

IoObject *IoObject_if(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Object if(<condition>, <trueMessage>, <optionalFalseMessage>)
	Evaluates trueMessage if condition evaluates to a non-Nil.
	Otherwise evaluates optionalFalseMessage if it is present.
	Returns the result of the evaluated message or Nil if none was evaluated.
	*/

	const IoObject *const r = IoMessage_locals_valueArgAt_(m, locals, 0);
	const int condition = ISTRUE((IoObject *)r);
	const int index = condition ? 1 : 2;

	if (index < IoMessage_argCount(m))
		return IoMessage_locals_valueArgAt_(m, locals, index);

	return IOBOOL(self, condition);
}

IoObject *IoObject_tailCall(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Object tailCall(expression)
	Does a tailCall on the currently executing Block. Example:
	<pre>	
	Io> a := method(x, x = x + 1; if(x > 10, return x); tailCall(x))
	==> method(x, updateSlot("x", x +(1));
			if(x >(10), return(x));
			tailCall(x))
	Io> a(1)
	==> 11
	</pre>	
	*/

	IOSTATE->tailCallMessage = m;
	IoState_return(IOSTATE, IONIL(self));
	return self;
}

