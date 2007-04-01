/*#io
Collector ioDoc(
			 docCopyright("Steve Dekorte", 2002)
			 docLicense("BSD revised")
			 docCategory("Core")
			 docDescription("""Contains methods related to Io's garbage collector. Io currently uses a incremental, non-moving, generational collector based on the tri-color (black/gray/white) algorithm with a write-barrier.
<p>
Every N number of object allocs, the collector will walk some of the objects marked as gray, marking their connected white objects as gray and turning themselves black. Every M allocs, it will pause for a sweep where it makes sure all grays are marked black and io_frees all whites.
<p>
If the sweepsPerGeneration is set to zero, it will immediately mark all blacks as white again and mark the root objects as gray. Otherwise, it will wait until the sweepsPerGeneration count is reached to do this. By adjusting the allocsPerSweep and sweepsPerGeneration appropriately, the collector can be tuned efficiently for various usage cases. Generally, the more objects in your heap, the larger you'll want this number.""")
			 */

#include "IoCollector.h"
#include "IoNumber.h"
#include "IoList.h"

typedef IoObject IoCollector;

IoObject *IoCollector_collect(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("collect", "Runs garbage collector. Returns the number of items collected. ")
	*/

	int count = Collector_collect(IOSTATE->collector);
	//io_show_mem("after Collector collect");
	return IONUMBER(count);
}

IoObject *IoCollector_showStats(IoCollector *self, IoObject *locals, IoMessage *m)
{
	io_show_mem("IoCollector_showStats");
	printf("marksPerAlloc       %i\n", Collector_marksPerAlloc(IOSTATE->collector));
	printf("allocatedStep       %i\n", Collector_allocatedStep(IOSTATE->collector));
	return self;
}

IoObject *IoCollector_maxAllocatedBytes(IoCollector *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(io_maxAllocatedBytes());
}

IoObject *IoCollector_resetMaxAllocatedBytes(IoCollector *self, IoObject *locals, IoMessage *m)
{
	io_resetMaxAllocatedBytes();
	return self;
}

IoObject *IoCollector_setDebug(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setDebug(aBool)", "Turns on/off printing of collector debugging messages. Returns self.")
	*/
	IoObject *aBool = IoMessage_locals_valueArgAt_(m, locals, 0);

	Collector_setDebug_(IOSTATE->collector, ISTRUE(aBool));
	return self;
}

IoObject *IoCollector_setMarksPerAlloc(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setMarksPerAlloc(aNumber)", "Sets the number of incremental collector marks per object allocation (can be fractional). Returns self.")
	*/

	float n = IoMessage_locals_floatArgAt_(m, locals, 0);

	Collector_setMarksPerAlloc_(IOSTATE->collector, n);
	return self;
}

IoObject *IoCollector_marksPerAlloc(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("allocsPerMark",
		   "Return the number of allocations per collector mark pass.")
	*/

	return IONUMBER(Collector_marksPerAlloc(IOSTATE->collector));
}

IoObject *IoCollector_setAllocatedStep(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setAllocatedStep(aNumber)", "Sets the allocatedStep (can have a fractional component, but must be larger than 1). A collector sweep is forced when the number of allocated objects exceeds the allocatedSweepLevel. After a sweep, the allocatedSweepLevel is set to the allocated object count times the allocatedStep. Returns self.")
	*/

	float n = IoMessage_locals_floatArgAt_(m, locals, 0);
	IOASSERT(n > 1.0, "allocatedStep must be larger than 1");
	Collector_setAllocatedStep_(IOSTATE->collector, n);
	return self;
}

IoObject *IoCollector_allocatedStep(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("allocatedStep",
		   "Return the allocation step value as a Number.")
	*/

	return IONUMBER(Collector_allocatedStep(IOSTATE->collector));
}

IoObject *IoCollector_timeUsed(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("timeUsed", "Return the time used so far by the collector in seconds.")
	*/

	return IONUMBER(Collector_timeUsed(IOSTATE->collector));
}

IoObject *IoCollector_allObjects(IoCollector *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("allObjects", "Returns a List containing all objects known to the collector.")
	*/

	IoList *allObjects = IoList_new(IOSTATE);
	Collector *collector = IOSTATE->collector;
	COLLECTOR_FOREACH(collector, v, IoList_rawAppend_(allObjects, (void *)v));
	return allObjects;
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
	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("Collector"));
	IoObject_addMethodTable_(self, methodTable);
	return self;
}
