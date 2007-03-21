
QDBM do(
	newSlot("path", "default.qdbm")
	
	_open := getSlot("open")
	open := method(p,
		if(p, setPath(p))
		_open(path)
	)
	
	transactionalAtPut := method(k, v,
		self begin
		self atPut(k, v)
		self commit
	)
	
	transactionalRemoveAt := method(k,
		self begin
		self removeAt(k)
		self commit
	)
	
	cursor := method(QDBMCursor clone setDb(self) syncKey)
)


QDBMCursor := Object clone do(
	newSlot("db")
	newSlot("key")
	
	syncKey := method(
		setKey(db cursorKey)
		self
	)
	
	syncDB := method(
		if(db cursorKey != key, db cursorFirst; db cursorJumpForward(key))
		self
	)
	
	first := method(db cursorFirst; syncKey)
	last  := method(db cursorLast; syncKey)
	
	next     := method(syncDB; db cursorNext; syncKey)
	previous := method(syncDB; db cursorPrevious; syncKey)

	jumpForward  := method(k, syncDB; db cursorJumpForward(k); syncKey)
	jumpBackward := method(k, syncDB; db cursorJumpBackward(k); syncKey)
		
	value := method(syncDB; db cursorValue)
	
	put    := method(v, syncDB; db cursorPut(v); self)
	remove := method(v, syncDB; db cursorRemove; self)
	
	transactionalPut    := method(v, syncDB; db begin; db cursorPut(v); db commit; self)
	transactionalRemove := method(v, syncDB; db begin; db cursorRemove; db commit; self)	
)
