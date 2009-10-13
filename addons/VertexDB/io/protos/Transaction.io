VertexDB Transaction := Object clone do(
	//internal
	requests ::= List clone
	inTransaction ::= false
	coroSlotName ::= "currentVertexDBTransaction"
	
	init := method(
		setRequests(requests clone)
		setInTransaction(false)
	)
	
	//api
	
	host ::= method(Settings host)
	port ::= method(Settings port)
	
	current := method(
		t := Coroutine currentCoroutine getSlot(coroSlotName)
		if(t not,
			t := self clone
			Coroutine currentCoroutine setSlot(coroSlotName, t)
		)
		t
	)
	
	appendRequest := method(request,
		if(inTransaction,
			requests append(request)
			self
		,
			request results
		)
	)
	
	begin := method(
		Exception raise("Transactions aren't currently supported")
		setInTransaction(true)
		self
	)
	
	commit := method(
		body := requests map(resource) join("\n")
		abort
		if(body size > 0,
			Request clone\
				setHost(host)\
				setPort(port)\
				setAction("transaction")\
				setBody(body)\
				results
		)
		self
	)
	
	abort := method(
		Coroutine currentCoroutine removeSlot(coroSlotName)
		setInTransaction(false)
		requests empty
		self
	)
	
	doCommit := method(
		e := try(
			begin
			call evalArgs
			commit
		)
		if(e,
			abort
			e pass
		)
		self
	)
)