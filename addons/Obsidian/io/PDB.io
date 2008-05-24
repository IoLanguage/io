PDB := Obsidian clone do(
	setPath("obsidian.tc")
	
	objectsToPersist ::= List clone
	ppidMap := Map clone
	
	sync := method(
		Collector dirtyObjects foreach(o, objectsToPersist appendIfAbsent(o))
		objectsToPersist foreach(persist) removeAll
		self
	)
	
	objectAtPpid := method(ppid,
		obj := ppidMap at(ppid)
		if(obj, return obj)
		objType := self onAt(ppid, "_type")
		obj := getSlot(objType) clone setPpid(ppid)
		ppidMap atPut(ppid, obj)
		obj unpersist
	)
	
	addObjectToPersist := method(o, 
		objectsToPersist appendIfAbsent(o)
	)
)