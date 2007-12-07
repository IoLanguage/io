SQLite

DBMTest := UnitTest clone do(

	test_1_open := method(
		File setPath("myDBM.sqlite") remove	
		self s := DBM clone
		s setPath("myDBM.sqlite") 
		assertEquals(s path, "myDBM.sqlite")
		s open
		assertTrue(s isOpen)
	)
	
	test_2_atPut := method(
		s atPut("a", "foo")
		s atPut("b", "bar")

		assertEquals(s at("a"), "foo")
		assertEquals(s at("b"), "bar")
	)
	
	test_3_close := method(
		s close
		File setPath("myDBM.sqlite") remove	
	)
)
