PMap := Object clone do(
	forward := method(
		slotName := call message name
		id := pdb onAt(ppid, slotName)
		if(id == nil, return nil)
		obj := pdb objectAtPpid(id)
		self _setSlot(slotName, obj) 
		obj
	)
	
	add := method(pObj,
		atPut(pObj ppid, pObj)
		self
	)
	
	at := method(ppid,
		self getSlot(ppid)
	)
	
	atPut := method(ppid, value,
		dirtyPersistentSlots appendIfAbsent(ppid)
		self setSlot(ppid, value)
		self
	)
	
	unpersist := method(
		watchPersistentSlots
		self
	)
)