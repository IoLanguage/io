
MessageTest := UnitTest clone do(
	testClone := method(
		message := Message clone
		assertNotSame(Message, message)
		assertEquals(Message name, message name)
		assertEquals(Message next, message next)
		assertEquals(Message arguments, message arguments)
	)

	// A colon is not part of an identifier, so assignment needs no space
	// before the operator. `a:=2` used to lex as `a:` `=` `2` and fail with
	// "Slot a: not found" (#253).
	testAssignWithoutSpaces := method(
		a:=2
		assertEquals(a, 2)
		a=3
		assertEquals(a, 3)
		o := Object clone
		o b::=4
		assertEquals(o b, 4)
		o setB(5)
		assertEquals(o b, 5)
		assertEquals(message(c:=1) asString, "setSlot(\"c\", 1)")
	)
)
