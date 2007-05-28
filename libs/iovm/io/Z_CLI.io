CLI := Object clone do(
	newSlot("prompt", "Io> ")
	newSlot("outPrompt", "==> ")
	newSlot("continuedLinePrompt", "    ")

	newSlot("context", method(Lobby))
	newSlot("isRunning", true)

	newSlot("commandLineLabel", "Command Line")
	
	stop := method(setIsRunning(false))
	
	runFile := method(path,
		Lobby launchPath := if(Path isPathAbsolute(path),
			path
		,
			Directory currentWorkingDirectory asMutable appendPathSeq(path)
		) pathComponent

		System launchScript = path

		try(context doFile(path)) ?showStack
	)

	runIorc := method(
		home := System getenv("HOME")
		if(home,
			path := Path with(home, ".iorc")
			if(File with(path) exists,
				context doFile(path)
			)
		)
	)

	run := method(
		// Move Lobby launchPath to System launchPath?
		Lobby launchPath := Directory currentWorkingDirectory
		Importer addSearchPath(Lobby launchPath)
		context exit := method(System exit)

		runIorc

		if(?args first == "-e", 
			writeln(context doString(args slice(1) join(" ")))
			return
		)
        
		if(?args and args size > 0,
			
			if(args first == "-i",
				if(args size >= 2,
					runFile(args at(1))
				,
					if(File clone setPath("main.io") exists, runFile("main.io"))
				)
				return interactiveMultiline
			)
			
			runFile(args first)
		,
			if(File clone setPath("main.io") exists,
				runFile("main.io")
			,
				interactiveMultiline
			)
		)
	)
	
	interactiveMultiline := method(
		writeln("Io ", System version)
		while(isRunning,
			handleInteractiveMultiline
		)
	)
	
	interactive := method(
		writeln("Io ", System version)
		while(isRunning,
			handleInteractiveSingleLine
		)
	)

	writeCommandResult := method(result, 
		writeln(outPrompt, getSlot("result") asString)
	)
	
	handleInteractiveSingleLine := method(
		write(prompt)

		line := File standardInput readLine
		if(File standardInput isAtEnd,
			writeln
			context exit
		)

		e := try(result := context doMessage(line asMessage(commandLineLabel)))
		if(e,
			e showStack
		,
			writeCommandResult(getSlot("result"))
		)
	)

	errorMessage := method(error,
		error beforeSeq(" on line")
	)

	# Find error messages for the errors we understand
	lazySlot("knownErrors",
		m := Map clone
		m atPut(compileErrorMessage("("), ")-> ")
		m atPut(compileErrorMessage("\"\"\""), "\"-> ")
		m atPut(knownErrorMissingArgument, ")-> ")
	)

	lazySlot("knownErrorMissingArgument",
		compileErrorMessage("(x,")
	)

	compileErrorMessage := method(source,
		errorMessage(try(source asMessage) error)
	)

	handleInteractiveMultiline := method(
		# Start with the default prompt. The prompt is changed for continued lines, and errors.
		nextPrompt := prompt
		line := ""

		# If there are unmatched ( or the command ends with a \ then we'll need to read multiple lines
		loop(
			# Write out prompt. 
			write(nextPrompt)

			# Read line
			nextLine := File standardInput readLine
			
			# If there was no line, exit
			nextLine ifNil(context exit)

			# Add what we read to the line we've been building up
			line := line .. "\n" .. nextLine

			# If there is a \ on the end of the line, then keep building up the line
			if(line endsWithSeq("""\\"""),
				nextPrompt = continuedLinePrompt
				continue
			)

			compileError := try(lineAsMessage := line asMessage(commandLineLabel))
			if(compileError,
				if(nextLine size > 0,
					# If they're missing the end of the line, then let them finish it
					error := compileError error
					continuePrompt := knownErrors at(errorMessage(error))
					if(error == knownErrorMissingArgument and line asMutable strip endsWithSeq(",") not,
						continuePrompt = nil
					)
					if(continuePrompt,
						nextPrompt = continuePrompt
						continue
					)
				)

				# If the error can't be fixed by continuing the line, report the error.
				compileError showStack
				return
			)

			# Execute the line and report any exceptions which happen
			executionError := try(result := context doMessage(lineAsMessage, context))
			if(executionError,
				executionError showStack
				return
			,
				# Write out the command's result
				writeCommandResult(getSlot("result"))
				return
			)
		)
	)
)
