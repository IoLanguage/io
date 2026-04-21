# Object

An Object is a key/value dictionary with string keys and values of any type.
The prototype Object contains a clone slot that is a CFunction that creates new
objects. When cloned, an Object will call its init slot (with no arguments).

## -(aNumber)

Returns the negative version of aNumber.
Raises an exception if argument is not a number.

## !=(aValue)

Returns true the receiver is not equal to aValue, false otherwise.

## ?(aMessage)

description: Sends the message aMessage to the receiver if it can respond to it. Example:
<pre>
MyObject test // performs test
MyObject ?test // performs test if MyObject has a slot named test
</pre>
The search for the slot only follows the receivers proto chain.

## ..(arg)

.. is an alias for: method(arg, self asString append(arg asString))

## (expression)

The '' method evaluates the argument and returns the result.

## @

Sends asynchronous message to an object, returns a FutureProxy.
<br/>
Caller coroutine is paused when proxy is accessed (i.e. message is sent)
till result is ready. Proxy will become an actual result when it is ready.
<br/>
See IoGuide for more information.
<br/>
Usage: obj @someMethod(a, b, c)

## @@

Same as Object @, but returns nil instead of FutureProxy.
<br/>
Might be useful in a command line or as a last expression in a block/method when
you don't want to return a future.

## <(expression)

Evaluates argument and returns self if self is less or Nil if not.

## <=(expression)

Evaluates argument and returns self if self is less
than or equal to it, or Nil if not.

## ==(aValue)

Returns true if receiver and aValue are equal, false otherwise.

## >(expression)

Evaluates argument and returns self if self is greater than it, or Nil if
not.

## >=(expression)

Evaluates argument and returns self if self is greater
than or equal to it, or Nil if not.

## actorProcessQueue

Processes each message in a queue, yielding between each message.

## actorRun

Starts actor mode if not started already. Basically, sets actorProcessQueue for later execution.

## addTrait

Takes another object, whose slots will be copied into the receiver. Optionally takes a second argument, a Map object containing string -> string pairs, holding conflicting slot names and names to rename them to. I.e., if you have two objects A and B, both have a slot named foo, you issue A addTrait(B, Map clone atPut("foo", "newFoo")) the value of B foo will be placed in A newFoo.

## ancestors

Returns a list of all of the receiver's ancestors as found by recursively following the protos links.

## ancestorWithSlot(slotName)

Returns the first ancestor of the receiver that contains
a slot of the specified name or Nil if none is found.

## and(arg)

Evaluates argument and returns the result.

## appendProto(anObject)

Appends anObject to the receiver's proto list. Returns self.

## apropos

Prints out <tt>Protos Core</tt> slot descriptions.

## argIsActivationRecord

Note: seems to be an obsolete method.

## argIsCall(arg)

Returns true if arg is an activation context (i.e. Call object)
<br/>
Note: this is used internally in one place only (Coroutine callStack).
Refactoring should be considered.

## asSimpleString

Returns <type>_<uniqueHexId> string.

## asString

Same as <tt>slotSummary</tt>.

## become(anotherObject)

Replaces receiver with <tt>anotherObject</tt> and returns self.
Useful for implementing transparent proxies. See also
<tt>FutureProxy</tt> and <tt>Object @</tt>. <br/> Note: primitives cannot
become new values.

## block(args..., body)

Creates a block and binds it to the sender context (i.e. lexical
context). In other words, block locals' proto is sender's locals.
<tt>args</tt> is a list of formal arguments (can be empty). <br/>
<tt>body</tt> is evaluated in the context of Locals object.<br/>
See also <tt>Object method</tt>.
<br/>
<pre>
Io> block(x, x*2) scope == thisContext
==> true
</pre>

## break(optionalReturnValue)

Break the current loop, if any.

## callcc(aBlock)

Calls aBlock with the current continuation as its argument.
The continuation captures the execution state at the point of
the callcc call. If the block returns normally, callcc returns
the block's return value. If the continuation is invoked with
a value, callcc returns that value instead.

Example:
result := callcc(block(cont,
// This is the "escape" pattern
if(someCondition,
cont invoke("early exit")
)
"normal return"
))
// result is either "early exit" or "normal return"

## checkMemory()

Accesses memory in the IoObjectData struct that should be accessible.
Should cause a memory access exception if memory is corrupt.

## clone

Returns a clone of the receiver.

## cloneWithoutInit

Returns a clone of the receiver but does not call init.

## compact

