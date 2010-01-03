
//metadoc Collector copyright Steve Dekorte 2002
//metadoc Collector license BSD revised
//metadoc Profiler category Core
/*metadoc Profiler description

*/

#include "IoProfiler.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoBlock.h"

typedef IoObject IoProfiler;

IO_METHOD(IoProfiler, reset)
{
	/*doc Collector reset
	Resets the profilerTime on all Block objects in the system.
	*/
	
	Collector *collector = IOSTATE->collector;
	
	COLLECTOR_FOREACH(collector, v,
		if(ISBLOCK(v)) { IoBlock_rawResetProfilerTime(v); }
	);
	
	return self;
}


IoObject *IoProfiler_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"reset", IoProfiler_reset},
	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("Profiler"));
	IoObject_addMethodTable_(self, methodTable);
	return self;
}
