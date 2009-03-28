//metadoc Obsidian copyright Steve Dekorte 2008
//metadoc Obsidian license BSD revised
//metadoc Obsidian description A double key/value database with cursors on first key. Basis for PDB.
//metadoc Obsidian credits In collaboration with Rich Collins
//metadoc Obsidian category Databases

Obsidian := Object clone do(
	sharedPrefixCursor ::= nil
	path := nil
	
	setPath := method(p, 
		path = p
		self
	)
	
	init := method(	
		self db := TokyoCabinet clone setCompareType("path")	
	)
	
	open := method(
		db setPath(path) open
		setSharedPrefixCursor(db prefixCursor)
		self
	)
	
	//doc Obsidian close Closes the persistence database file.
	close := method(
		self sharedPrefixCursor := nil
		db close
		self
	)
	
	//doc Obsidian delete Removes the persistence database file set via setPath.
	delete := method(
		db close
		File with(path) remove
		self
	)
	
	onAtPut := method(objId, slotName, value,
		if(slotName size == 0, return nil)
		key := objId .. "/" .. slotName
		//writeln("db transactionalAtPut('", key, "', '", value, "')")
		db transactionalAtPut(key, value)
		//if(db at(key) != value, Exception raise(key .. " not saved"))
		nil
 	)
	
	//doc Obsidian onAt Return the value associated with slotName of stored object whose ppid is objId.
	onAt := method(objId, slotName,
		if(slotName size == 0, return nil)
		db at(objId .. "/" .. slotName)
	)
	
	onRemoveAt := method(objId, slotName,
		db transactionalRemoveAt(objId .. "/" .. slotName)
		nil
	)
	
	cursorOn := method(objId,
		db prefixCursor setPrefix(objId)
	)
	
	// network API
	
	acceptedMessageNames := list("onAtPut", "onAt", "onRemoveAt", "onFirst", "onLast", "onAfter", "onBefore")
	
	onFirst := method(objId, count,
		c := sharedPrefixCursor setPrefix(objId asString)
		c first
		keys := list()
		count asNumber repeat(
			k := c key
			if(k == nil, break)
			keys append(k)
			c next
		)
		keys
	)
				
	onLast := method(objId, count,
		c := sharedPrefixCursor setPrefix(objId)
		writeln(objId)
		c last
		writeln(c key)
		keys := list()
		arg1 asNumber repeat(
			k := c key
			if(k == nil, break)
			keys append(k)
			c previous
		)
		keys
	)
			
	onAfter := method(objId, slotName, count,
		c := sharedPrefixCursor setPrefix(objId)
		c jump(slotName)
		keys := list()
		count asNumber repeat(
			k := c key
			if(k == nil, break)
			keys append(k)
			c next
		)
		keys 
	)

	onBefore := method(objId, slotName, count,
		c := sharedPrefixCursor setPrefix(objId)
		c jump(slotName)
		keys := list()
		count asNumber repeat(
			k := c key
			if(k == nil, break)
			keys append(k)
			c previous
		)
		keys
	)	
	
	sizeOn := method(objId,
		c := sharedPrefixCursor setPrefix(objId)
		c jump(slotName)
		size := 1
		while (c key,
			size = size + 1
			if(k == nil, break)
			c next
		)
		size			
	)
)
