# Coroutine

Object wrapper for an Io coroutine.
Now implemented using frame-based evaluation (no platform-specific assembly).

- **category**: Core
- **copyright**: Steve Dekorte 2002, 2025
- **license**: BSD revised

## backTraceString

Returns a formatted callStack output along with exception info (if any). In case of CGI script, wraps output with &lt;code&gt; tag.

## callStack

Returns a list of Call objects.

## currentCoroutine

Returns currently running coroutine in Io state.

## currentFrame

Returns the current (topmost) EvalFrame of this coroutine,
or nil if no frames are active. Only works for the currently
executing coroutine.

## debugWriteln

See <tt>Object debugWriteln</tt>.

## exception

Returns the current exception or nil if there is none.

## freeStack

Frees all the internal data from the receiver's stack. Returns self.

## handlerStack

The list of resumable exception handler entries for this coroutine, or nil.

## ignoredCoroutineMethodNames

List of methods to ignore when building a <tt>callStack</tt>.

## implementation

Returns coroutine implementation type: "frame-based" (portable, no assembly)

## inException

Set to true when processing an exception in the coroutine.

## ioStack

Returns List of values on this coroutine's stack.

## isCurrent

Returns true if the receiver is currently running coroutine.

## isYielding

Returns true if the receiver is yielding (not paused or running).

## label

A label slot useful for debugging purposes.

## main

[Seems to be obsolete!] Executes runMessage, resumes parent coroutine.

## parentCoroutine

Returns the parent coroutine this one was chained from or nil if it wasn't chained. When a Coroutine ends, it will attempt to resume its parent.

## pause

Removes current coroutine from the yieldingCoros queue and
yields to another coro. <tt>System exit</tt> is executed if no coros left.
<br/>
You can resume a coroutine using either <tt>resume</tt> or <tt>resumeLater</tt> message.

## pauseCurrentAndResumeSelf

Pauses current coroutine and yields to a receiver.

## raiseException

Sets exception in the receiver and signals the eval loop to unwind.

## rawSignalException

Bridges an Io-level exception to the eval loop by setting errorRaised.
Called by raiseException when there is no parent coroutine to resume.
The exception should already be set on this coroutine via setException.

## recentInChain

Returns the coroutine in the coro chain which was most recently run, so you can pause and resume coroutine based constructs like try and actors.

## result

The result set when the coroutine ends.

## resume

Yields to the receiver. Runs the receiver if it is not running yet.
Returns self.

## resumeLater

Promotes receiver to the top of the yieldingCoros queue, but not yielding to it.
When current coroutine yields, receiver will resume.

## resumeParentCoroutine

Pauses current coroutine and resumes parent.

## run

Runs receiver and returns self.

## runLocals

The locals object in whose context the coroutine will send its run message.

## runMessage

The message to send to the runTarget when the coroutine starts.

## runTarget

The object which the coroutine will send a message to when it starts.

## setInException(aBool)

Set the inException status. Returns self.

## setLabel(aLabel)

Sets the comment label for the Coro. Return self.

## setMessageDebugging(aBoolean)

Turns on message level debugging for this coro. When on, this
coro will send a vmWillSendMessage message to the Debugger object before
each message send and pause itself. See the Debugger object documentation
for more information.

## setParentCoroutine(aCoro)

Sets the parent coroutine. Returns self.

## setRecentInChain(aCoro)

Sets the most recently run coroutine of the chain.
Sets all the recentInChain in a direct path up to the top of the coro chain
starting at this coro. Returns self.

## setStackSize(aNumber)

Sets the stack size in bytes to allocate for new Coros. Returns self.

## showStack

Writes backTraceString to STDOUT.

## showYielding

Prints a list of yielding coroutines to STDOUT.

## stackSize

Stack size allocated for each new coroutine. Coroutines will automatically chain themselves as need if more stack space is required.

## typeId

Returns <type>_<uniqueHexId> string.

## yield

Yields to another coroutine in the yieldingCoros queue.
Does nothing if yieldingCoros is empty.

## yieldCurrentAndResumeSelf

Yields to a receiver.

## yieldingCoros

Reference to Scheduler yieldingCoros.

