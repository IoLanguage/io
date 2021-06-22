
// metadoc Profiler category Core
// metadoc Profiler copyright Steve Dekorte 2002
// metadoc Profiler license BSD revised
/*metadoc Profiler description
Basic support for profiling Io code execution.
*/

#include "IoProfiler.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoBlock.h"

typedef IoObject IoProfiler;

// static const char *protoId = "Profiler";

IO_METHOD(IoProfiler, reset) {
    /*doc Profiler reset
    Resets the profilerTime on all Block objects in the system.
    */

    Collector *collector = IOSTATE->collector;

    COLLECTOR_FOREACH(
        collector, v, if (ISBLOCK(v)) { IoBlock_rawResetProfilerTime(v); }
        // if(ISCFUNCTION(v) ) { IoCFunction_rawResetProfilerTime(v); }
    );

    return self;
}

IO_METHOD(IoProfiler, timedObjects) {
    /*doc Profiler timedObjects
    Returns a list of objects found in the system that have profile times.
    */

    Collector *collector = IOSTATE->collector;
    IoList *results = IoList_new(IOSTATE);

    COLLECTOR_FOREACH(
        collector, v,
        if (ISBLOCK(v) && IoBlock_rawProfilerTime(v)) {
            IoList_rawAppend_(results, (void *)v);
        }
        // if(ISCFUNCTION(v)) { IoList_rawAppend_(results, (void *)v) }
    );

    return results;
}

IoObject *IoProfiler_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"reset", IoProfiler_reset},
        {"timedObjects", IoProfiler_timedObjects},
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);
    IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("Profiler"));
    IoObject_addMethodTable_(self, methodTable);
    return self;
}
