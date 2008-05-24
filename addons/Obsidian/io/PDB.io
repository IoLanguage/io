PDB := Obsidian clone do(
	setPath("obsidian.tc")
	
	objectsToPersist ::= nil
	ppidMap := Map clone
	
	sync := method(
		setObjectsToPersist(Collector dirtyObjects)
		
		objectsToPersist foreach(obj,
			if(obj ppid == nil,
				obj setPdb(self) generatePid persistMetaData
			)

			obj persistData persistSlots
		)
		
		setObjectsToPersist(nil)

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
)