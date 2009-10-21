//usage: DBQueue clone setPath(path) setTarget(target) setMessageName(message)

VertexDB Queue := Object clone do(
	//internal
	
	//api
	target ::= nil
	messageName ::= nil
	expiresActive ::= true
	concurrency ::= 10
	node ::= nil
	qNodes ::= nil
	
	waitingNode ::= method(node nodeAt("waiting"))
	activeNode ::= method(node nodeAt("active"))
	doneNode ::= method(node nodeAt("done"))
	errorNode ::= method(node nodeAt("error"))
	
	qNodes ::= method(
		list(waitingNode, activeNode, doneNode, errorNode)
	)
	
	path := method(node path)
	setPath := method(call delegateTo(node); self)
	
	host := method(node host)
	setHost := method(call delegateTo(node); self)
	
	port := method(node port)
	setPort := method(call delegateTo(node); self)
	
	with := method(path,
		Node with(path) asQueue
	)
	
	create := method(
		node mkdir
		createNodes
	)
	
	createNodes := method(
		qNodes foreach(mkdir)
		self
	)
	
	empty := method(
		qNodes foreach(empty)
		self
	)
	
	path := method(node path)
	
	process := method(
		if(expiresActive, expireActive)
		processedOne := false
		self jobs := List clone
		while(k := waitingNode queueToNode(activeNode),
			debugWriteln("waitingNode<", waitingNode path, "> queueToNode(<", activeNode path, ">) == <", k, ">")
			if(k == nil, break)
			debugWriteln("jobs size: ", jobs size)
			debugWriteln("concurrency: ", concurrency)
			while(jobs size >= concurrency,
				debugWriteln("waiting to run new job ...")
				yield
			)
			processedOne = true
			
			job := AsyncJob clone setNode(activeNode nodeAt(k)) setQueue(self)
			jobs append(job)
			
			if(concurrency > 1, job @@run, job run)
		)
		while(jobs size > 0,
			debugWriteln("waiting for jobs to finish ...")
			yield
		)
		processedOne
	)
	
	AsyncJob := Object clone do(
		queue ::= nil
		node ::= nil
		
		run := method(
			runError := try(
				queue processNode(node)
			)
			
			if(runError,
				debugWriteln("AsyncJob run error:")
				debugWriteln(runError coroutine backTraceString)
			)
			queue finishedJob(self)
		)
	)
	
	processNode := method(node,
		error := nil

		VertexDB Transaction current begin

		debugWriteln("VertexDB Queue messageName == <", messageName, ">")
		e := try(target perform(messageName, node))
		if(e, errorMessage := e coroutine backTraceString)

		if(e,
			if(errorMessage not, errorMessage = "unknown error")
			debugWriteln("Error performing " .. messageName .. " in Queue")
			debugWriteln(errorMessage)
			node atWrite("_error", errorMessage asMutable replaceSeq("\n", "<br>"))
			activeNode moveKeyToNode(node key, errorNode)
		,
			activeNode moveKeyToNode(node key, doneNode)
		)
		///*
		e := try(Transaction current commit)
		if(e,
			writeln("Transaction current commit error in DBQueue")
		)
		//*/
	)
	
	finishedJob := method(job,
		self jobs remove(job)
	)
	
	expireActive := method(
		count := activeNode queueExpireToNode(waitingNode)
		//writeln(qNode path, " expired ", count, " from active to waiting")
	)
)
