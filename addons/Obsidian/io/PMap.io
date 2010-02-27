//metadoc PMap category Databases
/*metadoc PMap description
PMap stores persistent data in a Map-like fashion and lazily loads
available slots from the PDB backing store. Values stored or loaded are cached
into local object slots.
*/
PMap := Object clone do(
	//doc PMap init Initialize a new PMap and automatically add it as a persistent object to PDB.
	init := method(
		resend
		PDB addObjectToPersist(self)
		self needsMetaPersist = true
		self slotsToRemove := List clone
		//super(removeSlot("ppid"))		
	)
	
	slotsToRemove := List clone
	/*doc PMap shouldPersistByDefault
PDB boolean flag indicating whether this object should be considered for persistence
when persistence has not been specifically requested by calling ppid() or PDB addObjectToPersist.
Always true by default for PMap.
	*/
	shouldPersistByDefault := true
	
	//doc PMap atPut(slotName, value) Records value in the slot named slotName.
	atPut := method(slotName, value,
		self setSlot(slotName, getSlot("value"))
		self
	)
	
	/*doc PMap at(slotName)
Tries to obtain a value for slot slotName from a local slot of that name,
or tries to load it from the PDB if the local slot does not exist. When all else fails, returns nil.
<p>
If slotName begins with an underscore ("_"), returns the id of the slot from PDB instead of the value.
	*/
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
	
	hiddenSlots := list("", "ppid", "needsMetaPersist", "type", "shouldPersistByDefault", "hiddenSlots", "slotsToRemove")
	
	//doc PMap persistSlots Cleans up dirty slots by committing them to PDB.
	persistSlots := method(
		//writeln("PMap persistSlots")
		self slotNames foreach(name,
			//writeln("self hasDirtySlot(", name, ")", self hasDirtySlot(name))
			if(self getSlot(name) type != "Block" and self hasDirtySlot(name) and hiddenSlots contains(name) not,
				//writeln("PMap persisting slot ", name)
				value := self getSlot(name)
				//writeln("PMap saving slot ", name)
				slotsToRemove remove(name)
				pdb onAtPut(ppid, name, value ppid)
				//value persist
			)
		)
		self
	)
	
	unpersist := method(
		//writeln("PMap unpersist")
		self
	)
	
	//doc PMap persist Commits the PMap's slots to PDB.
	persist := method(
		//writeln("PMap persist")
		if(needsMetaPersist, persistMetaData)
		self persistSlots
		slotsToRemove foreach(slotName, pdb onRemoveAt(ppid, slotName))
		slotsToRemove removeAll
		self
	)
	
	//doc PMap removeAt Marks a value for removal.
	removeAt := method(slotName, 
		self removeSlot(slotName)
	)
	
	//doc PMap removeSlot Marks a value for removal.
	removeSlot := method(slotName,
		slotsToRemove appendIfAbsent(slotName)
		resend		
		self
	)

	firstCount  := method(count, 
		pdb onFirst(ppid, count)
	)
	
	/*doc PMap objectsForKeys
Returns a list of values for each key in the list given. Keys beginning with an
underscore ("_") are returned verbatim.
	*/
	objectsForKeys := method(keys,
		keys map(key, if(key beginsWithSeq("_"), key, pdb objectAtPpid(key)))
	)
	
	lastCount   := method(count, 
		pdb onLast(ppid, count)
	)
	
	afterCount  := method(key, count, 
		pdb onAfter(ppid, key, count)
	)
	
	beforeCount := method(key, count, 
		pdb onBefore(ppid, key, count)
	)
	
	//doc PMap slotCount Returns the number of slots committed to PDB.
	slotCount := method(
		pdb sizeOn(ppid) 
	)
	
	//doc PMap createIfAbsent Creates a slot with a new PMap clone if not already present.
	createIfAbsent := method(slotName,
		if(self at(slotName) == nil, self atPut(slotName, PMap clone))
		self at(slotName)
	)
)
