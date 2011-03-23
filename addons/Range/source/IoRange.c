//metadoc Range category Core
//metadoc Range copyright Jeremy Tregunna, 2006
//metadoc Range license BSD
//metadoc Range description Simple datastructure representing the items at and between two specific points.
#include <math.h>
#include "IoState.h"
#define IORANGE_C
#include "IoRange.h"
#undef IORANGE_C
#include "IoNumber.h"

#define DATA(self) ((IoRangeData *)IoObject_dataPointer(self))

IoTag *IoRange_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Range");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoRange_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoRange_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoRange_mark);
	return tag;
}

IoRange *IoRange_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoRange_newTag(state));
	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoRangeData)));
	DATA(self)->start = IONIL(self);
	DATA(self)->end = IONIL(self);
	DATA(self)->curr = IONIL(self);
	DATA(self)->increment = IONIL(self);
	DATA(self)->index = IONIL(self);

	IoState_registerProtoWithFunc_((IoState *)state, self, IoRange_proto);

	{
		IoMethodTable methodTable[] = {
			{"first", IoRange_first},
			{"last", IoRange_last},
			{"next", IoRange_next},
			{"previous", IoRange_previous},
			{"index", IoRange_index},
			{"value", IoRange_value},
			{"foreach", IoRange_foreach},
			{"setRange", IoRange_setRange},
			{"rewind", IoRange_rewind},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoRange *IoRange_rawClone(IoRange *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoRangeData)));

	DATA(self)->start = DATA(proto)->start;
	DATA(self)->curr = DATA(proto)->curr;
	DATA(self)->end = DATA(proto)->end;
	DATA(self)->increment = DATA(proto)->increment;
	DATA(self)->index = DATA(proto)->index;
	return self;
}

IORANGE_API IoRange *IoRange_new(void *state)
{
	IoRange *proto = IoState_protoWithInitFunction_(state, IoRange_proto);
	return IOCLONE(proto);
}

void IoRange_free(IoRange *self)
{
	io_free(IoObject_dataPointer(self));
}

void IoRange_mark(IoRange *self)
{
	IoRangeData *dp = IoObject_dataPointer(self);
	if(dp->start) IoObject_shouldMark(dp->start);
	if(dp->curr) IoObject_shouldMark(dp->curr);
	if(dp->end) IoObject_shouldMark(dp->end);
	if(dp->increment) IoObject_shouldMark(dp->increment);
	if(dp->index) IoObject_shouldMark(dp->index);
}

/* ----------------------------------------------------------- */

IoObject *IoRange_first(IoRange *self, IoObject *locals, IoMessage *m)
{
	/*doc Range first
	Moves the current cursor to the beginning of the range, and returns it.
	 */

	IoRangeData *rd = DATA(self);
	rd->curr = rd->start;
	IoRange_setIndex(self, IONUMBER(0));
	return rd->curr;
}

IoObject *IoRange_last(IoRange *self, IoObject *locals, IoMessage *m)
{
	/*doc Range last
	Moves the current cursor to the end of the range, and returns it.
	 */
	IoRangeData *rd = DATA(self);
	IoObject *context;
	
	IoObject *v = IoObject_rawGetSlot_context_(rd->curr, IOSYMBOL("nextInSequence"), &context);
	IoObject *lt = IoObject_rawGetSlot_context_(rd->curr, IOSYMBOL("compare"), &context);

	if(v && lt)
	{
		IoObject *last, *result_lt;
		IoMessage *newMessage = IoMessage_new(IOSTATE);
		// Get penultimate index and jump to it
		double index = ceil((CNUMBER(rd->end) - CNUMBER(rd->start))/CNUMBER(rd->increment)) - 1;
		IoMessage_setCachedArg_to_(newMessage, 0, IONUMBER(CNUMBER(rd->increment)*(index)));
		last = IoObject_activate(v, rd->start, locals, newMessage, context);

		// Set penultimate index as current
		IoRange_setCurrent(self, last);
		IoRange_setIndex(self, IONUMBER(index));

		// Try to get next 
		IoMessage_setCachedArg_to_(newMessage, 0, rd->increment);
		last = IoObject_activate(v, rd->curr, locals, newMessage, context);
		// Compare it with end of range
		IoMessage_setCachedArg_to_(newMessage, 0, rd->end);
		result_lt = IoObject_activate(lt, last, locals, newMessage, context);

		// If new last value not out of bounds set it as current
		if(rd->end >= rd->start ? IoNumber_asInt(result_lt) <= 0 : IoNumber_asInt(result_lt) >= 0)
		{
			IoRange_setCurrent(self, last);
			IoRange_setIndex(self, IONUMBER(CNUMBER(rd->index) + 1));
		}
		return rd->curr;
	}
	return IONIL(self);
}

