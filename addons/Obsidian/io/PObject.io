nil ppid := "nil"

Sequence do(
	asSerialization := method(self asSymbol)
	fromSerialization := method(serialization, copy(serialization))
)

Object do(
	pdb ::= nil
	pdb = PDB
	ppid ::= nil
	needsMetaPersist ::= false
	persistentSlots ::= nil
	
	pSlots := method(
		self persistentSlots :=  call message arguments map(name)
		self
	)
	
	ppid := method(
		PDB addObjectToPersist(self)
		self needsMetaPersist := true
		self ppid := UUID uuidTime
		self ppid
	)
	
	persist := method(
		if(persistentSlots == nil, return)
		if(needsMetaPersist, persistMetaData)
		self persistData 
		self persistSlots
		self
	)

	persistMetaData := method(
		pdb onAtPut(ppid, "_type", self type)
		needsMetaPersist = false
		self
	)
	
	persistData := method(
		if(self getSlot("asSerialization"),
			pdb onAtPut(ppid, "_data", asSerialization)
		)
		self
	)
	
	persistSlots := method(
		persistentSlots foreach(name,
			if(self hasDirtySlot(name),
				value := self getSlot(name)
				pdb onAtPut(ppid, name, value ppid)
			)
		)
		self
	)
	
	unpersist := method(
		pdb onFirst(ppid, 100) select(beginsWithSeq("_") not) foreach(key,
			value := pdb objectAtPpid(pdb onAt(ppid, key))
			self setSlot(key, value)
		)
		if(self getSlot("fromSerialization"),
			fromSerialization(pdb onAt(ppid, "_data"))
		)
		self
	)
)