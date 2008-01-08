
Image do(
	newSlot("path")

	_open := getSlot("open")
	open := method(p,
		if(p, setPath(p))
		_open(path)
	)
)