Compact the memory for the object if possible. Returns self.

## compactState

Attempt to compact the memory of the IoState if possible.

## compare(anObject)

Returns a number containing the comparison value of the target with
anObject.

## contextWithSlot(slotName)

Returns the first context (starting with the receiver and following the
lookup path) that contains a slot of the specified name or Nil if none is
found.

## continue

Skip the rest of the current loop iteration and start on
the next, if any.

## coroDo(code)

Creates a new coro to be run in a context of sender and yields to it.
Returns a coro.

## coroDoLater(code)

Returns a new coro to be run in a context of sender.
New coro is moved to the top of the yieldingCoros queue to be executed
when current coro yields.
<br/>
Note: run target is <tt>self</tt> (i.e. receiver), not <tt>call sender</tt> as in coroDo.

## coroFor(code)

Returns a new coro to be run in a context of sender.

## coroWith(code)

Returns a new coro to be run in a context of receiver.

## currentCoro

Returns the currently running coroutine.

## deprecatedWarning(optionalNewName)

Prints a warning message that the current method is deprecated.
If optionalNewName is supplied, the warning will suggest using that instead.
Returns self.

## do(expression)

Evaluates the message in the context of the receiver. Returns self.

## doFile(pathString)

Evaluates the File in the context of the receiver. Returns the result.
pathString is relative to the current working directory.

## doMessage(aMessage, optionalContext)

Evaluates the message object in the context of the receiver.
Returns the result. optionalContext can be used to specific the locals
context in which the message is evaluated.

## doRelativeFile(pathString)

Evaluates the File in the context of the receiver. Returns the result.
pathString is relative to the file calling doRelativeFile. (Duplicate of relativeDoFile)

## doString(aString)

Evaluates the string in the context of the receiver. Returns the result.

## evalArg(expression)

The '' method evaluates the argument and returns the result.

## evalArgAndReturnNil(expression)

Evaluates the argument and returns nil.

## evalArgAndReturnSelf(expression)

Evaluates the argument and returns the target.

## for(<counter>, <start>, <end>, <do message>)

A for-loop control structure. See the io Programming Guide for a full
description.

## foreach([name,] value, message)

For each slot, set name to the slot's
name and value to the slot's value and execute message. Examples:
<p>
<pre>
myObject foreach(n, v,
writeln("slot ", n, " = ", v type)
)

myObject foreach(v,
writeln("slot type ", v type)
)
</pre>

## foreachSlot(slotName, slotValue, code)

Iterates over all the slots in a receiver. Provides slotValue (non-activated)
along with slotName. Code is executed in context of sender. <tt>slotName</tt> and <tt>slotValue</tt>
become visible in the receiver (no Locals created! Maybe, it is not the best decision).
<br/>
<pre>
Io> thisContext foreachSlot(n, v, n println)
Lobby
Protos
exit
forward
n
v
==> false
</pre>

## forward

Called when the receiver is sent a message it doesn't recognize.
Default implementation raises an "Object doesNotRespond" exception.
Subclasses can override this method to implement proxies or special error
handling. <p> Example: <p> <pre> myProxy forward = method( messageName :=
thisMessage name arguments := thisMessage arguments myObject
doMessage(thisMessage)
)
</pre>

## getLocalSlot(slotNameString)

