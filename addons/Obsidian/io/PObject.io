Object do(
	pdb ::= nil
	ppid ::= nil
	needsMetaPersist ::= false
	
	ppid := method(
		PDB addObjectToPersist(self)
		self needsMetaPersist := true
		self ppid := UUID uuidTime
	)
	
	persist := method(
		if(needsFirstPersist, persistMetaData)
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
		pdb onAtPut(ppid, "_data", asSerialization)
		self
	)
	
	persistSlots := method(		
		persistentSlots foreach(name,
			self hasDirtySlot(name) ifTrue(
				value = self getSlot(name)
				pdb onAtPut(ppid, slotName, value ppid)
			)
		)
		self
	)
	
	unpersist := method(
		pdb onFirst(ppid, 100) select(beginsWithSeq("_") not) foreach(key,
			value := pdb objectAtPpid(pdb onAt(ppid, key))
			setSlot(key, value)
		)
	)
)