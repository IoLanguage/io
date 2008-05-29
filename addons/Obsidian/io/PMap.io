PMap := Object clone do(
	// PMap will lazily load it's slots from the DB
	init := method(
		resend
		PDB addObjectToPersist(self)
		self needsMetaPersist = true
		self slotsToRemove := List clone
	)
	
	shouldPersistByDefault := true
	
	at := method(slotName,
		id := pdb onAt(ppid, slotName)
		if(id == nil, return nil)
		obj := pdb objectAtPpid(id)
		self setSlot(slotName, obj)
		obj		
	)
	
	forward := method(
		self at(call message name)
	)
	
	hiddenSlots := list("ppid", "needsMetaPersist", "type", "shouldPersistByDefault", "hiddenSlots", "slotsToRemove")
	
	persistSlots := method(
		// persist all dirty slots
		self slotNames foreach(name,
			if(self getSlot(name) type != "Block" and self hasDirtySlot(name) and hiddenSlots contains(name) not,
				value := self getSlot(name)
				//writeln("PMap saving slot ", name)
				pdb onAtPut(ppid, name, value ppid)
				//value persist
			)
		)
		self
	)
	
	shouldPersistByDefault = true
	
	unpersist := method(
		//writeln("PMap unpersist")
		self
	)
	
	persist := method(
		//writeln("PMap persist")
		if(needsMetaPersist, persistMetaData)
		self persistSlots
		if(slotsToRemove, slotsToRemove foreach(slotName, pdb onRemoveAt(ppid, slotName)))
		self
	)
	
	removeSlot := method(slotName,
		slotsToRemove appendIfAbsent(slotName)
		resend		
	)
)