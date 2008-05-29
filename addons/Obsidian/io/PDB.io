PDB := Obsidian clone do(
	setPath("obsidian.tc")
	objectsToPersist ::= List clone
	ppidMap := Map clone
	_root := nil
	
	newId := method(
		UUID uuidTime asMutable replaceSeq("-", "") asSymbol
	)
	
	sync := method(
		Collector collect
		Collector dirtyObjects foreach(obj,
			//writeln("obj ", getSlot("obj") uniqueId)
			//writeln("obj ", getSlot("obj") type)
			if(getSlot("obj") shouldPersistByDefault == true, 
				//writeln(getSlot("obj") type, "_", getSlot("obj") uniqueId, " shouldPersistByDefault ")
				objectsToPersist appendIfAbsent(getSlot("obj"))
			)
		)
		objectsToPersist foreach(persist)
		objectsToPersist removeAll
		Collector cleanAllObjects
		self
	)
	
	objectAtPpid := method(ppid,
		//if(ppid == nil, return(nil))
		if(ppid == nil ppid, return(nil))
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
	
	collectGarbage := method(
		// walk objects from root, recording ids found
		walked := Map clone
		toWalk := List clone append("root")
		c := self sharedPrefixCursor
		while(id := toWalk pop,
			walked atPut(id, true)
			c setPrefix(id)
			c first
			while(c next,
				k := c key
				if(k beginsWithSeq("_") == false and walked at(k) == nil,
					toWalk append(k)
				)
			)
		)
		
		// now remove all non-walked ids
		db begin
		c := db cursor
		c first
		while(c next,
			k := c key
			id := k beforeSeq("/")
			if(walked at(id) == nil, db removeAt(k))
		)
		c close
		db commit
	)	
)