TokyoCabinet

TokyoCabinetTest := UnitTest clone do(
	cleanUp := method(
		File with("test.db") remove
	)
	
	testBasic := method(
		db := TokyoCabinet clone
		File with("test.db") remove		
		db open("test.db")
		
		assertEquals(nil, db at("foo"))
		
		db begin
		db atPut("a", "1")
		db commit
		
		assertEquals("1", db at("a"))
		
		db begin
		db atPut("b", "2")
		db commit
		
		db begin
		db atPut("c", "3")
		db commit
		
		assertEquals("3", db at("c"))
		assertEquals("2", db at("b"))
		assertEquals("1", db at("a"))
		
		db close
		db open("test.db")
		
		assertEquals("1", db at("a"))
		assertEquals("2", db at("b"))
		assertEquals("3", db at("c"))
		
		c := db cursor
		c first
		assertEquals("a", c key)
		assertEquals("1", c value)
		
		c next
		assertEquals("b", c key)
		assertEquals("2", c value)
		
		c next
		assertEquals("c", c key)
		assertEquals("3", c value)
		c close
		
		db begin
		db removeAt("a")
		db removeAt("b")
		db commit
		
		assertEquals(nil, db at("a"))
		assertEquals(nil, db at("b"))
		
		db close
		db open("test.db")
		
		assertEquals(nil, db at("a"))
		assertEquals(nil, db at("b"))
		
		db close
	)
)

