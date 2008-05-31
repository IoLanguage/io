
//metadoc Obsidian copyright Steve Dekorte 2008
//metadoc Obsidian license BSD revised
//metadoc Obsidian description A double key/value database with cursors on first key. Basis for PDB.
//metadoc Obsidian credits In collaboration with Rich Collins
//metadoc Obsidian category Databases

Obsidian := Object clone do(
	name ::= "unnamed"
	sharedPrefixCursor ::= nil
	path ::= nil
	
	init := method(	
		self db := TokyoCabinet clone
	)
	
	open := method(
		db open(path)
		setSharedPrefixCursor(db prefixCursor)
		self
	)
	
	close := method(
		self sharedPrefixCursor := nil
		db close
		self
	)
	
	delete := method(
		db close
		File with(path) remove
		self
	)
	
	onAtPut := method(objId, slotName, value,
		key := objId .. "/" .. slotName
		//writeln("db transactionalAtPut('", key, "', '", value, "')")
		db transactionalAtPut(key, value)
		//if(db at(key) != value, Exception raise(key .. " not saved"))
 	)
	
	onAt := method(objId, slotName,
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
		c last
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
		c goto(slotName)
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
		c goto(slotName)
		keys := list()
		count asNumber repeat(
			k := c key
			if(k == nil, break)
			keys append(k)
			c previous
		)
		keys			
	)	
)
