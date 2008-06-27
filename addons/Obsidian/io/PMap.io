/*
PMap stores persistent data in a Map-like fashion and lazily loads
available slots from the PDB. Values stored or loaded are cached into the PMap's
local slots.
*/
PMap := Object clone do(
	init := method(
		resend
		PDB addObjectToPersist(self)
		self needsMetaPersist = true
		self slotsToRemove := List clone
	)
	
	slotsToRemove := List clone
	shouldPersistByDefault := true
	
	// atPut(slotName, value) records value in the slot named slotName.
	atPut := method(slotName, value,
		self setSlot(slotName, getSlot("value"))
		self
	)
	
	// at(slotName) tries to obtain a value for slot slotName from the local
	//slot, or tries to load it from the PDB if the local slot does not exist.
	//When all else fails, returns nil.
	at := method(slotName,
		if(slotsToRemove contains(slotName), return nil)
		if(hasSlot(slotName) and hiddenSlots contains(slotName) not, return self getSlot(slotName))
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
	
	// persistSlots() cleans up dirty slots by committing them to PDB.
	persistSlots := method(
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
	
	// Commits the PMap's slots to PDB.
	persist := method(
		//writeln("PMap persist")
		if(needsMetaPersist, persistMetaData)
		self persistSlots
		slotsToRemove foreach(slotName, pdb onRemoveAt(ppid, slotName))
		slotsToRemove removeAll
		self
	)
	
	// Marks a value for removal.
	removeAt := method(slotName, 
		self removeSlot(slotName)
	)
	
	// Marks a value for removal.
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
	
	// Creates a slot with a new PMap clone if not already present.
	createIfAbsent := method(slotName,
		if(self at(slotName) == nil, self atPut(slotName, PMap clone))
		self at(slotName)
	)
)
