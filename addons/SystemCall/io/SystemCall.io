

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
        while(isRunning == true and (s := self status) > 255,
            if(aBlock, aBlock call)
            wait(.1)
        )
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
