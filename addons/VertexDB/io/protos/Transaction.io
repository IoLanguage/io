VertexDB Transaction := Object clone do(
	//internal
	requests ::= nil
	inTransaction ::= false
	coroSlotName ::= "currentVertexDBTransaction"
	sendsWrites ::= true
	
	writesOff := method(
		setSendsWrites(false)
	)
	
	writesOn := method(
		setSendsWrites(true)
	)
	
	init := method(
		setRequests(List clone)
		setInTransaction(false)
	)
	
	//api
	
	host ::= method(Settings host)
	port ::= method(Settings port)
	
	current := method(
		coro := Coroutine currentCoroutine
		while(coro and (t := coro getSlot(coroSlotName)) not,
			coro = coro parentCoroutine
		)
		
		if(t not, t = newForCoro)
		t
	)
	
	newForCoro := method(
		t := VertexDB Transaction clone
		Coroutine currentCoroutine setSlot(coroSlotName, t)
		t
	)
	
	appendRequest := method(request,
		request setShouldSend(sendsWrites)
		if(inTransaction,
			requests append(request)
			self
		,
			request results
		)
	)
	
	_begin := method(
		if(inTransaction, Exception raise("Already in transaction"))
		if(requests not, Exception raise("Uninitialized transaction"))
		setInTransaction(true)
		self
	)
	
	_commit := method(
		//write(" <join"); File standardOutput flush
		body := Sequence clone
		body setSize(1000000) setSize(0)
		requests foreach(r, body appendSeq(r resource, "\n"))
		body removeSuffix("\n")
		//write(">"); File standardOutput flush
		_abort
		//write(" <send"); File standardOutput flush
		if(body size > 0,
			tr := TransactionRequest clone
			tr setHost(host)
			tr setPort(port)
			tr setBody(body)
			tr results
		)
		//write(">"); File standardOutput flush
		self
	)
	
	_abort := method(
		//Coroutine currentCoroutine removeSlot(coroSlotName)
		setInTransaction(false)
		requests empty
		self
	)
	
	doCommit := method(
		if(inTransaction,
			call evalArgs
			return(self)
		)
		
		e := try(
			_begin
			call evalArgs
			_commit
		)
		if(e,
			_abort
			e pass
		)
		self
	)
)