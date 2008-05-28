PDB := Obsidian clone do(
	setPath("obsidian.tc")
	objectsToPersist ::= List clone
	ppidMap := Map clone
	
	sync := method(
		//objectsToPersist appendSeq(Collector dirtyObjects select(shouldPersist))
		objectsToPersist foreach(persist)
		objectsToPersist removeAll
		self
	)
	
	objectAtPpid := method(ppid,
		if(ppid == nil ppid, return(nil))
		obj := ppidMap at(ppid)
		if(obj, return obj)
		objType := self onAt(ppid, "_type")
		if(objType == nil, return nil)
		obj := Lobby getSlot(objType) clone setPpid(ppid)
		ppidMap atPut(ppid, obj)
		obj unpersist
	)
	
	addObjectToPersist := method(o, 
		objectsToPersist appendIfAbsent(o)
	)
	
	close := method(
		resend
		self root := getSlot("_root")
		self
	)
	
	_root := method(
		obj := self objectAtPpid("root")
		if(obj == nil, obj := PMap clone setPpid("root"))
		self root := obj
		obj
	)
	
	root := getSlot("_root")
)