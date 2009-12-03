//usage: DBQueue clone setPath(path) setTarget(target) setMessageName(message)

VertexDB Queue := Object clone do(
	//internal
	
	//api
	target ::= nil
	messageName ::= nil
	expiresActive ::= true
	concurrency ::= 1
	node ::= nil
	qNodes ::= nil
	itemsPerProcess ::= nil //if non nil then process at most itemsPerProcess
	jobs ::= nil
	processedCount ::= 0
	
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
	
	jobError ::= nil
	
	processCountLimited := method(
		if(itemsPerProcess, processedCount >= itemsPerProcess, false)
	)
	
	process := method(
		setJobError(nil)
		if(expiresActive, expireActive)
		processedOne := false
		setJobs(List clone)
		setProcessedCount(0)
		while(processCountLimited not and (k := waitingNode queueToNode(activeNode)),
			debugWriteln("waitingNode<", waitingNode path, "> queueToNode(<", activeNode path, ">) == <", k, ">")
			if(k == nil, break)
			debugWriteln("jobs size: ", jobs size)
			debugWriteln("concurrency: ", concurrency)
			while(jobs size >= concurrency,
				if(jobError, jobError pass)
				yield
			)
			processedOne = true
			
			job := AsyncJob clone setNode(activeNode nodeAt(k)) setQueue(self)
			jobs append(job)
			
			if(concurrency > 1, job @@run, job run)
		)
		while(jobs size > 0,
			if(jobError, jobError pass)
			yield
		)
		processedOne
	)
	
	AsyncJob := Object clone do(
		queue ::= nil
		node ::= nil
		
		run := method(
			e := try(
				queue processNode(node)
			)
			
			if(e, queue setJobError(e), queue finishedJob(self))
		)
	)
	
	processNode := method(node,
		error := nil

		VertexDB Transaction newForCoro doCommit(
			e := try(target perform(messageName, node))
			if(e, errorMessage := e coroutine backTraceString)

			node rm("_qexpire")
			node rm("_qtime")
			
			if(e,
				if(errorMessage not, errorMessage = "unknown error")
				debugWriteln("Error performing " .. messageName .. " in Queue")
				debugWriteln(errorMessage)
				target ?processNodeError(errorMessage)
				node atWrite("_error", errorMessage asMutable replaceSeq("\n", "<br>"))
				activeNode moveKeyToNode(node key, errorNode)
				//node setPath(Path with(errorNode path, node key))
			,
				activeNode moveKeyToNode(node key, doneNode)
				//node setPath(Path with(doneNode path, node key))
			)
		)
	)
	
	finishedJob := method(job,
		setProcessedCount(processedCount + 1)
		self jobs remove(job)
	)
	
	expireActive := method(
		count := activeNode queueExpireToNode(waitingNode)
		count
	)
	
	hasKey := method(k,
		qNodes detect(hasKey(k))
	)
)
