Object do(
	pdb ::= nil
	ppid ::= nil
	needsFirstPersist ::= false
	
	ppid := method(
		PDB addObjectToPersist(self)
		self needsFirstPersist := true
		self ppid := UUID uuidTime
	)
	
	persist := method(
		if(needsFirstPersist, needsFirstPersist = false; persistMetaData)
		self persistData 
		self persistSlots
		self
	)
	
	persistMetaData := method(
		pdb onAtPut(ppid, "_type", self type)
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
				if(value ppid == nil,
					value generatePid
					pdb objectsToPersist appendIfAbsent(value)
				)
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