IoObject *IoRange_next(IoRange *self, IoObject *locals, IoMessage *m)
{
	/*doc Range next
		Sets the current item in the range to the next item in the range, 
		and returns a boolean value indicating whether it is not at the end of the range.
	*/

	IoRangeData *rd = DATA(self);
	IoObject *context;
	IoObject *v = IoObject_rawGetSlot_context_(rd->curr, IOSYMBOL("nextInSequence"), &context);
	IoObject *lt = IoObject_rawGetSlot_context_(rd->curr, IOSYMBOL("compare"), &context);
	

	if (v && lt)
	{
		IoMessage *newMessage = IoMessage_new(IOSTATE);
		IoObject *r_lt, *ret;
		
		IoMessage_setCachedArg_to_(newMessage, 0, rd->increment);
		ret = IoObject_activate(v, rd->curr, locals, newMessage, context);

		// compare next value with end of range
		IoMessage_setCachedArg_to_(newMessage, 0, rd->end);
		r_lt = IoObject_activate(lt, ret, locals, newMessage, context);

		// The comparing result depends on a range (his decreasing or increasing)
		if (rd->end >= rd->start ? IoNumber_asInt(r_lt) <= 0 : IoNumber_asInt(r_lt) >= 0)
		{
			IoRange_setCurrent(self, ret);
			IoRange_setIndex(self, IONUMBER(CNUMBER(rd->index) + 1));
			return self;
		}
	}

	return IONIL(self);
}

IoObject *IoRange_previous(IoRange *self, IoObject *locals, IoMessage *m)
{
	/*doc Range previous
	Sets the current item in the range to the previous item in the range, 
	and returns a boolean value indicating whether it is not at the beginning of the range.
	 */

	IoRangeData *rd = DATA(self);
	IoObject *context;
	IoObject *v = IoObject_rawGetSlot_context_(rd->curr, IOSYMBOL("nextInSequence"), &context);

	if (v && rd->curr != rd->start)
	{
		IoMessage *newMessage = IoMessage_new(IOSTATE);
		IoObject *ret;
		IoMessage_addCachedArg_(newMessage, IONUMBER(-CNUMBER(rd->increment)));
		ret = IoObject_activate(v, rd->curr, locals, newMessage, context);
		IoRange_setCurrent(self, ret);
		IoRange_setIndex(self, IONUMBER(CNUMBER(rd->index) - 1));
		return self;
	}

	return IONIL(self);
}

IoObject *IoRange_index(IoRange *self, IoObject *locals, IoMessage *m)
{
/*doc Range index
Returns the current index number starting from zero and extending 
outward up to the maximum number of items in the range.
*/

	return DATA(self)->index;
}

IoObject *IoRange_value(IoRange *self, IoObject *locals, IoMessage *m)
{
	/*doc Range value
	Returns the value of the current item in the range.
	 */

	return DATA(self)->curr;
}

/* ----------------------------------------------------------- */

IORANGE_API IoRange *IoRange_setRange(IoRange *self, IoObject *locals, IoMessage *m)
{
	/*doc Range setRange(start, end, increment)
	Has several modes of operation. First, if only two parameters are specified, 
	the increment value is set to 1 by default, while the first parameter represents
	the point to start from, and the second parameter represents the point to end at. 
	If the second parameter is smaller than the first, the range will operate backwards. 
	If the third parameter is specified, a custom iteration value will be used instead of 1.
	 */

	IoObject *start = IoMessage_locals_valueArgAt_(m, locals, 0);
	IoObject *end = IoMessage_locals_valueArgAt_(m, locals, 1);
	IoNumber *increment;

	if (IoMessage_argCount(m) == 3)
		increment = IoMessage_locals_numberArgAt_(m, locals, 2);
	else 
	{
	  if( IOREF(start) < IOREF(end))
	    increment = IONUMBER(1);
	  else
      increment = IONUMBER(-1);
  }
  
	DATA(self)->start = IOREF(start);
	DATA(self)->end = IOREF(end);
	DATA(self)->curr = DATA(self)->start;
	DATA(self)->increment = IOREF(increment);
	DATA(self)->index = IONUMBER(0);

	return self;
}

