//usage: DBQueue clone setPath(path) setTarget(target) setMessageName(message)

VertexDB Queue := Object clone do(
	//internal
	
	//api
	target ::= nil
	messageName ::= nil
	expiresActive ::= true
	concurrency ::= 10
	node ::= nil
	qNodes ::= method(
		setQNodes(
			"waiting active done error jobs" split map(name,
				self setSlot(name .. "Node", node nodeAt(name))
			)
		)
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
		node create
		createNodes
	)
	
	createNodes := method(
		qNodes foreach(create)
	)
	
	empty := method(
		qNodes foreach(remove) 
		qNodes foreach(create)
	)
	
	path := method(node path)
	
	process := method(
		if(expiresActive, expireActive)
		processedOne := false
		self jobs := List clone
		while(k := waitingNode queueToNode(activeNode),
			if(k == "", break)
			waiting := false
			while(jobs size >= concurrency,
				yield
			)
			processedOne = true
			
			job := AsyncJob clone setNode(activeNode nodeAt(k)) setQueue(self)
			jobs append(job)
			
			if(concurrency > 1, job @@run, job run)
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

		//Transaction current begin

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
		/*
		e := try(Transaction current commit)
		if(e,
			writeln("Transaction current commit error in DBQueue")
		)
		*/
	)
	
	finishedJob := method(job,
		self jobs remove(job)
	)
	
	expireActive := method(
		count := activeNode queueExpireToNode(waitingNode)
		//writeln(qNode path, " expired ", count, " from active to waiting")
	)
)
