TokyoCabinet

TokyoCabinetTest := UnitTest clone do(
	cleanUp := method(
		File with("test.db") remove
	)
	
	testBasic := method(
		db := TokyoCabinet clone
		File with("test.db") remove		
		db setPath("test.db") open
		
		assertEquals(nil, db at("foo"))
		
		db begin
		db atPut("001/a", "1")
		db commit
		
		assertEquals("1", db at("001/a"))
		
		db begin
		db atPut("001/b", "2")
		db commit
		
		db begin
		db atPut("001/c", "3")
		db commit
		
		assertEquals("3", db at("001/c"))
		assertEquals("2", db at("001/b"))
		assertEquals("1", db at("001/a"))
		
		db close
		db setPath("test.db") open
		
		assertEquals("1", db at("001/a"))
		assertEquals("2", db at("001/b"))
		assertEquals("3", db at("001/c"))
		
		c := db cursor
		c first
		assertEquals("001/a", c key)
		assertEquals("1", c value)
		
		c next
		assertEquals("001/b", c key)
		assertEquals("2", c value)
		
		c next
		assertEquals("001/c", c key)
		assertEquals("3", c value)
		c close
		
		/*
		db begin
		db removeAt("001/a")
		db removeAt("001/b")
		db commit
		
		assertEquals(nil, db at("001/a"))
		assertEquals(nil, db at("001/b"))
		
		db close
		db setPath("test.db") open
		
		assertEquals(nil, db at("001/a"))
		assertEquals(nil, db at("001/b"))
		
		db close
		*/
		
		// --------------------------------
		
		c := db prefixCursor setPrefix("001")
		c first
		assertEquals("a", c key)
		assertEquals("1", c value)
		
		c next
		assertEquals("b", c key)
		assertEquals("2", c value)
		
		c next
		assertEquals("c", c key)
		assertEquals("3", c value)

		c next
		assertEquals(nil, c key)
		assertEquals(nil, c value)
		c close
		
		db begin
		db removeAt("001/a")
		db removeAt("001/b")
		db commit
		
		assertEquals(nil, db at("001/a"))
		assertEquals(nil, db at("001/b"))
		
		db close
		db setPath("test.db") open
		
		assertEquals(nil, db at("001/a"))
		assertEquals(nil, db at("001/b"))
		
		db close
	)
)

