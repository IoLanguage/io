
//metadoc Collector copyright Steve Dekorte 2002
//metadoc Collector license BSD revised
//metadoc Collector category Core
/*metadoc Collector description
A singleton containing methods related to Io's garbage collector. 
Io currently uses a incremental, non-moving, generational 
collector based on the tri-color (black/gray/white) 
algorithm with a write-barrier.
<p>
Every N number of object allocs, the collector will walk 
some of the objects marked as gray, marking their connected 
white objects as gray and turning themselves black. 
Every M allocs, it will pause for a sweep where it makes sure 
all grays are marked black and io_frees all whites.
<p>
If the sweepsPerGeneration is set to zero, it will immediately mark 
all blacks as white again and mark the root objects as gray. Otherwise, 
it will wait until the sweepsPerGeneration count is reached to do this. 
By adjusting the allocsPerSweep and sweepsPerGeneration appropriately, the 
collector can be tuned efficiently for various usage cases. 

Generally, the more objects in your heap, the larger you'll want this number.
*/

#include "IoCollector.h"
#include "IoNumber.h"
#include "IoList.h"

typedef IoObject IoCollector;

IoObject *IoCollector_collect(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector collect
	Runs garbage collector. Returns the number of items collected. 
	*/

	int count = Collector_collect(IOSTATE->collector);
	//io_show_mem("after Collector collect");
	return IONUMBER(count);
}

IoObject *IoCollector_showStats(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector showStats
	Prints the collector's stats to standard output.
	*/
	io_show_mem("IoCollector_showStats");
	printf("marksPerAlloc       %g\n", Collector_marksPerAlloc(IOSTATE->collector));
	printf("allocatedStep       %g\n", Collector_allocatedStep(IOSTATE->collector));
	return self;
}

IoObject *IoCollector_maxAllocatedBytes(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector maxAllocatedBytes
	Returns the maximum number of bytes allocated by the collector.
	*/
	return IONUMBER(io_maxAllocatedBytes());
}

IoObject *IoCollector_resetMaxAllocatedBytes(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector resetMaxAllocatedBytes
	Resets maximum number of bytes allocated by the collector. Returns self.
	*/
	io_resetMaxAllocatedBytes();
	return self;
}

IoObject *IoCollector_setDebug(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector setDebug(aBool)
	Turns on/off printing of collector debugging messages. Returns self.
	*/
	
	IoObject *aBool = IoMessage_locals_valueArgAt_(m, locals, 0);

	Collector_setDebug_(IOSTATE->collector, ISTRUE(aBool));
	return self;
}

IoObject *IoCollector_setMarksPerAlloc(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector setMarksPerAlloc(aNumber)
	Sets the number of incremental collector marks per object 
	allocation (can be fractional). Returns self.
	*/

	float n = IoMessage_locals_floatArgAt_(m, locals, 0);

	Collector_setMarksPerAlloc_(IOSTATE->collector, n);
	return self;
}

IoObject *IoCollector_marksPerAlloc(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector marksPerAlloc
	Return the number of allocations per collector mark pass.
	*/

	return IONUMBER(Collector_marksPerAlloc(IOSTATE->collector));
}

IoObject *IoCollector_setAllocatedStep(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector setAllocatedStep(aNumber)
	Sets the allocatedStep (can have a fractional component, 
	but must be larger than 1). A collector sweep is forced when the 
	number of allocated objects exceeds the allocatedSweepLevel. 
	After a sweep, the allocatedSweepLevel is set to the allocated 
	object count times the allocatedStep. Returns self.
	*/

	float n = IoMessage_locals_floatArgAt_(m, locals, 0);
	IOASSERT(n > 1.0, "allocatedStep must be larger than 1");
	Collector_setAllocatedStep_(IOSTATE->collector, n);
	return self;
}

IoObject *IoCollector_allocatedStep(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector allocatedStep
	Return the allocation step value as a Number.
	*/

	return IONUMBER(Collector_allocatedStep(IOSTATE->collector));
}

IoObject *IoCollector_timeUsed(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector timeUsed
	Return the time used so far by the collector in seconds.
	*/

	return IONUMBER(Collector_timeUsed(IOSTATE->collector));
}

IoObject *IoCollector_allObjects(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector allObjects
	Returns a List containing all objects known to the collector.
	*/

	IoList *results = IoList_new(IOSTATE);
	Collector *collector = IOSTATE->collector;

	COLLECTOR_FOREACH(collector, v, IoList_rawAppend_(results, (void *)v));
	return results;
}

IoObject *IoCollector_dirtyObjects(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector dirtyObjects
	Returns a List containing all dirty objects known to the collector.
	*/

	IoList *results = IoList_new(IOSTATE);
	Collector *collector = IOSTATE->collector;

	COLLECTOR_FOREACH(collector, v,
		if(IoObject_isDirty(v))
		{
			// raw append will do an IOREF on each v
			IoList_rawAppend_(results, (void *)v);
		}
	);
	
	return results;
}

IoObject *IoCollector_cleanAllObjects(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector cleanAllObjects
	Sets all objects as clean. Returns self.
	*/

	Collector *collector = IOSTATE->collector;

	COLLECTOR_FOREACH(collector, v, IoObject_protoClean(v); );	
	return self;
}


IoObject *IoCollector_objectWithUniqueId(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*doc Collector objectWithUniqueId(aNumber)
	Returns an object whose uniqueId is aNumber or nil if no match is found. 
	Warning: This lookup currently scans all objects, so it is not efficient, 
	though it should handle thousands of lookups per second.
	*/
	
	double n = IoMessage_locals_doubleArgAt_(m, locals, 0);
	Collector *collector = IOSTATE->collector;
	
	COLLECTOR_FOREACH(collector, v,
		if(n == ((double)((size_t)IoObject_deref((IoObject *)v)))) { return (IoObject *)v; }
	);
	
	return IONIL(self);
}

IoObject *IoCollector_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"setDebug", IoCollector_setDebug},
	{"collect", IoCollector_collect},

	{"setMarksPerAlloc", IoCollector_setMarksPerAlloc},
	{"marksPerAlloc", IoCollector_marksPerAlloc},

	{"setAllocatedStep", IoCollector_setAllocatedStep},
	{"allocatedStep", IoCollector_allocatedStep},

	{"allObjects", IoCollector_allObjects},
	{"showStats", IoCollector_showStats},
	{"maxAllocatedBytes", IoCollector_maxAllocatedBytes},
	{"resetMaxAllocatedBytes", IoCollector_resetMaxAllocatedBytes},
	{"timeUsed", IoCollector_timeUsed},
	
	{"objectWithUniqueId", IoCollector_objectWithUniqueId},
	{"dirtyObjects", IoCollector_dirtyObjects},
	{"cleanAllObjects", IoCollector_cleanAllObjects},
	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("Collector"));
	IoObject_addMethodTable_(self, methodTable);
	return self;
}
