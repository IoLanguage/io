
Call do(
	docSlot("description", "Returns a description of the receiver as a String.")

	description := method(
		m := self message
		s := self target type .. " " .. m name
        s alignLeft(36) .. m label lastPathComponent .. " " .. m lineNumber
	)

	delegateTo := method(target, altSender,
		call relayStopStatus(target doMessage(self message clone setNext, altSender ifNilEval(self sender)))
	)

    delegateToMethod := method(target, methodName,
        call relayStopStatus(target doMessage(self message clone setNext setName(methodName), self sender))
    )

	evalArgs := method(self message argsEvaluatedIn(sender))
    hasArgs  := method(argCount > 0)
    argCount := method(self message argCount)
)

Message description := method(
	self name alignLeft(36) .. self label lastPathComponent .. " " .. self lineNumber	
)
	
Scheduler := Object clone do(
	newSlot("yieldingCoros", List clone)
	newSlot("timers", List clone)
	currentCoroutine := method(Coroutine currentCoroutine)
)

Coroutine do(
	newSlot("stackSize", 128000) // PPC needs 128k for current parser
	newSlot("exception")
	newSlot("parentCoroutine")
	newSlot("runTarget")
	newSlot("runLocals")
	newSlot("runMessage")
	newSlot("result")
	newSlot("label", "")
	newSlot("inException", false)
	newSlot("yieldingCoros", Scheduler yieldingCoros)
	debugWriteln := nil
	
	label := method(self uniqueId)
	setLabel := method(s, self label = s .. "_" .. self uniqueId)
	
	showYielding := method(s,
		writeln("   ", label, " ", s)
		yieldingCoros foreach(v, writeln("    ", v uniqueId))
	)

    isYielding := method(yieldingCoros contains(self))
    
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
	
    resumeLater := method(
        yieldingCoros remove(self)
		yieldingCoros atInsert(0, self)
		//writeln(self label, " resumeLater")
	)
		
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
		
	yieldCurrentAndResumeSelf := method(
		//showYielding("yieldCurrentAndResumeSelf")
		yieldingCoros remove(self)
		isCurrent ifFalse(resume)
	)
	
	pauseCurrentAndResumeSelf := method(
		//showYielding("pauseCurrentAndResumeSelf")
		yieldingCoros remove(self)
		isCurrent ifFalse(resume)
	)

	typeId := method(self type .. "_0x" .. self uniqueId asString toBase(16))

	callStack := method(
		stack := ioStack
		stack selectInPlace(v, Object argIsCall(getSlot("v"))) reverse 
		stack selectInPlace(v, (v target type == "Coroutine" and v message name == "setResult") not)  
		stack selectInPlace(v, (v target type == "Coroutine" and v message name == "main") not)  
		stack foreach(i, v, if(v target type == "Importer" and v message name == "find", stack sliceInPlace(i+1); break) )
		stack := stack unique
		stack
	)
	
	backTraceString := method(
		if(Coroutine inException, 
			writeln("\n", exception type, ": ", exception error, "\n\n")
			writeln("Coroutine Exception loop detected"); 
			System exit
		)
		Coroutine setInException(true)
		buf := Sequence clone
		
		if(getSlot("CGI") != nil and CGI isInWebScript, buf appendSeq("<pre>"))
		
		if(exception, buf appendSeq("\n  ", exception type, ": ", exception error, "\n"))
		//buf appendSeq("  ", self typeId, " stack trace")
		
		if(callStack size > 0) then(
			buf appendSeq("  ---------\n")
			
			if(exception and exception caughtMessage, 
				buf appendSeq("  ", exception caughtMessage description, "\n")
			)
			
			callStack foreach(v, buf appendSeq("  ", v description, "\n"))
			buf appendSeq("\n")
		) else(
			//buf appendSeq("    no call stack found\n")
			buf appendSeq("\n")
		)

		Coroutine setInException(false)
		buf
	)
	
	showStack := method(write(backTraceString))

	resumeParentCoroutine := method(
		if(parentCoroutine, parentCoroutine pauseCurrentAndResumeSelf)
	)

	main := method(
		setResult(runTarget doMessage(runMessage, runLocals))
		resumeParentCoroutine
	    pause
	)

	raiseException := method(e, 
		self setException(e)
		resumeParentCoroutine
	)
)

Object wait := method(s, 
	endDate := Date clone now + Duration clone setSeconds(s)
	loop(endDate isPast ifTrue(break); yield)
)

Message do(
	codeOfLength := method(length,
		c := self code
		if (c size < length, c, c slice(0, length) .. "...") asMutable replaceSeq("\n", ";")
	)

	asStackEntry := method(
		label := label lastPathComponent fileName
        label alignLeft(19) .. lineNumber asString alignLeft(7) .. name
	)
)

Object do(
	try := method(
		coro := Coroutine clone
		coro setParentCoroutine(Scheduler currentCoroutine) 
		coro setRunTarget(call sender) 
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0)) 
		coro run
		if(coro exception, coro exception, nil)
	)

	coroFor := method(
		coro := Coroutine clone
		coro setRunTarget(call sender) 
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0)) 
		coro
	)
		
	coroDo := method(
		coro := Coroutine clone
		coro setRunTarget(call sender) 
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0)) 
		Coroutine yieldingCoros atInsert(0, Scheduler currentCoroutine)
		coro run
		coro
	)
	
    coroDoLater := method(
		coro := Coroutine clone
		coro setRunTarget(self) 
		coro setRunLocals(call sender)
		coro setRunMessage(call argAt(0)) 
		Coroutine yieldingCoros atInsert(0, coro)
		coro
	)
	
	currentCoro := method(Coroutine currentCoroutine)
)

nil do(
	catch := nil
	pass := nil
)

Protos Exception do(
	type := "Exception"
	newSlot("error")
	newSlot("coroutine")
	newSlot("caughtMessage")
    newSlot("nestedException")
	
	raise := method(error, nestedException,
		coro := Scheduler currentCoroutine
		coro raiseException(self clone setError(error) setCoroutine(coro) setNestedException(nestedException))
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