Returns the value of the slot named slotNameString
(not looking in the object's protos) or nil if no such slot is found.

## getSlot(slotNameString)

Returns the value of the slot named slotNameString
(following the lookup path) or nil if no such slot is found.

## handleActorException(exception)

Callback for handling exceptions during asynchronous message processing.
<br/>
Default value: method(e, e showStack)

## hasLocalSlot(slotNameString)

Returns true if the slot exists in the receiver or false otherwise.

## hasProto(anObject)

Returns true if anObject is found in the proto path of the target, false
otherwise.

## hasSlot(name)

Returns <tt>true</tt> if slot is found somewhere in the inheritance chain
(including receiver itself).

## if(<condition>, <trueMessage>, <optionalFalseMessage>)

Evaluates trueMessage if condition evaluates to a non-Nil.
Otherwise evaluates optionalFalseMessage if it is present.
Returns the result of the evaluated message or Nil if none was evaluated.

## ifError

Does nothing, returns self.

## ifNil(arg)

Does nothing, returns self.

## ifNilEval(arg)

Does nothing, returns self.

## ifNonNil(arg)

Evaluates argument and returns self.

## ifNonNilEval(arg)

Evaluates argument and returns the result.

## in(aList)

Same as: aList contains(self)

## inlineMethod

Creates a method which is executed directly in a receiver (no Locals object is created).
<br/>
<pre>
Io> m := inlineMethod(x := x*2)
Io> x := 1
==> 1
Io> m
==> 2
Io> m
==> 4
Io> m
==> 8
</pre>

## isActivatable

Returns true if the receiver is activatable, false otherwise.

## isError

Returns false if not an error.

## isIdenticalTo(aValue)

Returns true if the receiver is identical to aValue, false otherwise.

## isKindOf(anObject)

Returns true if anObject is in the receiver's ancestors.

## isLaunchScript

Returns true if the current file was run on the command line. Io's version of Python's __file__ == "__main__"

## isNil

Returns false.

## isTrue

Returns true.

## justSerialized(stream)

Writes serialized representation to a SerializationStream. Returns stream contents.
[This is unintended side effect! Returned value may change in the future.]

## launchFile(pathString)

Eval file at pathString as if from the command line in its folder.

## lazySlot(code)

Defines a slot with a lazy initialization code.
Code is run only once: the first time slot is accessed.
Returned value is stored in a regular slot.
<br/>
<pre>
Io> x := lazySlot("Evaluated!" println; 17)
Io> x
Evaluated!
==> 17
Io> x
==> 17
Io> x
==> 17
</pre>
<br/>
Another form is <tt>lazySlot(name, code)</tt>:
<br/>
<pre>
Io> lazySlot("x", "Evaluated!" println; 17)
Io> x
Evaluated!
==> 17
Io> x
==> 17
Io> x
==> 17
</pre>

## lexicalDo(expression)

Evaluates the message in the context of the receiver.
The lexical context is added as a proto of the receiver while the argument
is evaluated. Returns self.

## list(...)

Returns a List containing the arguments.

## localsForward

CFunction used by Locals prototype for forwarding.

## localsUpdateSlot(slotNameString, valueObject)

Local's version of updateSlot mthod.

## loop(expression)

Keeps evaluating message until a break.

## markClean

Cleans object's slots.

## memorySize

Return the amount of memory used by the object.

## memorySizeOfState

Returns the number of bytes in the IoState
(this may not include memory allocated by C libraries).

## message(expression)

Return the message object for the argument or Nil if there is no argument.
Note: returned object is a mutable singleton. Use "message(foo) clone" if
you wish to modify it.

## method(args..., body)

Creates a method.
<tt>args</tt> is a list of formal arguments (can be empty). <br/>
<tt>body</tt> is evaluated in the context of Locals object.<br/>
Locals' proto is a message receiver (i.e. self).
<br/>
Slot with a method is <em>activatable</em>. Use getSlot(name) to
retrieve method object without activating it (i.e. calling). <br/> See
also <tt>Object block</tt>.

## newSlot(slotName, aValue)

Creates a getter and setter for the slot with the name slotName
and sets its default value to aValue. Returns self. For example,
newSlot("foo", 1) would create slot named foo with the value 1 as well as a setter method setFoo().

## not

Returns nil.

## or(arg)

Returns true.

## ownsSlots

A debug method.

## pause

Removes current coroutine from the yieldingCoros queue and
yields to another coro. Exits if no coros left.
<br/>
See Coroutine documentation for more details.

## perform(methodName, <arg1>, <arg2>, ...)

Performs the method corresponding to methodName with the arguments supplied.

## performWithArgList(methodName, argList)

Performs the method corresponding to methodName with the arguments in the
argList.

## prependProto(anObject)

Prepends anObject to the receiver's proto list. Returns self.

## print

Prints a string representation of the object. Returns Nil.

## println

Same as print, but also prints a new line. Returns self.

## proto

Same as; method(self protos first)

## protos

Returns a copy of the receiver's protos list.

## raiseIfError

Does nothing, returns self.

## relativeDoFile(pathString)

Evaluates the File in the context of the receiver. Returns the result.
pathString is relative to the file calling doRelativeFile. (Duplicate of doRelativeFile)

## removeAllProtos

Removes all of the receiver's protos. Returns self.

## removeAllSlots

Removes all of the receiver's slots. Returns self.

## removeProto(anObject)

Removes anObject from the receiver's proto list if it
is present. Returns self.

## removeSlot(slotNameString)

Removes the specified slot (only) in the receiver if it exists. Returns
self.

## resend

Send the message used to activate the current method to the Object's proto.
For example:
<pre>
Dog := Mammal clone do(
init := method(
resend
)
)
</pre>
Calling Dog init will send an init method to Mammal, but using the Dog's context.

## return(anObject)

Return anObject from the current execution block.

## returnIfError

Does nothing, returns self.

## returnIfNonNil

Returns the receiver from the current execution block if it is non nil.
Otherwise returns the receiver locally.

## self

Returns self.

## serialized

Returns a serialized representation of the receiver.
<br/>
<pre>
Io> Object clone do(x:=1) serialized
==> Object clone do(
x := 1
)
</pre>

## serializedSlots(stream)

Writes all slots to a stream.

## serializedSlotsWithNames(names,

stream) Writes selected slots to a stream.

