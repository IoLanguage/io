# Block

Blocks are anonymous functions (messages with their own locals object).
They are typically used to represent object methods.

## argumentNames

Returns a List of strings containing the argument names of the receiver.

## asSimpleString

Returns a short description of the receiver.

## asString

Returns a full description of the receiver with decompiled code.

## call(arg0, arg1, ...)

Activates the receiver with the provided arguments.

## callWithArgList(aList)

Activates the block with the given argument list.

## code

Returns a string containing the decompiled code of the receiver.

## Formatter

Helper object for the <tt>asString</tt> method.

## justSerialized(stream)

Writes the receiver's code into the stream.

## message

Returns the root message of the receiver.

## passStops

Returns whether or not the receiver passes return/continue/break to caller.

## performOn(anObject, optionalLocals, optionalMessage,

optionalSlotContext) Activates the receiver in the target context of
anObject. Returns the result.

## print

Prints an Io source code representation of the block/method.

## println

Same as <tt>print</tt>.

## profilerTime

Returns clock() time spent in compiler in seconds.

## scope

Returns the scope used when the block is activated or
Nil if the target of the message is the scope.

## setArgumentNames(aListOfStrings)

Sets the receiver's argument names to those specified in
aListOfStrings. Returns self.

## setCode(aString)

Sets the receiver's message to a compiled version of aString. Returns self.

## setMessage(aMessage)

Sets the root message of the receiver to aMessage.

## setPassStops(aBool)

Sets whether the receiver passes return/continue/break to caller.

## setProfilerOn(aBool)

If aBool is true, the global block profiler is enabled, if false it is
disabled. Returns self.

## setScope(anObjectOrNil)

If argument is an object, when the block is activated,
it will set the proto and self slots of its locals to the specified
object. If Nil, it will set them to the target of the message.

