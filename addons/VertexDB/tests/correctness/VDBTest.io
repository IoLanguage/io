
VertexDBTest := UnitTest clone do(
	prependProto(VertexDB)
	
	testBasic := method(
		VertexDB Settings setHost("localhost") setPort(8080)
		Request debugOn
			
		foo  := Node with("/foo") mkdir
		bar  := Node with("/bar") mkdir
		item := Node with("/foo/1") mkdir
		pop  := Node with("/foo") queuePopTo("/bar")
		
		assertEquals(pop, "1")
	)
) 