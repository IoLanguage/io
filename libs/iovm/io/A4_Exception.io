
Call do(
	description := method(
		//doc Call description Returns a description of the receiver as a String.
		m := self message
		s := self target type .. " " .. m name
		s alignLeft(36) .. " " .. m label lastPathComponent .. " " .. m lineNumber
	)

	delegateTo := method(target, altSender,
		/*doc Call delegateTo(target, altSender)
		Sends the call's message to target (and relays it's stop status).
		The sender is set to altSender, if it is supplied.
		Returns the result of the message.
		*/
		call relayStopStatus(target doMessage(self message clone setNext, altSender ifNilEval(self sender)))
	)

	delegateToMethod := method(target, methodName,
		/*doc Call delegateToMethod(target, methodName)
		Sends the call's message to target via the method specified by methodName.
		Returns the result of the message.
		*/
		call relayStopStatus(target doMessage(self message clone setNext setName(methodName), self sender))
	)

	/*doc Call evalArgs
	Returns a list containing the call message arguments evaluated in the context of the sender.
	*/
	evalArgs := method(self message argsEvaluatedIn(sender)) setPassStops(true)

	//doc Call hasArgs Returns true if the call was passed arguments.
	hasArgs  := method(argCount > 0)

	//doc Call argCount Returns the number of arguments for the call. Shortcut for "call message argCount".
	argCount := method(self message argCount)
)

Message description := method(
	//doc Message description Returns a string containing a short description of the method.
	self name alignLeft(36) .. self label lastPathComponent .. " " .. self lineNumber
)

Scheduler := Object clone do(
	//metadoc Scheduler category Core
	//metadoc Scheduler description Io's coroutine scheduler.

	//doc Scheduler yieldingCoros The List of yielding Coroutine objects.
	//doc Scheduler setYieldingCoros(aListOfCoros) Sets the list of yielding Coroutine objects.
	yieldingCoros ::= List clone

	//doc Scheduler timers The List of active timers.
	//doc Scheduler setTimers(aListOfTimers) Sets the list of active timers.
	timers ::= List clone

	//doc Scheduler currentCoroutine Returns the currently running coroutine.
	currentCoroutine := method(Coroutine currentCoroutine)

	waitForCorosToComplete := method(
		while(yieldingCoros size > 0, yield)
	)
)

