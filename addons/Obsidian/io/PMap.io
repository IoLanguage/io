PMap := Object clone do(
	// PMap will lazily load it's slots from the DB
	init := method(
		resend
		PDB addObjectToPersist(self)
	)
	
	forward := method(
		writeln("PMap forward ", call message name)
		slotName := call message name
		id := pdb onAt(ppid, slotName)
		if(id == nil, return nil)
		obj := pdb objectAtPpid(id)
		self setSlot(slotName, obj)
		obj
	)
	
	persistSlots := method(
		// persist all dirty slots
		self slotNames foreach(name,
			if(self hasDirtySlot(name),
				value := self getSlot(name)
				if(getSlot(name) type != "Block",
					pdb onAtPut(ppid, name, value ppid)
				)
			)
		)
		self
	)
	
	shouldPersist := true
	
	unpersist := method(
		self
	)
	
	persist := method(
		writeln("PMap persist")
		if(needsMetaPersist, persistMetaData)
		self persistSlots
		self
	)
)