
Obsidian := Object clone do(
<<<<<<< HEAD:addons/Obsidian/io/Obsidian.io
	name ::= "test"
	sharedPrefixCursor ::= nil
=======
	path ::= nil
>>>>>>> 280aabf42b52afa2e38ff54c97471c3e448c417b:addons/Obsidian/io/Obsidian.io
	
	init := method(	
		self db := TokyoCabinet clone
	)
	
	open := method(
<<<<<<< HEAD:addons/Obsidian/io/Obsidian.io
		db open(name .. ".tc")
		setSharedPrefixCursor(db prefixCursor)
=======
		db open(path)
>>>>>>> 280aabf42b52afa2e38ff54c97471c3e448c417b:addons/Obsidian/io/Obsidian.io
	)
	
	close := method(
		self sharedPrefixCursor := nil
		db close
	)
	
	onAtPut := method(objId, slotName, value,
		db transactionalAtPut(objId .. "/" .. slotName, value)
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

	before := method(objId, slotName, count,
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
