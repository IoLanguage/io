
Obsidian := Object clone do(
	path ::= nil
	
	init := method(	
		self db := TokyoCabinet clone
	)
	
	open := method(
		db open(path)
	)
	
	close := method(
		db close
	)
	
	onAtPut := method(objId, slotName, value,
		db transactionalAtPut(objId .. "/" .. slotName, value)
	)
	
	onAt := method(objId, slotName,
		db at(objId .. "/" .. slotName)
	)
	
	cursorOn := method(objId,
		db prefixCursor setPrefix(objId)
	)
	
)
