
UserTest := UnitTest clone do(
	type := "UserTest"

	testUserName := method(		
		name := User name
		
		assertTrue(name isSymbol)
		assertTrue(name size > 0)
	)
	
	testHomeDirectory := method(		
		dir  := User homeDirectory
		assertEquals(dir type, "Directory")
	)
)