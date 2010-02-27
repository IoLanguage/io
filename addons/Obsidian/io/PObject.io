//metadoc PObject category Databases
//metadoc Object category Databases

nil ppid := "nil"
true ppid := "true"
false ppid := "true"

List do(
	asSerialization := method(
	  map(ppid) join(",")
	)
	
	fromSerialization := method(serialization,
		copy(serialization split(",") map(ppid, pdb objectAtPpid(ppid)))
	)
)

Sequence do(
	asSerialization := method(self asSymbol)
	fromSerialization := method(serialization, serialization setPpid(ppid))
)

Number do(
	asSerialization := method(self asString)
	fromSerialization := method(serialization, serialization asNumber)
)

Object do(
	pdb ::= nil
	pdb = PDB
	ppid ::= nil

	/*doc Object shouldPersistByDefault
PDB boolean flag indicating whether this object should be considered for persistence
when persistence has not been specifically requested by calling ppid() or PDB addObjectToPersist.
Always false by default for Object.
	*/
	needsMetaPersist ::= false
	persistentSlots ::= nil
	shouldPersistByDefault := false
	
	/*doc Object pSlots
PDB extension to set a list of slots to persist with PDB.
Creates the specified slots using newSlot and sets them to nil.
	*/
	pSlots := method(
		self persistentSlots := call message arguments map(arg,
			if(hasSlot(arg name) not, newSlot(arg name, nil))
			arg name
		)
		shouldPersistByDefault = true
		self
	)
	
	/*doc Object ppid
This PDB extension returns a unique identifier for this object and registers it
for persistence with PDB.
	*/
	ppid := method(
		self needsMetaPersist := true
		self setPpid(PDB newId)
		PDB addObjectToPersist(self)
		self ppid
	)
	
	/*doc Object setPpid
PDB extension to set the value returned by ppid.
	*/
	setPpid := method(id,
		self ppid := id
		//writeln(getSlot("self") type, " setPpid")
		//if(getSlot("self") type == "List", Exception raise("List setPpid"))
		shouldPersistByDefault = true
		self
	)
	
	//doc Object persist Force immediate persistence of this object with PDB.
	persist := method(
		//writeln("Persisting #{self type}_#{self uniqueHexId}" interpolate)
		//if(persistentSlots == nil, return)
		if(needsMetaPersist, persistMetaData)
		self persistData 
		self persistSlots
		self
	)

	/*doc Object persistMetaData
Force immediate persistence of this object's type data into PDB
	*/
	persistMetaData := method(
		pdb onAtPut(ppid, "_type", self type)
		needsMetaPersist = false
		self
	)
	
	/*doc Object persistData
Force immediate persistence of this object's serialized form (using asSerialization)
into PDB, if possible. 
	*/
	persistData := method(
		if(self getSlot("asSerialization"),
			pdb onAtPut(ppid, "_data", asSerialization)
		)
		self
	)
	
	/*doc Object persistSlots
Force immediate persistence of this object's dirty slots into PDB.
	*/
	persistSlots := method(
		if(getSlot("self") type == "Block",
			Exception raise("attempt to persist a Block")
		)
		persistentSlots ?foreach(name,
			if(self hasDirtySlot(name),
				value := self getSlot(name)
				pdb onAtPut(ppid, name, value ppid)
			)
		)
		self
	)
	
	/*doc Object unpersist
PDB extension to populate this object with the data associated with this object's ppid from PDB.
	*/
	unpersist := method(
		//writeln(self type, " unpersist")
		obj := self
		if(self getSlot("fromSerialization"),
			obj = self fromSerialization(pdb onAt(ppid, "_data"))
		)
		
		pdb onFirst(ppid, 100) select(beginsWithSeq("_") not) foreach(key,
			value := pdb objectAtPpid(pdb onAt(ppid, key))
			obj setSlot(key, value)
		)

		obj
	)
	
	pdbMark := nil
)
