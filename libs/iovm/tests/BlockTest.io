BlockTest := UnitTest clone do(

	testClone := method(
		block1 := Lobby getSlot("Block") clone
		assertNotEquals(Lobby getSlot("Block") uniqueId, getSlot("block2") uniqueId)
		block2 := Lobby getSlot("Block") clone
		assertNotEquals(getSlot("block1") uniqueId, getSlot("block2") uniqueId)
	)

	_testPrint := method(
		// would need to be able to read stdout or install a printCallback from Io to test print()
		Nop
	)

	testCode := method(
		assertEquals("block(foo bar)", block(foo bar) code)
		testBlock := block(foo bar)
		getSlot("testBlock") setScope(nil)
		assertEquals("method(foo bar)", getSlot("testBlock") code)
		getSlot("testBlock") setScope(self)
		assertEquals("block(foo bar)", getSlot("testBlock") code)
	)

	testMessage := method(
		assertRaisesException(block(foo bar) setMessage)
		assertRaisesException(block(foo bar) setMessage(nil))
		assertEquals("foo bar", block(foo bar) message code)
		testMessage := Message fromString("blah fasel")
		assertEquals(testMessage, block(foo bar) setMessage(testMessage) message)
	)

	testArgumentNames := method(
		assertRaisesException(block(foo bar) setArgumentNames)
		assertRaisesException(block(foo bar) setArgumentNames(nil))
		testBlock := block(a, b, c, foo bar)
		assertEquals(List clone append("a", "b", "c"), getSlot("testBlock") argumentNames)
		testBlock = block(foo bar)
		assertEquals(List, getSlot("testBlock") argumentNames)
		getSlot("testBlock") setArgumentNames(List clone append("d", "e"))
		assertEquals(List clone append("d", "e"), getSlot("testBlock") argumentNames)
	)

	testConstruction := method(
		testMessage := Message fromString("arg1 + arg2")
		argumentNames := List clone append("arg1", "arg2")
		testBlock := Lobby getSlot("Block") clone setMessage(testMessage) setArgumentNames(argumentNames)
		assertEquals(5, testBlock call(3, 2))
	)

	testScope := method(
		assertSame(self, block(foo bar) scope self)

		selfBlock := block(thisContext)
		getSlot("selfBlock") setScope(nil)
		assertNil(getSlot("selfBlock") scope)
		//assertSame(thisContext, selfBlock call call target) // should this be true? block recycling doesn't like it?
		//assertSame(getSlot("selfBlock") message, selfBlock call message)

		object := Object clone
		getSlot("selfBlock") setScope(object)
		assertSame(object, getSlot("selfBlock") scope)

		getSlot("selfBlock") setScope
		assertNil(getSlot("selfBlock") scope)
	)
	
	testPassStops := method(
		b := block(return "block")
		a := method(b call; return "method")
		assertEquals(a, "method")

		b := block(return "block") setPassStops(true)
		a := method(b call; return "method")		
		assertEquals(a, "block")
	)

/*
	testIfTrue := method(
		assertRaisesException(block(1) ifTrue)
		called := nil
		block(1) ifTrue(called = 1)
		assertTrue(called)
		block(nil) ifTrue(fail)
	)

	testIfFalse := method(
		assertRaisesException(block(1) ifFalse)
		block(1) ifFalse(fail)
		called := nil
		block(nil) ifFalse(called = 1)
		assertTrue(called)
	)

	testWhileTrue := method(
		assertRaisesException(block(1) whileTrue)
		callCounter := 0
		amount := 3
		block(amount = amount -1; amount >= 0) whileTrue(callCounter = callCounter + 1)
		assertEquals(3, callCounter)
	)

	testWhileFalse := method(
		assertRaisesException(block(1) whileFalse)
		callCounter := 0
		amount := 3
		block(amount = amount -1; amount < 0) whileFalse(callCounter = callCounter + 1)
		assertEquals(3, callCounter)
	)
*/
)
