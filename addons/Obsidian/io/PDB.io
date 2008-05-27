PDB := Obsidian clone do(
	setPath("obsidian.tc")
	
	objectsToPersist ::= List clone
	ppidMap := Map clone
	
	sync := method(
		Collector dirtyObjects select(getSlot("ppid")) foreach(o, objectsToPersist appendIfAbsent(o))
		objectsToPersist foreach(persist)
		objectsToPersist removeAll
		Collector cleanAllObjects
		self
	)
	
	objectAtPpid := method(ppid,
		if(ppid == nil ppid, return(nil))
		obj := ppidMap at(ppid)
		if(obj, return obj)
		objType := self onAt(ppid, "_type")
		obj := self getSlot(objType) clone setPpid(ppid)
		ppidMap atPut(ppid, obj)
		obj unpersist
	)
	
	emptyPpidMap := method(
		ppidMap empty
		self
	)
	
	addObjectToPersist := method(o, 
		objectsToPersist appendIfAbsent(o)
	)
)