IoRange *IoRange_rewind(IoRange *self, IoObject *locals, IoMessage *m)
{
	/*doc Range rewind
	Sets the current item and the index to the values the receiver started out with.
	*/
	IoRange_setCurrent(self, RANGEDATA(self)->start);
	IoRange_setIndex(self, IONUMBER(0));
	return self;
}

IoObject *IoRange_each(IoRange *self, IoObject *locals, IoMessage *m)
{
	IoState *state = IOSTATE;
	IoObject *result = IONIL(self);
	IoMessage *doMessage = IoMessage_rawArgAt_(m, 0);
	IoObject *savedCursor, *savedIndex;
	IoRangeData *rd = DATA(self);

	double index;
	savedCursor = rd->curr;
	savedIndex = rd->index;
	IoRange_setCurrent(self, rd->start);
	
	for(index = 0; ; index += 1)
	{
		IoState_clearTopPool(state);
		result = IoMessage_locals_performOn_(doMessage, locals, rd->curr);
		if (IoRange_next(self, locals, m) == IONIL(self)) break;
		if (IoState_handleStatus(state)) break;
	}

	IoRange_setCurrent(self, savedCursor);
	IoRange_setIndex(self, savedIndex);
	IoState_popRetainPoolExceptFor_(state, result);
	return result;
}

IoObject *IoRange_foreach(IoRange *self, IoObject *locals, IoMessage *m)
{
	/*doc Range foreach(optionalIndex, value, message)
	Iterates over each item beginning with the starting point, and finishing at 
	the ending point inclusive. This method can operate several ways; these include: 
	(1) Takes one argument, the message tree to be executed during each iteration; (2) 
	Takes two arguments, the first argument is the name of the current value being 
	iterated over, and the second is the message tree to be executed during each 
	iteration; (3) Takes three arguments: the first is the current index within the 
	range, the second is the name of the current value being iterated over, and the 
	third is the message tree to be executed during each iteration. 
	For example:
<pre>
// First method (operating on numbers)
1 to(10) foreach("iterating" print) // prints "iterating" 10 times
// Second method (operating on numbers)
1 to(10) foreach(v, v print) // prints each value
// Third method (operating on numbers)
1 to(10) foreach(i, v, writeln(i .. ": " .. v)) // prints "index: value"
</pre>
	 */

	IoState *state = IOSTATE;
	IoObject *result = IONIL(self);
	IoObject *savedCursor, *savedIndex;
	IoSymbol *indexName,*valueName;
	IoMessage *doMessage;
	IoRangeData *rd = DATA(self);

	if (IoMessage_argCount(m) == 1)
	{
		return IoRange_each(self, locals, m);
	}

	IoMessage_foreachArgs(m, self, &indexName, &valueName, &doMessage);
	IoState_pushRetainPool(state);

	{
		double index;

		// Saving state
		savedCursor = rd->curr;
		savedIndex = rd->index;
		IoRange_setCurrent(self, rd->start);
		
		for(index = 0; ; index += 1)
		{
			IoState_clearTopPool(state);
			if (indexName)
				IoObject_setSlot_to_(locals, indexName, IONUMBER(index));

			IoObject_setSlot_to_(locals, valueName, rd->curr);
			result = IoMessage_locals_performOn_(doMessage, locals, locals);
			if (IoState_handleStatus(state)) break;
			if (IoRange_next(self, locals, m) == IONIL(self)) break;
		}
	}

	IoState_popRetainPoolExceptFor_(state, result);

	// Recovering state
	IoRange_setCurrent(self, savedCursor);
	IoRange_setIndex(self, savedIndex);
	return result;
}

