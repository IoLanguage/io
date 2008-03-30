
MessageTest := UnitTest clone do(
	testClone := method(
		message := Message clone
		assertNotSame(Message, message)
		assertEquals(Message name, message name)
		assertEquals(Message next, message next)
		assertEquals(Message arguments, message arguments)
	)
)
