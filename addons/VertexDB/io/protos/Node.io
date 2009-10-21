
VertexDB Node := Object clone do(
	
	// api
	//host ::= method(Settings host)
	//port ::= method(Settings port)

	
	path ::= "/"
		
	with := method(path,
		self clone setPath(path)
	)
	
	nodeAt := method(name,
		Node clone setPath(Path with(path, name))
	)
	
	asQueue := method(
		Queue clone setNode(self)
	)
	
	// reads
	// query

	queryRequest := method(
		QueryRequest clone setPath(self path)
	)
	
	firstKey := method(
		queryRequest setOp("keys") setCount(1) results first
	)
	
	lastKey := method(
		queryRequest setOp("keys") setBefore("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ") setCount(1) results first
	)


	keys := method(
		queryRequest setOp("keys") results
	)
	
	values := method(
		queryRequest setOp("values") results
	)
	
	pairs := method(
		queryRequest setOp("pairs") results
	)
	
	counts := method(
		queryRequest setOp("counts") results
	)
	
	object := method(
		queryRequest setOp("object") results
	)
	
	empty := method(
		Transaction current appendRequest(queryRequest setOp("rm"))
	)
	/*
	rm := method(
		queryRequest setOp("rm") setMethod("rm") results
	)
	*/
		
	read := method(key,
		ReadRequest clone setPath(path) setKey(key) results
	)

	size := method(
		SizeRequest clone setPath(path) results
	)
	
	rm := method(key,
		Transaction current appendRequest(RmRequest clone setPath(path) setKey(key))
		self
	)
		
	atWrite := method(key, value,
		Transaction current appendRequest(
			//WriteRequest clone setPath(path) setKey(key) setValue(value)
			w := WriteRequest clone 
			w path := path
			w key := key
			w value := value
			w
		)
		self
	)
	
	mkdir := method(
		Transaction current appendRequest(MkdirRequest clone setPath(path))
		self
	)
	
	linkTo := method(aPath,
		Transaction current appendRequest(
			LinkToRequest clone setPath(self path) setToPath(aPath pathComponent) setKey(aPath lastPathComponent)
		)
		self
	)
	
	key := method(
		path lastPathComponent
	)
	
	moveKeyToNode := method(key, node,
		nodeAt(key) linkTo(node nodeAt(key) path)
		rm(key)
		self
	)
	
	//queues
	queuePopTo := method(aPath,
		QueuePopToRequest clone setPath(path) setToPath(aPath) results
	)
	
	queueToNode := method(aNode,
		queuePopTo(aNode path)
	)
	
	queueExpireTo := method(aPath,
		Transaction current appendRequest(QueueExpireToRequest clone setPath(path) setToPath(aPath))
		self
	)
	
	queueExpireToNode := method(aNode,
		queueExpireTo(aNode path)
		self
	)

) 

