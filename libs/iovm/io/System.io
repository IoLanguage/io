System do(
	args := list()
	
	launchScript := nil
	//doc System launchScript Returns the path of the io file run on the command line. Returns nil if no file was run.

	ioPath := installLibDir asMutable appendPathSeq("io")
	//doc System ioPath Returns the path of io installation. The default is $INSTALL_PREFIX/lib/io.

	/*doc System getOptions(args) 
	This primitive is used to get command line options similar to Cs getopt().
	It returns a map in containing the left side of the argument, with the
	value of the right side. (The key will not contain
	the beginning dashes (--).
	<p>
	Example:
	<pre>
	options := System getOptions(args)
	options foreach(k, v,
	  if(v type == List type,
		v foreach(i, j, writeln(\"Got unnamed argument with value: \" .. j))
		continue
	  )
	  writeln(\"Got option: \" .. k .. \" with value: \" .. v)
	)
	</pre>
	*/

	getOptions := method(arguments,
		opts := Map clone
		optname := Sequence clone
		optvalue := Sequence clone
		optsNoKey := List clone

		arguments foreach(i, arg,
			if(arg beginsWithSeq("--") isNil,
				optsNoKey append(arg)
				continue
			)

			if(arg containsSeq("=")) then(
				optname := arg clone asMutable
				optname clipAfterStartOfSeq("=")
				optname clipBeforeEndOfSeq("--")
				optvalue := arg clone asMutable
				optvalue clipBeforeEndOfSeq("=")
			) else(
				optname := arg clone asMutable
				optname clipBeforeEndOfSeq("--")
				optvalue = ""
			)
			opts atPut(optname, optvalue)
		)

		if(optsNoKey last != nil, opts atPut("", optsNoKey))
		opts
	)

	//doc System userInterruptHandler Called when control-c is hit. Override to add custom behavior. Returns self.
	userInterruptHandler := method(
		writeln("\n  current coroutine")
		Scheduler currentCoroutine showStack
		Scheduler yieldingCoros foreach(coro,
			writeln("  coroutine ", coro label)
			coro showStack
		)
		self exit
	)
	
	//doc System runCommand Calls system and redirects stdout/err to tmp files.  Returns object with exitStatus, stdout and stderr slots.
	runCommand := method(cmd, successStatus,
		successStatus := if(successStatus, successStatus, 0)
		tmpDirPath := System getEnvironmentVariable("TMPDIR")
		outPath := method(suffix,
			Path with(tmpDirPath, list(System thisProcessPid, Date clone now asNumber, suffix) join("-"))
		)
		stdoutPath := outPath("stdout")
		stderrPath := outPath("stderr")
		exitStatus := System system(cmd .. " > " .. stdoutPath .. " 2> " .. stderrPath)
		result := Object clone
		result successStatus := successStatus
		result exitStatus := exitStatus
		result failed := method(exitStatus != successStatus)
		result succeeded := method(exitStatus == successStatus)
		result stdout := File with(stdoutPath) contents
		result stderr := File with(stderrPath) contents
		result
	)
)
