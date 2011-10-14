Socket

SystemCall do(
	command ::= ""
	isRunning ::= false
	returnCode ::= nil
	stdin ::= nil
	stdout ::= nil
	stderr ::= nil
	arguments ::= nil
	environment ::= Map clone
	readEvent ::= nil
	writeEvent ::= nil
	readTimeout ::= 1000
	writeTimeout ::= 1000

	init := method(
		self arguments := List clone
		self environment := environment clone
		setReadEvent(ReadEvent clone)
		setWriteEvent(WriteEvent clone)
	)

	tryToRun := method(times,
		if(times == nil, times = 3)
		times repeat(
			e := try(run)
			if(e == nil, break)
			writeln("RETRYING: Runable to run command: '", command, "'")
		)
		if(e, e pass)
		self
	)
	
	commandString := method(
		command .. " " .. arguments map(a, "'" .. a .. "'") join(" ")
	)
	
	run := method(
		runWatingOnStdout
	)
	
	runWatingOnStdout := method(
		runWatingOnStream("stdout")
	)

	runWatingOnStderr := method(
		runWatingOnStream("stderr")
	)
	
	didRead := nil

	runWatingOnStream := method(streamName,
	    buffer := Sequence clone
		err := self asyncRun(command, arguments, environment)
		if(err == -1, Exception raise("unable to run command"))

		if(streamName == "stdout", stream := stdout)
		if(streamName == "stderr", stream := stderr)
		readEvent setDescriptorId(stream descriptorId)
	
		//writeln("runWatingOnStream(", streamName, ")")
		//writeln("SystemCall command: ", commandString)
		
		setIsRunning(true)
		loop(
//writeln("SystemCall loop 1---------------------------------------")
//writeln("status 11:", self status)
//writeln("status 22:", self status)
//writeln("status 33:", self status)
			readEvent waitOn(readTimeout) 
//writeln("status 1:", self status)
//writeln("status 2:", self status)
//writeln("status 3:", self status)
//writeln("SystemCall loop 2--------------------------------------- status ", self status)
			buffer appendSeq(stream readLines join("\n"))	
			didRead
			if(isRunning not, 
				//writeln("not running")
				break
			)
			if(self status > 255, 
				//writeln("status > 255")
				break
			)
			if(self status == -1, 
				//writeln("status == -1")
				break
			)
//writeln("SystemCall loop 3---------------------------------------")
		)

//writeln("buffer: '", buffer, "'")
		setIsRunning(false)
		setReturnCode(self status)
		//writeln("systemcall returning")
		buffer
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
