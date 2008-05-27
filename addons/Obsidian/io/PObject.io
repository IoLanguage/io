nil ppid := "This is my ppid. There are many like it, but this one is mine.  My ppid is my best friend. It is my life. I must master it as I must master my life.  My ppid, without me, is useless."

Object do(
	aliasMethodChain := method(oldName, suffix,
		self setSlot(oldName .. "Without" .. suffix asCapitalized, self getSlot(oldName))
		self setSlot(oldName, self getSlot(oldName .. "With" .. suffix asCapitalized))
		self
	)
)

Sequence do(
	asSerialization := method(self asSymbol)
	fromSerialization := method(serialization, copy(serialization))
)

Object do(
	pdb ::= nil
	ppid ::= nil
	needsFirstPersist ::= false
	
	withPpid := method(
		o := self clone
		o ppid
		o
	)
	
	pSlots := method(
		if(self getSlot("persistentSlots") not, self persistentSlots := List clone)
		m := call message arguments first
		while(m,
			slotName := m name
			self persistentSlots appendIfAbsent(slotName)
			self newSlot(slotName, nil)
			m := m next
		)
		self
	)
	
	ppid := method(
		pdb addObjectToPersist(self)
		watchPersistentSlots
		self needsFirstPersist := true
		self ppid := UUID uuidTime
		self ppid
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
		if(self getSlot("asSerialization"),
			pdb onAtPut(ppid, "_data", asSerialization)
		)
		self
	)
	
	setSlotWithPersistentSlotWatching := method(name, value,
		?persistentSlots contains(name) ifTrue(
			dirtyPersistentSlots appendIfAbsent(name)
		)
		setSlotWithoutPersistentSlotWatching(name, value)
	)
	
	updateSlotWithPersistentSlotWatching := method(name, value,
		?persistentSlots contains(name) ifTrue(
			dirtyPersistentSlots appendIfAbsent(name)
		)
		updateSlotWithoutPersistentSlotWatching(name, value)
	)
	
	watchPersistentSlots := method(
		self dirtyPersistentSlots := List clone
		self aliasMethodChain("updateSlot", "persistentSlotWatching")
		self aliasMethodChain("setSlot", "persistentSlotWatching")
		self
	)
	
	persistSlots := method(
		dirtyPersistentSlots foreach(name,
			value := self getSlot(name)
			pdb onAtPut(ppid, name, value ppid)
		)
		dirtyPersistentSlots removeAll
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
		watchPersistentSlots
		self
	)
)