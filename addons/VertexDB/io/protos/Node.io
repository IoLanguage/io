VertexDB Node := Object clone do(
	//internal
	request := method(
		Request clone setRaisesOnError(raisesOnError) setHost(host) setPort(port) setPath(path)
	)
	
	//api
	host ::= method(Settings host)
	port ::= method(Settings port)
	
	path ::= "/"
	
	raisesOnError ::= method(Settings raisesOnError)
	
	with := method(path,
		self clone setPath(path)
	)
	
	nodeAt := method(name,
		Node clone setPath(Path with(path, name))
	)
	
	asQueue := method(
		Queue with(self)
	)
	
	// reads
	//query
	query := method(
		Query clone setNode(self)
	)
	
	keys := method(
		query keys
	)
	
	values := method(
		query values
	)
	
	pairs := method(
		query pairs
	)
	
	counts := method(
		query counts
	)
	
	object := method(
		query object
	)
	
	rm := method(
		query rm
	)
	
	//read
	readRequest := method(
		request setAction("read") addQuerySlots("key")
	)
	
	read := method(key,
		readRequest setKey(key) results
	)
	
	sizeRequest := method(
		request setAction("size")
	)
	
	size := method(
		sizeRequest results
	)
	
	//
	
	//write
	//remove
	rmRequest := method(key,
		request setAction("rm") addQuerySlots("key") setHttpMethod("post") setKey(key)
	)
	
	rm := method(key,
		rmRequest(key) results
		self
	)
	
	//insert
	atWriteRequest := method(key, data,
		request setAction("write")\
			addQuerySlots("key")\
			setHttpMethod("post")\
			setKey(key)\
			setBody(data)
	)
	
	atWrite := method(key, data,
		Transaction current appendRequest(atWriteRequest(key, data))
		self
	)
	
	mkdirRequest := method(
		request setAction("mkdir") setHttpMethod("post")
	)
	
	mkdir := method(
		Transaction current appendRequest(mkdirRequest)
		self
	)
	
	linkToRequest := method(aPath,
		request\
			setAction("link")\
			addQuerySlots("fromPath toPath key")\
			setPath(self path)\
			setToPath(aPath pathComponent)\
			setKey(aPath lastPathComponent)\
			setHttpMethod("post")
	)
	
	linkTo := method(aPath,
		Transaction current appendRequest(linkToRequest(aPath))
		self
	)
	
	moveKeyToNode := method(key, node,
		nodeAt(key) linkTo(node nodeAt(key))
		rm(key)
		self
	)
	
	//queues
	queuePopToRequest := method(aPath,
		request\
			setAction("queuePopTo")\
			addQuerySlots("path ttl whereKey whereValue")\
			setPath(aPath)\
			setHttpMethod("post")
	)
	
	queuePopTo := method(aPath,
		Transaction current appendRequest(queuePopToRequest(aPath))
	)
	
	queueToNode := method(aNode,
		queuePopTo(aNode path)
	)
	
	queueExpireToRequest := method(aPath,
		request\
			setAction("queueExpireTo")\
			addQuerySlots("path")\
			setPath(aPath)\
			setHttpMethod("post")
	)
	
	queueExpireTo := method(aPath,
		Transaction current appendRequest(queueExpireToRequest(aPath))
		self
	)
	
	queueExpireToNode := method(aNode,
		queueExpireTo(aNode path)
		self
	)
	
	//
)