## setIsActivatable(aValue)

When called with a non-Nil aValue, sets the object
to call its activate slot when accessed as a value. Turns this behavior
off if aValue is Nil. Only works on Objects which are not Activatable
Primitives (such as CFunction or Block). Returns self.

## setProto(anObject)

Sets the first proto of the receiver to anObject, replacing the
current one, if any. Returns self.

## setProtos(aList)

Replaces the receiver's protos with a copy of aList. Returns self.

## setSlot(slotNameString, valueObject)

Sets the slot slotNameString in the receiver to
hold valueObject. Returns valueObject.

## setSlotWithType(slotNameString, valueObject)

Sets the slot slotNameString in the receiver to
hold valueObject and sets the type slot of valueObject
to be slotNameString. Returns valueObject.

## shallowCopy

Returns a shallow copy of the receiver.

## slotDescriptionMap

Returns raw map of slot names and short values' descriptions.
See also <tt>Object slotSummary</tt>.

## slotNames

Returns a list of strings containing the names of the
slots in the receiver (but not in its lookup path).

## slotSummary

Returns a formatted <tt>slotDescriptionMap</tt>.
<br/>
<pre>
Io> slotSummary
==>  Object_0x30c590:
Lobby            = Object_0x30c590
Protos           = Object_0x30c880
exit             = method(...)
forward          = method(...)
</pre>

## slotValues

Returns a list of the values held in the slots of the receiver.

## stopStatus

Returns the internal IoState->stopStatus.

## super(aMessage)

Sends the message aMessage to the receiver's proto with the context of self. Example:
<pre>
self test(1, 2)   // performs test(1, 2) on self
super(test(1, 2)) // performs test(1, 2) on self proto but with the context of self
</pre>

## switch(<key1>, <expression1>, <key2>, <expression2>, ...)

Execute an expression depending on the value of the caller. (This is an equivalent to C switch/case)
<code>
hour := Date hour switch(
12, "midday",
0, "midnight",
17, "teatime",
Date hour asString
)
</code>

## thisContext

Synonym to self.

## thisLocalContext

Returns current locals.

## thisMessage

Returns the calling message (i.e. thisMessage itself, huh).

## try(code)

Executes particular code in a new coroutine.
Returns exception or nil if no exception is caught.
<br/>
See also documentation for Exception catch and pass.

## type

Returns a string containing the name of the type of Object (Number, String,
etc).

## uniqueHexId

Returns uniqueId in a hexadecimal form (with a "0x" prefix)
<pre>
Io> Object uniqueId
==> 3146784
Io> Object uniqueHexId
==> 0x300420
</pre>

## uniqueId

Returns a Number containing a unique id for the receiver.

## updateSlot(slotNameString, valueObject)

Same as setSlot(), but raises an error if the slot does not
already exist in the receiver's slot lookup path.

## wait(s)

Pauses current coroutine for at least <tt>s</tt> seconds.
<br/>
Note: current coroutine may wait much longer than designated number of seconds
depending on circumstances.

## while(<condition>, expression)

Keeps evaluating message until condition return Nil.
Returns the result of the last message evaluated or Nil if none were
evaluated.

## withHandler(exceptionProto, handlerBlock, body)

Installs a resumable exception handler for exceptions matching exceptionProto,
runs body, then removes the handler. The handlerBlock receives (exception, resume).
To resume at the signal site, call resume invoke(value) or simply return a value.

## write(<any number of arguments>)

Sends a print message to the evaluated result of each argument. Returns Nil.

## writeln(<any number of arguments>)

Same as write() but also writes a return character at the end. Returns Nil.

## yield

Yields to another coroutine. Does nothing if yieldingCoros queue is empty.
<br/>
See Coroutine documentation for more details.

