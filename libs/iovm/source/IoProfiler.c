
// metadoc Profiler category Debug
// metadoc Profiler copyright Steve Dekorte 2002
// metadoc Profiler license BSD revised
/*metadoc Profiler description
Basic support for profiling Io code execution.
*/

/*cmetadoc Profiler description
C implementation of Profiler — walks the full heap via the collector
to reset or report per-Block timing data. Profiler owns no state of
its own; the profiler timings live on individual IoBlocks
(IoBlock_rawProfilerTime / IoBlock_rawResetProfilerTime). This module
just provides the bulk reset and bulk query entry points. A
commented-out CFunction branch is kept as a placeholder for
extending profiling to native functions.
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

/*cdoc Profiler IoProfiler_proto(state)
Creates the Profiler singleton: an IoObject with a `type` slot and
the reset / timedObjects method table. No proto registration — the
Profiler is reached by name from the lobby, and there is nothing to
clone. State lives on each profiled Block, not here.
*/
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
