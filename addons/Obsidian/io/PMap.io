PMap := Object clone do(
	// PMap will lazily load it's slots from the DB
	init := method(
		resend
		PDB addObjectToPersist(self)
		self needsMetaPersist = true
		self slotsToRemove := List clone
	)
	
	slotsToRemove := List clone
	shouldPersistByDefault := true
	
	atPut := method(slotName, value,
		self setSlot(slotName, getSlot("value"))
		self
	)
	
	at := method(slotName,
		if(slotsToRemove contains(slotName), return nil)
		id := pdb onAt(ppid, slotName)
		if(id == nil, return nil)
		if(slotName beginsWithSeq("_"), return id)
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
				slotsToRemove remove(name)
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
		slotsToRemove foreach(slotName, pdb onRemoveAt(ppid, slotName))
		slotsToRemove removeAll
		self
	)
	
	removeAt := method(slotName, 
		self removeSlot(slotName)
	)
	
	removeSlot := method(slotName,
		slotsToRemove appendIfAbsent(slotName)
		resend		
		self
	)

	firstCount  := method(count, 
		pdb onFirst(ppid, count)
	)
	
	objectsForKeys := method(keys,
		keys map(key, if(key beginsWithSeq("_"), key, pdb objectAtPpid(key)))
	)
	
	lastCount   := method(count, 
		pdb onLast(ppid, count)
	)
	
	afterCount  := method(key, count, 
		pdb onAfter(ppid, count)
	)
	
	beforeCount := method(key, count, 
		pdb onBefore(ppid, key, count)
	)
	
	slotCount := method(
		pdb sizeOn(ppid) 
	)
	
	createIfAbsent := method(slotName,
		if(self at(slotName) == nil, self atPut(slotName, PMap clone))
		self at(slotName)
	)
)
