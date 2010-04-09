CFFI

// TODO Need to check endianness

ArrayTest := UnitTest clone do(
	testWith := method(
		appendProto(Types)

		length := 1000

		AI := Array with(Int, length)

		a := AI clone

		for(i, 0, length - 1, 1,
			a atPut(i, i)
		)

		for(i, 0, length - 1, 1,
			assertEquals(a at(i), i)
		)

	)

	testNestedByValueAtPut := method(
		appendProto(Types)

		length := 100

		AI := Array with(Int, length)
		BAI := Array with(AI, 2)

		ai1 := AI clone
		ai2 := AI clone

		for(i, 0, length - 1, 1,
			ai1 atPut(i, i)
			ai2 atPut(i, i + length)
		)

		bai := BAI with(ai1, ai2)

		for(i, 0, length - 1, 1,
			assertEquals(ai1 at(i), i)
			assertEquals(bai at(0) at(i), ai1 at(i))
			assertEquals(ai2 at(i), i + length)
			assertEquals(bai at(1) at(i), ai2 at(i))
		)
	)

	testNestedByRefAtPut := method(
		appendProto(Types)

		length := 100

		AI := Array with(Int, length)
		BAI := Array with(AI ptr, 2)

		ai1 := AI clone
		ai2 := AI clone

		for(i, 0, length - 1, 1,
			ai1 atPut(i, i)
			ai2 atPut(i, i + length)
		)

		bai := BAI with(ai1, ai2)

		for(i, 0, length - 1, 1,
			assertEquals(ai1 at(i), i)
			assertEquals(bai at(0) value at(i), ai1 at(i))
			assertEquals(ai2 at(i), i + length)
			assertEquals(bai at(1) value at(i), ai2 at(i))
		)
	)

	testPointerToPointers := method(
		appendProto(Types)

		AI := Array with(Int, 10)

		ai    := AI with(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)

		pai   := AI ptr clone setValue(ai)
		ppai  := AI ptr ptr clone setValue(pai)
		pppai := AI ptr ptr ptr clone setValue(ppai)
		
		assertEquals("^["  ,   pai typeString)
		assertEquals("^^[" ,  ppai typeString)
		assertEquals("^^^[", pppai typeString)
		
		for(i, 0, 10 - 1, 1,
			assertEquals(ai at(i),   pai value at(i))
			assertEquals(ai at(i),   ppai value value at(i))
			assertEquals(ai at(i),   pppai value value value at(i))
		)
	)
	
	testAddress := method(
		appendProto(Types)

		AI := Array with(Int, 10)

		ai    := AI with(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)

		pai   := AI ptr clone
		ppai  := AI ptr ptr clone
		pppai := AI ptr ptr ptr clone

		assertEquals(nil,   pai address)
		assertEquals(nil,  ppai address)
		assertEquals(nil, pppai address)

		pai   setValue(ai)
		ppai  setValue(pai)
		pppai setValue(ppai)

		assertEquals(pai  address,  ppai value address)
		assertEquals(pai  address, pppai value value address)
		assertEquals(ppai address, pppai value address)

	)

	testPointedToType := method(
		appendProto(Types)
		
		AI := Array with(Int, 10)

		1pAI := AI ptr clone
		2pAI := AI ptr ptr clone
		3pAI := AI ptr ptr ptr clone
		4pAI := AI ptr ptr ptr ptr clone
		5pAI := AI ptr ptr ptr ptr ptr clone
		
		assertEquals(5pAI proto,							AI ptr ptr ptr ptr ptr)
		assertEquals(5pAI pointedToType,						4pAI proto)
		assertEquals(5pAI pointedToType pointedToType,					3pAI proto)
		assertEquals(5pAI pointedToType pointedToType pointedToType,			2pAI proto)
		assertEquals(5pAI pointedToType pointedToType pointedToType pointedToType,	1pAI proto)
		
		assertEquals(4pAI proto,					AI ptr ptr ptr ptr)
		assertEquals(4pAI pointedToType,				3pAI proto)
		assertEquals(4pAI pointedToType pointedToType,			2pAI proto)
		assertEquals(4pAI pointedToType pointedToType pointedToType,	1pAI proto)
		
		assertEquals(3pAI proto,			AI ptr ptr ptr)
		assertEquals(3pAI pointedToType,		2pAI proto)
		assertEquals(3pAI pointedToType pointedToType,	1pAI proto)
		
		assertEquals(2pAI proto,		AI ptr ptr)
		assertEquals(2pAI pointedToType,	1pAI proto)
		
		assertEquals(1pAI proto, AI ptr)
	)

)