Coroutine do(
	//metadoc Coroutine category Core
	//metadoc Coroutine description Coroutine is an primitive for Io's lightweight cooperative C-stack based threads.

	init := method(
		recentInChain = nil
	)
	
	//doc Coroutine stackSize Stack size allocated for each new coroutine. Coroutines will automatically chain themselves as need if more stack space is required.
	//doc Coroutine setStackSize(aNumber) Sets the stack size in bytes to allocate for new Coros. Returns self.
	//stackSize ::= 131072 // PPC needs 128k for current parser
	stackSize ::= 131072 // PPC needs 128k for current parser

	//doc Coroutine exception Returns the current exception or nil if there is none.
	//doc Coroutine setException
	exception ::= nil

	//doc Coroutine parentCoroutine Returns the parent coroutine this one was chained from or nil if it wasn't chained. When a Coroutine ends, it will attempt to resume its parent.

	//doc Coroutine setParentCoroutine(aCoro) Sets the parent coroutine. Returns self.
	parentCoroutine ::= nil
	
	//doc Coroutine recentInChain Returns the coroutine in the coro chain which was most recently run, so you can pause and resume coroutine based constructs like try and actors.
	recentInChain := Coroutine

	//doc Coroutine runTarget The object which the coroutine will send a message to when it starts.
	//doc Coroutine setRunTarget(anObject)
	runTarget ::= nil

	//doc Coroutine runLocals The locals object in whose context the coroutine will send its run message.
	//doc Coroutine setRunLocals
	runLocals ::= nil

	//doc Coroutine runMessage The message to send to the runTarget when the coroutine starts.
	//doc Coroutine setRunMessage
	runMessage ::= nil

	//doc Coroutine result The result set when the coroutine ends.
	//doc Coroutine setResult
	result ::= nil

	//doc Coroutine label A label slot useful for debugging purposes.
	//doc Coroutine setLabel(aLabel) Sets the comment label for the Coro. Return self.
	label ::= ""

	//doc Coroutine inException Set to true when processing an exception in the coroutine.
	//doc Coroutine setInException(aBool) Set the inException status. Returns self.
	inException ::= false


	yieldingCoros ::= Scheduler yieldingCoros
	//doc Coroutine yieldingCoros Reference to Scheduler yieldingCoros.
	//--doc Coroutine setYieldingCoros(aListOfCoros)

	debugWriteln := nil
	//doc Coroutine debugWriteln See <tt>Object debugWriteln</tt>.

	label := method(self uniqueId)
	setLabel := method(s, self label = s .. "_" .. self uniqueId)

	showYielding := method(s,
		//doc Coroutine showYielding Prints a list of yielding coroutines to STDOUT.
		writeln("   ", label, " ", s)
		yieldingCoros foreach(v, writeln("    ", v uniqueId))
	)

	isYielding := method(yieldingCoros contains(self))
	//doc Coroutine isYielding Returns true if the receiver is yielding (not paused or running).

	yield := method(
		/*doc Coroutine yield
		Yields to another coroutine in the yieldingCoros queue.
		Does nothing if yieldingCoros is empty.
		*/
		//showYielding("yield")
		//writeln("Coro ", self uniqueId, " yielding - yieldingCoros = ", yieldingCoros size)
		if(yieldingCoros isEmpty, return)
		yieldingCoros append(self)
		next := yieldingCoros removeFirst
		if(next == self, return)
		//writeln(Scheduler currentCoroutine label, " yield - ", next label, " resume")
		if(next, next resume)
	)

	resumeLater := method(
		/*doc Coroutine resumeLater
		Promotes receiver to the top of the yieldingCoros queue, but not yielding to it.
		When current coroutine yields, receiver will resume.
		*/
		//waitingOn remove(call sender)
		//if(waitingOn isEmpty,
			yieldingCoros remove(self)
			yieldingCoros atInsert(0, self)
		//)
		//writeln(self label, " resumeLater")
	)


	pause := method(
		/*doc Coroutine pause
		Removes current coroutine from the yieldingCoros queue and
		yields to another coro. <tt>System exit</tt> is executed if no coros left.
		<br/>
		You can resume a coroutine using either <tt>resume</tt> or <tt>resumeLater</tt> message.
		*/
		yieldingCoros remove(self)
		if(isCurrent,
			next := yieldingCoros removeFirst
			if(next,
				next resume
			,
				Exception raise("Scheduler: nothing left to resume so we are exiting")
				writeln("Scheduler: nothing left to resume so we are exiting")
				self showStack
				System exit
			)
		,
			yieldingCoros remove(self)
		)
	)

	//FIXME: these two methods are identical!!
	yieldCurrentAndResumeSelf := method(
		//doc Coroutine yieldCurrentAndResumeSelf Yields to a receiver.
		//showYielding("yieldCurrentAndResumeSelf")
		yieldingCoros remove(self)
		isCurrent ifFalse(resume)
	)

	//FIXME: these two methods are identical!!
	pauseCurrentAndResumeSelf := method(
		//doc Coroutine pauseCurrentAndResumeSelf Pauses current coroutine and yields to a receiver.
		//showYielding("pauseCurrentAndResumeSelf")
		yieldingCoros remove(self)
		isCurrent ifFalse(resume)
	)

	typeId := method(self type .. "_0x" .. self uniqueId asString toBase(16))
	//doc Coroutine typeId Returns <type>_<uniqueHexId> string.

	ignoredCoroutineMethodNames := list("setResult", "main", "pauseCurrentAndResumeSelf", "resumeParentCoroutine", "raiseException")
  	//doc Coroutine ignoredCoroutineMethodNames List of methods to ignore when building a <tt>callStack</tt>.

	callStack := method(
		//doc Coroutine callStack Returns a list of Call objects.
		stack := ioStack
		stack selectInPlace(v, Object argIsCall(getSlot("v"))) reverseInPlace
		stack selectInPlace(v,
			(v target type == "Coroutine" and \
             self ignoredCoroutineMethodNames contains(v message name)) not
		)
		stack foreach(i, v, if(v target type == "Importer" and v message name == "import", stack sliceInPlace(i+1); break) )
		stack := stack unique
		//if(parentCoroutine and parentCoroutine != self, stack appendSeq(parentCoroutine callStack))
		stack
	)

	backTraceString := method(
	    //doc Coroutine backTraceString Returns a formatted callStack output along with exception info (if any). In case of CGI script, wraps output with &lt;code&gt; tag.
		if(Coroutine inException,
			writeln("\n", exception type, ": ", exception error, "\n\n")
			writeln("Coroutine Exception loop detected");
			System exit
		)
		Coroutine setInException(true)
		buf := Sequence clone

		//writeln("backTraceString 1\n")
		if(getSlot("CGI") != nil and CGI isInWebScript, buf appendSeq("<code>"))

		if(exception, buf appendSeq("\n  ", exception type, ": ", exception error, "\n"))

		//writeln("backTraceString 2\n")
		if(callStack size > 0) then(
			buf appendSeq("  ---------\n")

			if(exception and exception ?caughtMessage,
				buf appendSeq("  ", exception caughtMessage description, "\n")
			)

			frames := callStack

			if(exception and exception originalCall,
				index := frames indexOf(exception originalCall)
				if(index,
					frames sliceInPlace(index)
				)
			)

			frames foreach(v,
				buf appendSeq("  ", v description, "\n")
			)
			buf appendSeq("\n")
		) else(
			buf appendSeq("  ---------\n")
			if(exception and exception caughtMessage,
				m := exception caughtMessage
				buf appendSeq("  message '" .. m name .. "' in '" .. m label .. "' on line " .. m lineNumber .. "\n")
				buf appendSeq("\n")
			,
				buf appendSeq("  nothing on stack\n")
			)
		)

		Coroutine setInException(false)
		buf
	)

	showStack := method(
		//doc Coroutine showStack Writes backTraceString to STDOUT.
		write(backTraceString)
	)

	resumeParentCoroutine := method(
		//doc Coroutine resumeParentCoroutine Pauses current coroutine and resumes parent.
		if(parentCoroutine, parentCoroutine pauseCurrentAndResumeSelf)
	)

	main := method(
		//doc Coroutine main [Seems to be obsolete!] Executes runMessage, resumes parent coroutine.
		setResult(self getSlot("runTarget") doMessage(runMessage, self getSlot("runLocals")))
		resumeParentCoroutine
		pause
	)

	raiseException := method(e,
		//doc Coroutine raiseException Sets exception in the receiver and resumes parent coroutine.
		self setException(e)
		resumeParentCoroutine
	)
)

