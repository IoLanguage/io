Importer := Object clone do(
    docDescription("A simple search path based auto-importer.")

    paths := List clone append("")
    debug := false
    
    addSearchPath    := method(p, paths appendIfAbsent(p asSymbol))
    removeSearchPath := method(p, paths remove(p asSymbol))
    
    extensions := list("io")

    executeString := method(name, s,
		Lobby doString(s, name)
		r := Lobby getSlot(name)
		if(r, return r)
		Exception raise("Importer slot '" .. name .. "' missing after file load")
		nil    
    )
    
	execute_io := method(path, name, extension,
		p := Path with(path, name .. "." .. extension)
		s := File clone setPath(p) contents
		executeString(name, s)
	)

    find := method(message, theObject,
        name := message name
                            
		p := ?launchPath
		if(debug, writeln("Importer find '", name, "' ", message label, " ", message lineNumber, "----"))
		if(p and(paths contains(p) not), paths atInsert(0, p))  
		
		// add relative path first
		//paths := paths clone atInsert(0, message label pathComponent)
		
		if(name at(0) isUppercase, 
            paths foreach(p, 
                extensions foreach(extension,
                    if(Path isPathAbsolute(p)) then(
                        fullPath := Path with(p, name .. "." .. extension) asSymbol
                    ) else(
                        fullPath := Path with(Directory currentWorkingDirectory, p, name .. "." .. extension) asSymbol
                    )
                    if(debug, 
                        writeln("cwd:                  ", Directory currentWorkingDirectory)
                        writeln("p:                    ", p)
                        writeln("Importer looking for: ", fullPath)
                    )
                    if(File clone setPath(fullPath) exists, 
                        r := self perform("execute_" .. extension, p, name, extension)
                        if (r, return r)
                    )
                )
			)
			
			if(AddonLoader hasAddonNamed(name),
                b := AddonLoader loadAddonNamed(name)
                if(b, return b)
                Exception raise("Importer slot '" .. name .. "' missing after dll load")
            )
		)
		
		Exception raise(theObject type .. " does not respond to '" .. name .. "'")
    )

    docSlot("turnOn", "Turns on the Importer. Returns self.")    
    turnOn := method(Lobby forward := method(Importer find(call message, self)); self)

    docSlot("turnOff", "Turns off the Importer. Returns self.")    
    turnOff := method(Lobby removeSlot("forward"); self)
    turnOn
)

//Collector collect; Collector collect; System symbols select(size >50) println
