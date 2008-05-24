PMap := Object clone do(	
	_setSlot := getSlot("setSlot")
	
	forward := method(
		slotName := call message name
		id := pdb onAt(ppid, slotName)
		if(id == nil, return nil)
		obj := pdb objectAtPpid(id)
		self _setSlot(slotName, obj) 
		obj
	)
	
	setSlot := method(slotName, value,
		pdb atPut(slotName, getSlot("value") ppid)
		self _setSlot(slotName, getSlot("value"))
	)
	
	unpersist := method(
		self
	)
)