PMap := Object clone do(
	// PMap will lazily load it's slots from the DB
	init := method(
		resend
		PDB addObjectToPersist(self)
		self needsMetaPersist = true
	)
	
	forward := method(
		//writeln("PMap forward ", call message name)
		slotName := call message name
		id := pdb onAt(ppid, slotName)
		if(id == nil, return nil)
		obj := pdb objectAtPpid(id)
		self setSlot(slotName, obj)
		obj
	)
	
	hiddenSlots := list("ppid", "needsMetaPersist")
	persistSlots := method(
		// persist all dirty slots
		self slotNames foreach(name,
			if(self hasDirtySlot(name) and hiddenSlots contains(name) not,
				value := self getSlot(name)
				if(getSlot(name) type != "Block",
					pdb onAtPut(ppid, name, value ppid)
					value persist
				)
			)
		)
		self
	)
	
	shouldPersist := true
	
	unpersist := method(
		//writeln("PMap unpersist")
		self
	)
	
	persist := method(
		//writeln("PMap persist")
		if(needsMetaPersist, persistMetaData)
		self persistSlots
		self
	)
)