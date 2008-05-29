PDB := Obsidian clone do(
	setPath("obsidian.tc")
	objectsToPersist ::= List clone
	ppidMap := Map clone
	_root := nil
	
	newId := method(
		UUID uuidTime asMutable replaceSeq("-", "") asSymbol
	)
	
	repoen := method(
		sync
		close
		open
	)
	
	sync := method(
		Collector collect
		Collector dirtyObjects foreach(obj,
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
		if(ppid == nil ppid, return(nil))
		obj := ppidMap at(ppid)
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
	
	show := method(
		writeln("PDB ", db path, ":")
		c := db cursor
		c first
		while(c key,
			k := c key
			id := k beforeSeq("/")
			slotName := k afterSeq("/")
			if(slotName beginsWithSeq("_"),
				writeln("  ", k, " = '", c value, "'")
			,
				writeln("  ", k, " = ", c value)
			)
			c next
		)
		c close
			
	)
	
	collectGarbage := method(
		//writeln("PDB collectGarbage:")
		// walk objects from root, recording ids found
		walked := Map clone
		toWalk := List clone append("root")
		c := self sharedPrefixCursor
		while(id := toWalk pop,
			walked atPut(id, true)
			c setPrefix(id)
			c first
			while(c key,
				k := c key
				if(k beginsWithSeq("_") == false and walked at(k) == nil,
					toWalk append(k)
				)
				c next
			)
		)

		// now remove all non-walked ids
		db begin
		c := db cursor
		c first
		while(k := c key,
			if(walked at(k beforeSeq("/")) == nil, c remove, c next)
		)
		c close
		db commit
	)	
)