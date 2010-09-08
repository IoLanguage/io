
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
		queryRequest setOp("keys") setBefore("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ") setCount(1) results first // hack
	)
	
	firstValue := method(
		queryRequest setOp("values") setCount(1) results first
	)
	
	lastValue := method(
		queryRequest setOp("values") setBefore("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ") setCount(1) results first // hack
	)

	keys := method(
		queryRequest setOp("keys") results
	)
	
	hasKey := method(key,
		read(key) != nil
	)
	
	exists := method(
		e := try(VertexDB Node with(path pathComponent) hasKey(key))
		if(e,
			if(e error containsSeq("path does not exist"), false, e pass)
		,
			true
		)
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
	
	metaRead := method(key,
		ReadRequest clone setPath(path) setKey(key) setMode("meta") results
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
		r := QueueExpireToRequest clone setPath(path) setToPath(aPath)
		t := Transaction current
		t appendRequest(r)
		self
	)
	
	queueExpireToNode := method(aNode,
		queueExpireTo(aNode path)
		self
	)
	
/*
	foreach := method(
		slotName := call message arguments first name
		todo := call message arguments at(1)
		keys foreach(key,
			call sender setSlot(slotName, key)
			call sender doMessage(todo)
		)
	)
*/
	foreachBlock := method(b,
		last := ""
		loop(
			keys := queryRequest setOp("keys") setAfter(last) setCount(200) results
			if(keys isEmpty, break)
			keys foreach(k, b call(k, nodeAt(k)))
			last := keys last
		)
	)
	
	foreachAttributeBlock := method(attribute, b,
		last := ""
		loop(
			value := queryRequest setOp("value") setAfter(last) setAttribute(attribute) setCount(200) results
			if(keys isEmpty, break)
			keys foreach(k, b call(k, nodeAt(k)))
			last := keys last
		)
	)

	renameKey := method(oldKey, newKey,
		if(oldKey == newKey, return self)
		nodeAt(oldKey) linkTo(self nodeAt(newKey) path)
		rm(oldKey)
		self	
	)
) 

