
Call do(
	//doc Call description Returns a description of the receiver as a String.
	description := method(
		m := self message
		s := self target type .. " " .. m name
		s alignLeft(36) .. " " .. m label lastPathComponent .. " " .. m lineNumber
	)

	/*doc Call delegateTo(target, altSender)
	Sends the call's message to target (and relays it's stop status). 
	The sender is set to altSender, if it is supplied.
	Returns the result of the message.
	*/
	delegateTo := method(target, altSender,
		call relayStopStatus(target doMessage(self message clone setNext, altSender ifNilEval(self sender)))
	)

	/*doc Call delegateToMethod(target, methodName)
	Sends the call's message to target via the method specified by methodName. 
	Returns the result of the message.
	*/
	delegateToMethod := method(target, methodName,
		call relayStopStatus(target doMessage(self message clone setNext setName(methodName), self sender))
	)

	/*doc Call evalArgs
	Returns a list containing the call message arguments evaluated in the context of the sender.
	*/
	evalArgs := method(self message argsEvaluatedIn(sender))
	
	/*doc Call hasArgs
	Returns true if the call was passed arguments.
	*/
	hasArgs  := method(argCount > 0)
	
	/*doc Call argCount
	Returns the number of arguments for the call. Same as call message argCount.
	*/
	argCount := method(self message argCount)
)

//doc Message description Returns a string containing a short description of the method.
Message description := method(
	self name alignLeft(36) .. self label lastPathComponent .. " " .. self lineNumber
)

Scheduler := Object clone do(
	//doc Scheduler yieldingCoros The List of yielding Coroutine objects.
	//doc Scheduler setYieldingCoros(aListOfCoros) Sets the list of yielding Coroutine objects.
	yieldingCoros ::= List clone
	
	//doc Scheduler timers The List of active timers.
	//doc Scheduler setTimers(aListOfTimers) Sets the list of active timers.
	timers ::= List clone
	
	//doc Scheduler currentCoroutine Returns the currently running coroutine.
	currentCoroutine := method(Coroutine currentCoroutine)
)

