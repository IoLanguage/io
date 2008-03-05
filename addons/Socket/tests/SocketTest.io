Socket

//Object uniqueHexId := method("0x" .. uniqueId asString toBase(16))

SocketTest := UnitTest clone do(

	testSocketHttpClient := method(
		s := Socket clone setHost("www.google.com") setPort(80)
		s streamOpen
		s connect
		s streamWrite("GET http://www.google.com/ HTTP/1.0\r\n\r\n")
		s streamReadNextChunk
		assertTrue(s readBuffer size > 0)
	)	
	
	//debugWriteln := getSlot("writeln")
	port := 5000
	
	runServer := method(
		coro := Scheduler currentCoroutine 
		debugWriteln("--- runServer coro ", coro uniqueId)
		
		coro setLabel("S_" .. coro uniqueId)
		sv := Socket clone setHost("127.0.0.1") setPort(port)
		sv serverOpen
		debugWriteln("serverWaitForConnection...")
		s := sv serverWaitForConnection
		debugWriteln("server got connection... ", s type)
		debugWriteln("server isOpen: ", s isOpen)
		s streamReadNextChunk
		debugWriteln("server read: '", s readBuffer, "'")
		debugWriteln("server isOpen: ", s isOpen)
		assertEquals(s readBuffer, "test")
		debugWriteln("server write: '", s readBuffer, "'")
		debugWriteln("server isOpen: ", s isOpen)
		s streamWrite(s readBuffer)
		in := s readMessage
		debugWriteln("s readMessage = ", in)
		assertEquals(in, "this is a test message")
		debugWriteln("server close")
		s close
	)
		
	runClient := method(
		debugWriteln("--- runClient coro ", Scheduler currentCoroutine uniqueId)
		coro := Scheduler currentCoroutine 
		coro setLabel("C_" .. coro uniqueId)
		client := Socket clone setHost("127.0.0.1") setConnectTimeout(1) setReadTimeout(1) setPort(port)
		assertFalse(client streamOpen isError)
		assertFalse(client connect isError)
		debugWriteln("client write")
		client streamWrite("test")
		debugWriteln("client read")
		client streamReadNextChunk
		debugWriteln("client read: '", client readBuffer, "'")
		debugWriteln("client got: '", client readBuffer, "'")
		assertEquals(client readBuffer, "test")
		debugWriteln("client writeMessage")
		client writeMessage("this is a test message")
		debugWriteln("client close")
		client close
	)
		
	testClientServer := method(
		debugWriteln("\ntestClientServer... coro = ", Scheduler currentCoroutine uniqueId)
		Scheduler currentCoroutine setLabel("main")
		self coroDo(runServer)
		yield
		runClient
		while(Scheduler yieldingCoros size > 1, yield)
	)
	
)


