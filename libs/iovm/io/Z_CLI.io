
CLI := Object clone do(
	stdout := File clone standardOutput
	stdin  := File clone standardInput
	newSlot("inPrompt", "Io> ")
	newSlot("outPrompt", "==> ")
	multiLine := true
	newSlot("evalContext", method(Lobby))
	newSlot("isRunning", true)
	
	stop := method(setIsRunning(false))
	
	runPath := method(path,
		//Lobby launchPath := path pathComponent
		Lobby launchPath := if(Path isPathAbsolute(path) , path,
			Directory currentWorkingDirectory asMutable appendPathSeq(path)
		) pathComponent
		e := try(Lobby doFile(path))
		if(e, e showStack)
	)

	run := method(
		Lobby launchPath := Directory currentWorkingDirectory
        Lobby exit := method(System exit)

        if(?User,
            doString(File with(User homeDirectory path .. "/.iorc") contents)
        )
        
		if(?args and(args size > 0), 
			args foreach(i, arg,
			
				if(arg == "-e", 
					writeln(evalContext doString(args slice(i + 1) join(" ")))
					return
				)
				
				if(arg == "-i",
					if(args size == i + 1,
						if(File clone setPath("main.io") exists,
							runPath("main.io")
						)
					,
						nextArg := args at(i + 1) 
						if(nextArg, runPath(nextArg))
					)
					return interactiveMultiline
				)
				
				runPath(arg)
				return
			)
		,
			if(File clone setPath("main.io") exists,
				runPath("main.io")
				return
			)
		)

        interactiveMultiline
	)
	
	//outputEncoding ::= "utf8"
	
	outputResult := method(result, 
		s := getSlot("result") asString
		//if(s encoding != outputEncoding, s = s convertToEncoding(outputEncoding))
		writeln("\n", outPrompt, s)
	)
	
	interactive := method(
		writeln("Io ", System version)
 		while(isRunning,
			write(inPrompt)
			line := stdin readLine
			if(stdin isAtEnd, writeln; Lobby exit)
			result := nil
			e := try(result = evalContext doString(line))
			if(e, e showStack)
			outputResult(getSlot("result"))
		)
	)

	stripEnd := method(error,
		return error beforeSeq(" on line")
	)
	
	interactiveMultiline := method(
		writeln("Io ", System version)
		errTriQuote	:= stripEnd(try(Compiler messageForString("\"\"\"")) error)
		errParen := stripEnd(try(Compiler messageForString("(")) error)
		errArg := stripEnd(try(Compiler messageForString("(x,")) error)
		//errParen2	:= stripEnd(try(Compiler messageForString("1+(2+(")) error)
		mapContinueOnErr := Map clone
		mapContinueOnErr atPut(errTriQuote, "\"-> ")
		mapContinueOnErr atPut(errParen, ")-> ")
		//mapContinueOnErr atPut(errParen2, "))> ")

		while(isRunning,
			prompt := inPrompt
			line := ""
			loop(
				write(prompt)
				nextLine := stdin readLine
				nextLine ifNil(Lobby exit)
				line := line .. "\n" .. (nextLine)
				if(line endsWithSeq("""\\"""),
					prompt = "    "
					continue
				)
				result := nil
				e := try(result = evalContext doString(line))
				if(e,
					err := stripEnd(e error)
					if(prompt = mapContinueOnErr at(err),
						//err println
						continue
					)
					if(err ==(errArg) and(line asMutable strip endsWithSeq(",")),
						//err println
						prompt = "a-> "
						continue
					)
					e showStack
				)
				break
			)
			outputResult(getSlot("result"))
		)
	)
)

/*
Shell := Object clone do(
	newSlot("directory", Directory clone setPath("."))

    cd := method(dir,
        if(dir isKindOf(Sequence), 
            setCurrentWorkingDirectory(dir),
            if(dir isKindOf(Directory) not) then(
                Exception raise("cd requires a directory or a path name as its argument")
            ) else(setDirectory(dir))
        )
    )
    
	ls := method(directory items foreach(name println))

	open := method(name,
		item := directory at(call argAt(0) name)
		if(item isUserExecutable,
			// insert stuff to add args
			System system(item path)
		)	
	)
	
	forward := method(
		call delegateTo(directory)
		//item := directory at(call message name)
		//if(item == nil, writeln("no such path"); return item)
	)
)

Lobby shell := method(
	CLI setEvalContext(Shell)
	//CLI outputResult := method(nil)
)
*/

