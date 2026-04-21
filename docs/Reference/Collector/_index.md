# Collector

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

## allObjects

Returns a List containing all objects known to the collector.

## allocatedStep

Return the allocation step value as a Number.

## cleanAllObjects

Sets all objects as clean. Returns self.

## collect

Runs garbage collector. Returns the number of items collected.

## dirtyObjects

Returns a List containing all dirty objects known to the collector.

## marksPerAlloc

Return the number of allocations per collector mark pass.

## maxAllocatedBytes

Returns the maximum number of bytes allocated by the collector.

## objectWithUniqueId(aNumber)

Returns an object whose uniqueId is aNumber or nil if no match is found.
Warning: This lookup currently scans all objects, so it is not efficient,
though it should handle thousands of lookups per second.

## resetMaxAllocatedBytes

Resets maximum number of bytes allocated by the collector. Returns self.

## setAllocatedStep(aNumber)

Sets the allocatedStep (can have a fractional component,
but must be larger than 1). A collector sweep is forced when the
number of allocated objects exceeds the allocatedSweepLevel.
After a sweep, the allocatedSweepLevel is set to the allocated
object count times the allocatedStep. Returns self.

## setDebug(aBool)

Turns on/off printing of collector debugging messages. Returns self.

## setMarksPerAlloc(aNumber)

Sets the number of incremental collector marks per object
allocation (can be fractional). Returns self.

## showStats

Prints the collector's stats to standard output.

## timeUsed

Return the time used so far by the collector in seconds.