Object wait := method(s,
	/*doc Object wait(s)
	Pauses current coroutine for at least <tt>s</tt> seconds.
	<br/>
	Note: current coroutine may wait much longer than designated number of seconds
	depending on circumstances.
	*/

	//writeln("Scheduler yieldingCoros size = ", Scheduler yieldingCoros size)
	if(Scheduler yieldingCoros isEmpty,
		//writeln("System sleep")
		System sleep(s)
	,
		//writeln("Object wait")
		endDate := Date clone now + Duration clone setSeconds(s)
		loop(endDate isPast ifTrue(break); yield)
	)
)

Message do(
	codeOfLength := method(length,
		/*doc Message codeOfLength(n)
		Same as <tt>Message code</tt>, but returns first <tt>n</tt> characters only.
		*/
		c := self code
		if (c size < length, c, c exclusiveSlice(0, length) .. "...") asMutable replaceSeq("\n", ";")
	)

	asStackEntry := method(
		//doc Message asStackEntry Returns a string containing message name, file and line.
		label := label lastPathComponent fileName
		label alignLeft(19) .. lineNumber asString alignLeft(7) .. name
	)
)

Object do(
	try := method(
		/*doc Object try(code)
		Executes particular code in a new coroutine.
		Returns exception or nil if no exception is caught.
		<br/>
		See also documentation for Exception catch and pass.
		*/
		coro := Coroutine clone
		coro setParentCoroutine(Scheduler currentCoroutine)
		coro setRunTarget(call sender)
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0))
		coro run
		if(coro exception, coro exception, nil)
	)


	coroFor := method(
		/*doc Object coroFor(code)
		Returns a new coro to be run in a context of sender.
		*/
		coro := Coroutine clone
		coro setRunTarget(call sender)
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0))
		coro
	)

	coroDo := method(
		/*doc Object coroDo(code)
		Creates a new coro to be run in a context of sender and yields to it.
		Returns a coro.
		*/
		coro := Coroutine clone
		coro setRunTarget(call sender)
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0))
		Coroutine yieldingCoros atInsert(0, Scheduler currentCoroutine)
		coro run
		coro
	)


	coroDoLater := method(
		/*doc Object coroDoLater(code)
		Returns a new coro to be run in a context of sender.
		New coro is moved to the top of the yieldingCoros queue to be executed
		when current coro yields.
		<br/>
		Note: run target is <tt>self</tt> (i.e. receiver), not <tt>call sender</tt> as in coroDo.
		*/
		coro := Coroutine clone
		coro setRunTarget(self)
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0))
		Coroutine yieldingCoros atInsert(0, coro)
		coro
	)


	coroWith := method(
		//doc Object coroWith(code) Returns a new coro to be run in a context of receiver.
		coro := Coroutine clone
		coro setRunTarget(self)
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0))
		coro
	)

	currentCoro := method(
		//doc Object currentCoro Returns the currently running coroutine.
		Coroutine currentCoroutine
	)
)

