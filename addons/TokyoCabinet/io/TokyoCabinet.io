
TokyoCabinet do(
	//doc TokyoCabinet path Returns the path of the database file.
	//doc TokyoCabinet setPath(aPath) Sets the path of the database file. Returns self.
	path ::= "default.tc"
	
	//doc TokyoCabinet _open Private.
	_open := getSlot("open")
	open := method(p, opt,
		if(p, setPath(p))
		if(opt, 
			self _open(path, opt)
		, 
			self _open(path)
		)
		self
	)

	//doc TokyoCabinet transactionalAtPut(key, value) Transactionally insert the given key and value. Returns self.
	transactionalAtPut := method(k, v,
		self begin
		self atPut(k, v)
		self commit
		self
	)

	//doc TokyoCabinet transactionalRemoveAt(key) Transactionally remove the given key. Returns self.
	transactionalRemoveAt := method(k,
		self begin
		self removeAt(k)
		self commit
		self
	)
)

TokyoCabinetPrefixCursor prefix ::= nil