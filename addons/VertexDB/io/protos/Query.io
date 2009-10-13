VertexDB Query := Object clone do(
	//api
	node ::= nil
	
	op ::= nil
	before ::= nil
	after ::= nil
	whereKey ::= nil
	whereValue ::= nil
	
	httpMethod ::= "get"
	
	request := method(
		node request\
			setHttpMethod(httpMethod)\
			setAction("select")\
			addQuerySlots("op before after count whereKey whereValue")\
			setOp(op)\
			setBefore(before)\
			setAfter(after)\
			setWhereKey(whereKey)\
			setWhereValue(whereValue)
	)
	
	results := method(
		request results
	)
	
	//reads
	asKeys := method(
		setOp("keys")
	)
	
	keys := method(
		asKeys results
	)
	
	asValues := method(
		setOp("values")
	)
	
	values := method(
		asValues results
	)
	
	asPairs := method(
		setOp("pairs")
	)
	
	pairs := method(
		asPairs results
	)
	
	asCounts := method(
		setOp("counts")
	)
	
	counts := method(
		asCounts results
	)
	
	asObject := method(
		setOp("object")
	)
	
	object := method(
		asObject results
	)
	
	//writes
	
	asRm := method(
		setOp("rm") setHttpMethod("post")
	)
	
	rm := method(
		asRm results
	)
)