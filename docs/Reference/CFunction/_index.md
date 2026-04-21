# CFunction

A container for a pointer to a C function binding.
CFunction's can only be defined from the C side and act
like blocks in that when placed in a slot, are called when the
slot is activated. The for, if, while and clone methods of the Lobby
are examples of CFunctions. CFunctions are useful for implementing
methods that require the speed of C or binding to a C library.

- **category**: Core
- **copyright**: Steve Dekorte 2002
- **license**: BSD revised

## ==(anObject)

Returns self if the argument is a CFunction with the same internal C
function pointer.

## asSimpleString

Returns the CFunction name.

## asString

Same as asSimpleString.

## id

Returns a number containing a unique id for the receiver's internal C
function.

## name

Returns the CFunction name in the form CFunction_Pointer.

## performOn(target, blockLocals, optionalMessage,

optionalContext) Activates the CFunctions with the supplied settings.

## profilerTime

Returns clock() time spent in compiler in seconds.

## setProfilerOn(aBool)

If aBool is true, the global block profiler is enabled, if false it is
disabled. Returns self.

## typeName

Returns the owning type of the CFunction or nil if the CFunction can be
called on any object.

## uniqueName

Returns the name given to the CFunction.

