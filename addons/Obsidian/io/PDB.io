PDB := Obsidian clone do(
	setPath("obsidian.tc")
	objectsToPersist ::= List clone
	ppidMap := Map clone
	_root := nil
	
	newId := method(
		UUID uuidTime asMutable replaceSeq("-", "") asSymbol
	)
	
	sync := method(
		// objectsToPersist appendSeq(Collector dirtyObjects select(shouldPersist))
		objectsToPersist foreach(persist)
		objectsToPersist removeAll
		self
	)
	
	objectAtPpid := method(ppid,
		//if(ppid == nil, return(nil))
		//if(ppid == nil ppid, return(nil))
		obj := ppidMap at(ppid)
		//writeln("obj = ", obj)
		if(obj, return obj)
		objType := self onAt(ppid, "_type")
		//writeln(objType, " = db onAt('", ppid, "/_type)")
		if(objType == nil, return nil)
		obj := Lobby getSlot(objType) clone setPpid(ppid)
		ppidMap atPut(ppid, obj)
		obj unpersist
	)
	
	addObjectToPersist := method(o, 
		objectsToPersist appendIfAbsent(o)
	)
	
	close := method(
		resend
		_root = nil
		ppidMap = Map clone
		self
	)
	
	root := method(
		if(_root, return _root)
		_root = self objectAtPpid("root")
		if(_root == nil, _root = PMap clone setPpid("root"))
		_root
	)
	
)