
TokyoCabinet do(
	newSlot("path", "default.tc")

	_open := getSlot("open")
	open := method(p, opt,
		if(p, setPath(p))
		if(opt, _open(path, opt), _open(path))
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
)