Coroutine do(
	//doc Coroutine stackSize Stack size allocated for each new coroutine. Coroutines will automatically chain themselves as need if more stack space is required.
	//doc Coroutine setStackSize
	stackSize ::= 128000 // PPC needs 128k for current parser
	
	//doc Coroutine exception Returns the current exception or nil if there is none.
	//doc Coroutine setException
	exception ::= nil
	
	//doc Coroutine parentCoroutine Returns the parent coroutine this one was chained from or nil if it wasn't chained. When a Coroutine ends, it will attempt to resume it's parent.
	//doc Coroutine setParentCoroutine
	parentCoroutine ::= nil
	
	//doc Coroutine runTarget The object which the coroutine will send a message to when it starts.
	//doc Coroutine setRunTarget
	runTarget ::= nil

	//doc Coroutine runLocals The locals object in whose context the coroutine will send it's run message.
	//doc Coroutine setRunLocals
	runLocals ::= nil

	//doc Coroutine runMessage The message to send to the runTarget when the coroutine starts.
	//doc Coroutine setRunMessage
	runMessage ::= nil

	//doc Coroutine result The result set when the coroutine ends.
	//doc Coroutine setResult 
	result ::= nil
	
	//doc Coroutine label A label slot useful for debugging purposes.
	//doc Coroutine setLabel
	label ::= ""

	//doc Coroutine inException Set to true when processing an exception in the coroutine.
	//doc Coroutine setInException
	inException ::= false
	
	//doc Coroutine yieldingCoros Reference to Scheduler yieldingCoros.
	//doc Coroutine setYieldingCoros
	yieldingCoros ::= Scheduler yieldingCoros
	//doc Coroutine debugWriteln (See <tt>Object debugWriteln</tt>.)
	debugWriteln := nil

	label := method(self uniqueId)
	setLabel := method(s, self label = s .. "_" .. self uniqueId)
  
  //doc Coroutine showYielding Prints a list of yielding coroutines to STDOUT.
	showYielding := method(s,
		writeln("   ", label, " ", s)
		yieldingCoros foreach(v, writeln("    ", v uniqueId))
	)
	
  //doc Coroutine isYielding Returns true if the receiver is yielding (not paused or running).
	isYielding := method(yieldingCoros contains(self))
  
  /*doc Coroutine yield
  Yields to another coroutine in the yieldingCoros queue.
  Does nothing if yieldingCoros is empty.
  */
	yield := method(
		//showYielding("yield")
		//writeln("Coro ", self uniqueId, " yielding - yieldingCoros = ", yieldingCoros size)
		if(yieldingCoros isEmpty, return)
		yieldingCoros append(self)
		next := yieldingCoros removeFirst
		if(next == self, return)
		//writeln(Scheduler currentCoroutine label, " yield - ", next label, " resume")
		if(next, next resume)
	)

  /*doc Coroutine resumeLater
  Promotes receiver to the top of the yieldingCoros queue, but not yielding to it.
  When current coroutine yields, receiver will resume.
  */
	resumeLater := method(
		yieldingCoros remove(self)
		yieldingCoros atInsert(0, self)
		//writeln(self label, " resumeLater")
	)

  /*doc Coroutine pause
	Removes current coroutine from the yieldingCoros queue and
	yields to another coro. <tt>System exit</tt> is executed if no coros left.
  */
	pause := method(
		yieldingCoros remove(self)
		if(isCurrent,
			next := yieldingCoros removeFirst
			if(next,
				next resume,
				//Exception raise("Scheduler: nothing left to resume so we are exiting")
				writeln("Scheduler: nothing left to resume so we are exiting")
				self showStack
				System exit
			)
			,
			yieldingCoros remove(self)
		)
	)

  //FIXME: these two methods are identical!!
  //doc Coroutine yieldCurrentAndResumeSelf Yields to a receiver.
	yieldCurrentAndResumeSelf := method(
		//showYielding("yieldCurrentAndResumeSelf")
		yieldingCoros remove(self)
		isCurrent ifFalse(resume)
	)
  //FIXME: these two methods are identical!!
  //doc Coroutine pauseCurrentAndResumeSelf Pauses current coroutine and yields to a receiver.
	pauseCurrentAndResumeSelf := method(
		//showYielding("pauseCurrentAndResumeSelf")
		yieldingCoros remove(self)
		isCurrent ifFalse(resume)
	)
  
  //doc Coroutine typeId Returns <type>_<uniqueHexId> string.
	typeId := method(self type .. "_0x" .. self uniqueId asString toBase(16))
    
  //doc Coroutine ignoredCoroutineMethodNames List of methods to ignore when building a <tt>callStack</tt>.
	ignoredCoroutineMethodNames := list("setResult", "main", "pauseCurrentAndResumeSelf", "resumeParentCoroutine", "raiseException")

  //doc Coroutine callStack Returns a list of Call objects.
	callStack := method(
		stack := ioStack
		stack selectInPlace(v, Object argIsCall(getSlot("v"))) reverseInPlace
		stack selectInPlace(v,
			(v target type == "Coroutine" and ignoredCoroutineMethodNames contains(v message name)) not
		)
		stack foreach(i, v, if(v target type == "Importer" and v message name == "import", stack sliceInPlace(i+1); break) )
		stack := stack unique
		stack
	)

  //doc Coroutine backTraceString Returns a formatted callStack output along with exception info (if any). In case of CGI script, wraps output with &lt;code&gt; tag.
	backTraceString := method(
		if(Coroutine inException,
			writeln("\n", exception type, ": ", exception error, "\n\n")
			writeln("Coroutine Exception loop detected");
			System exit
		)
		Coroutine setInException(true)
		buf := Sequence clone

		if(getSlot("CGI") != nil and CGI isInWebScript, buf appendSeq("<code>"))

		if(exception, buf appendSeq("\n  ", exception type, ": ", exception error, "\n"))

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

  //doc Coroutine showStack Writes backTraceString to STDOUT.
	showStack := method(write(backTraceString))
    
  //doc Coroutine resumeParentCoroutine Pauses current coroutine and resumes parent.
	resumeParentCoroutine := method(
		if(parentCoroutine, parentCoroutine pauseCurrentAndResumeSelf)
	)
  
  //doc Coroutine main [Seems to be obsolete!] Executes runMessage, resumes parent coroutine.
	main := method(
		setResult(self getSlot("runTarget") doMessage(runMessage, self getSlot("runLocals")))
		resumeParentCoroutine
		pause
	)
  
  //doc Coroutine raiseException Sets exception in the receiver and resumes parent coroutine.
	raiseException := method(e,
		self setException(e)
		resumeParentCoroutine
	)
)

/*doc Object wait(s)
Pauses current coroutine for at least <tt>s</tt> seconds.
<br/>
Note: current coroutine may wait much longer than designated number of seconds
depending on circumstances. 
*/

Object wait := method(s,
	endDate := Date clone now + Duration clone setSeconds(s)
	loop(endDate isPast ifTrue(break); yield)
)

Message do(
  /*doc Message codeOfLength(n)
  Same as <tt>Message code</tt>, but returns first <tt>n</tt> characters only.
  */
	codeOfLength := method(length,
		c := self code
		if (c size < length, c, c slice(0, length) .. "...") asMutable replaceSeq("\n", ";")
	)

  //doc Message asStackEntry Returns a string containing message name, file and line.
	asStackEntry := method(
		label := label lastPathComponent fileName
		label alignLeft(19) .. lineNumber asString alignLeft(7) .. name
	)
)

Object do(
  /*doc Object try(code)
  Executes particular code in a new coroutine.
  Returns exception or nil if no exception is caught.
  <br/>
  See also documentation for Exception catch and pass.  
  */
	try := method(
		coro := Coroutine clone
		coro setParentCoroutine(Scheduler currentCoroutine)
		coro setRunTarget(call sender)
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0))
		coro run
		if(coro exception, coro exception, nil)
	)

  /*doc Object coroFor(code)
  Returns a new coro to be run in a context of sender.
  */
	coroFor := method(
		coro := Coroutine clone
		coro setRunTarget(call sender)
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0))
		coro
	)

  /*doc Object coroDo(code)
  Creates a new coro to be run in a context of sender and yields to it.
  Returns a coro.
  */
	coroDo := method(
		coro := Coroutine clone
		coro setRunTarget(call sender)
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0))
		Coroutine yieldingCoros atInsert(0, Scheduler currentCoroutine)
		coro run
		coro
	)

  /*doc Object coroDoLater(code)
  Returns a new coro to be run in a context of sender.
  New coro is moved to the top of the yieldingCoros queue to be executed 
  when current coro yields.
  <br/>
  Note: run target is <tt>self</tt> (i.e. receiver), not <tt>call sender</tt> as in coroDo.
  */
	coroDoLater := method(
		coro := Coroutine clone
		coro setRunTarget(self)
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0))
		Coroutine yieldingCoros atInsert(0, coro)
		coro
	)

  /*doc Object coroWith(code)
  Returns a new coro to be run in a context of receiver.
  */
	coroWith := method(
		coro := Coroutine clone
		coro setRunTarget(self)
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0))
		coro
	)
  //doc Object currentCoro Returns the currently running coroutine.
	currentCoro := method(Coroutine currentCoroutine)
)

nil do(
  //doc nil catch Does nothing, returns nil. See <tt>Exception catch</tt>.
	//doc nil pass Does nothing, returns nil. See <tt>Exception pass</tt>.
	catch := nil
	pass := nil
)

Protos Exception do(
	type := "Exception"
	newSlot("error")
	newSlot("coroutine")
	newSlot("caughtMessage")
	newSlot("nestedException")
	newSlot("originalCall")

	raise := method(error, nestedException,
		coro := Scheduler currentCoroutine
		coro raiseException(self clone setError(error) setCoroutine(coro) setNestedException(nestedException))
	)

	raiseFrom := method(originalCall, error, nestedException,
		coro := Scheduler currentCoroutine
		coro raiseException(self clone setError(error) setCoroutine(coro) setNestedException(nestedException) setOriginalCall(originalCall))
	)

	catch := method(exceptionProto,
		if (self isKindOf(exceptionProto), call evalArgAt(1); nil, self)
	)

	pass := method(Scheduler currentCoroutine raiseException(self))

	showStack := method(
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
