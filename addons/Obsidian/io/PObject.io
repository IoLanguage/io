Object do(
	pdb ::= nil
	ppid ::= nil
		
	generatePpid := method(
		setPpid(UUID uuidTime)
	)
	
	persistData := method(
		pdb onAtPut(ppid, "_data", asSerialization)
		self
	)
	
	persistMetaData := method(
		pdb onAtPut(ppid, "_type", self type)
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