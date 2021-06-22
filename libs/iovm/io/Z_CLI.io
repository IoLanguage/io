Locals removeSlot("doFile")

DummyLine := File standardInput do(
    readLine := method(prompt,
        if(prompt, prompt print)
        resend
    )
)

CLI := Object clone do(
    prompt ::= "Io> "
    outPrompt ::= "==> "
    continuedLinePrompt ::= "... "

    isRunning ::= true # Get rid of this ...
    commandLineLabel ::= "Command Line" # and this?

    context ::= lazySlot(
        Lobby do(
            # Python-style underscore, stores the result of the previous computation.
            # Example:
            # Io> 1 + 1
            # ==> 2
            # Io> _ == 2
            # ==> true
            _ ::= nil

            exit := method(CLI stop)
        )
    )

    lineReader := lazySlot(
        # This might look as a `hack`, but why not use stdin as the default
        # reader, since it shares the same interface with Read(Edit)Line,
        # i.e. <reader> readLine.
        reader := DummyLine

        # Trying to use GNU ReadLine as the default line reader, falling
        # back to EditLine, if the attempt failed.
        try(reader := ReadLine) catch(Exception,
            try(reader := EditLine)
        )
        reader
    )

    # A list of error messages for the errors we understand.
    knownErrors := lazySlot(
        list("(", "[", "{", "\"\"\"", "(x,") map(error,
            self errorMessage(try(error asMessage) error)
        )
    )

    errorMessage := method(error, error beforeSeq(" on line"))

    doFile := method(path,
        System launchPath = if(Path isPathAbsolute(path),
            path
        ,
            System launchPath asMutable appendPathSeq(path)
        ) pathComponent

        System launchScript = path

        context doFile(path)
    )

    doLine := method(lineAsMessage,
        # Execute the line and report any exceptions which happened.
        executionError := try(result := context doMessage(lineAsMessage))
        if(executionError,
            executionError showStack
        ,
            # Write out the command's result to stdout; nothing is written
            # if the CLI is terminated, this condition is satisfied, only
            # when CLI exit() was called.
            if(isRunning,
                context set_(getSlot("result"))
                writeCommandResult(getSlot("result")))
        )
    )

    doIorc := method(
        # Note: Probably won't work on Windows, since it uses %HOMEPATH%
        # and %HOMEDRIVE% pair to indentify user's home directory.
        home := System getEnvironmentVariable("HOME")
        if(home,
            path := Path with(home, ".iorc")
            if(File with(path) exists,
                context doFile(path)
            )
        )
    )

    ioHistoryFile := lazySlot(
        Path with(System getEnvironmentVariable("HOME"), ".io_history")
    )

    saveHistory := method(lineReader ?saveHistory(ioHistoryFile))
    loadHistory := method(
        if(File with(ioHistoryFile) exists,
            lineReader ?loadHistory(ioHistoryFile)
        )
    )


    writeWelcomeBanner := method("Io #{System version}" interpolate println)
    writeCommandResult := method(result,
        outPrompt print

        if(exc := try(getSlot("result") asString println),
            "<exception while dislaying result>" println
            exc showStack
        )
    )

    stop := method(setIsRunning(false))
    run  := method(
        Importer addSearchPath(
            System launchPath := Directory currentWorkingDirectory
        )

        doIorc

        # Note: GetOpt should be used there, since System getOptions
        # is completely useless.
        if("-h" in(System args) and System args size == 1, help)
        if("--version" in (System args) and System args size == 1, version) # hm...

        if(System args first == "-e") then(
            return context doString(
                System args slice(1) map(asUTF8) join(" ")
            )
        ) elseif(System args first == "-i" and System args size >= 2) then(
            # Note: when given an -i option, all the following arguments
            # should be filenames to be loaded into the REPL.
            System args rest foreach(arg,
                doFile(arg)
            )
        ) elseif(System args size > 0,
            return doFile(System args first)
        )

        # Is this still needed? Not used anywhere in the source.
        if(File clone setPath("main.io") exists,
            doFile("main.io")
        )

        loadHistory
        writeWelcomeBanner
        interactive
        saveHistory
    )

    interactive := method(
        # Start with the default prompt. The prompt is changed for continued lines,
        # and errors.
        prompt := self prompt
        line := ""

        # If there are unmatched (, {, [ or the command ends with a \ then we'll
        # need to read multiple lines.
        loop(
            # Write out prompt and read line.
            if(nextLine := lineReader readLine(prompt),
                # Add what we read to the line we've been building up
                line = line .. nextLine
            ,
                # Note: readLine method returns nil if ^D was pressed.
                context exit
                "\n" print # Fixing the newline issue.
            )

            compileError := try(
                lineAsMessage := line asMessage setLabel(commandLineLabel)
            )

            if(compileError,
                # Not sure that, displaying a different notification for
                # each error actually makes sense.
                if(nextLine size > 0 and errorMessage(compileError error) in(knownErrors),
                    prompt = continuedLinePrompt
                    continue
                )
                # If the error can't be fixed by continuing the line - report it.
                compileError showStack
            ,
                doLine(lineAsMessage)
            )

            lineReader ?addHistory(line)
            return if(isRunning, interactive, nil)
        )
    )

    version := inlineMethod(
        "Io Programming Language, v. #{System version}" interpolate println;
        System exit
    )

    help := inlineMethod(
"""
usage: io [-h | -e expr | -i file.io, file.io, ...| file.io arg, arg, ... | --version]

options:
  --version   print the version of the interpreter and exit
  -h          print this help message and exit
  -e          eval a given expression and exit
  -i          run the interpreter, after processing the files passed

""" println
        System exit
    )
)