nil do(
	catch := nil
	//doc nil catch Does nothing, returns nil. See <tt>Exception catch</tt>.

	pass := nil
	//doc nil pass Does nothing, returns nil. See <tt>Exception pass</tt>.
)

Protos Exception do(
	//metadoc Exception category Core
	/*metadoc Exception description
The Exception proto is used for raising exceptions and instances are used to hold rexception related info.

<p><b>Raise</b><p>

An exception can be raised by calling raise() on an exception proto.
Exception raise("generic foo exception")

<p><b>Try and Catch</b><p>

To catch an exception, the try() method of the Object proto is used. try() will catch any exceptions that occur within it and return the caught exception or nil if no exception is caught.

<pre>
e := try(<doMessage>)
</pre>

To catch a particular exception, the Exception catch() method can be used. Example:

<pre>
e := try(
    // ...
)

e catch(Exception,
    writeln(e coroutine backtraceString)
)
</pre>

The first argument to catch indicates which types of exceptions will be caught. catch() returns the exception if it doesn't match and nil if it does.

<p><b>Pass</b><p>

To re-raise an exception caught by try(), use the pass method. This is useful to pass the exception up to the next outer exception handler, usually after all catches failed to match the type of the current exception:
<pre>
e := try(
    // ...
)

e catch(Error,
    // ...
) catch(Exception,
    // ...
) pass
</pre>

<p><b>Custom Exceptions</b><p>

Custom exception types can be implemented by simply cloning an existing Exception type:
<pre>
MyErrorType := Error clone
</pre>

*/

	type := "Exception"
	newSlot("error")
	//doc Exception error Returns the error description string.

	newSlot("coroutine")
	//doc Exception error Returns the coroutine that the exception occurred in.

	newSlot("caughtMessage")
	//doc Exception caughtMessage Returns the message object associated with the exception.

	newSlot("nestedException")
	//doc Exception nestedException Returns the nestedException if there is one.

	newSlot("originalCall")
	//doc Exception originalCall Returns the call object associated with the exception.

	raise := method(error, nestedException,
		//doc Exception raise(error, optionalNestedException) Raise an exception with the specified error message.
		coro := Scheduler currentCoroutine
		coro raiseException(self clone setError(error) setCoroutine(coro) setNestedException(nestedException))
	)

	raiseFrom := method(originalCall, error, nestedException,
		coro := Scheduler currentCoroutine
		coro raiseException(self clone setError(error) setCoroutine(coro) setNestedException(nestedException) setOriginalCall(originalCall))
	)

	catch := method(exceptionProto,
		//doc Exception catch(exceptionProto) Catch an exception with the specified exception prototype.
		if (self isKindOf(exceptionProto), call evalArgAt(1); nil, self)
	)

	pass := method(
		//doc Exception pass Pass the exception up the stack.
		Scheduler currentCoroutine raiseException(self)
	)

	showStack := method(
		//doc Exception showStack Print the exception and related stack.
		coroutine showStack
		if(nestedException,
				writeln("Nested Exception: '", nestedException,  "'")
				nestedException showStack
		)
	)
)

System userInterruptHandler := method(
	writeln("\nStack trace:\n")
	Scheduler currentCoroutine showStack
	exit
)
