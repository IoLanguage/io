

SystemCall do(
	newSlot("command", "")
	newSlot("isRunning", false)
	newSlot("returnCode", nil)
	newSlot("stdin", nil)
	newSlot("stdout", nil)
	newSlot("stderr", nil)
	newSlot("arguments", nil)
	newSlot("environment", Map clone)

	init := method(
		self arguments := List clone
		self environment := environment clone
	)

	run := method(aBlock,
		err := self asyncRun(command, arguments, environment)
		if(err == -1, Exception raise("unable to run command"))

		// replace this with something to watch the file streams?
		isRunning := true
		//writeln("self status = ", self status)
		wait(.00001)
		s := self status
		while(isRunning == true and s > 255 and s != -1,
			//writeln("self status = ", s)
			if(aBlock, if(aBlock call == false, return false))
			wait(.02)
			s := self status
		)
		//writeln("self status = ", s)
		if(aBlock, aBlock call)

		isRunning := false
		setReturnCode(s)
		self
	)

	runWith := method(
		run(Block clone setMessage(call argAt(0)) setScope(self))
	)

	with := method(s,
		newSysCall := self clone
		parts := s splitNoEmpties(" ")
		newSysCall setCommand(parts removeFirst)
		newSysCall setArguments(parts)
		newSysCall
	)
)
