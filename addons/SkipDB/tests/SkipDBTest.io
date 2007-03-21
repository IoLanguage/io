SkipDB

SkipDBTest := UnitTest clone do(
	sdbm := SkipDBM clone setPath("foo")
	
	cleanUp := method(
		sdbm delete
	)
	
	test_1_Open := method(
		sdbm delete
		sdbm open
		self sdb := sdbm root
		assertEquals(true, sdbm isOpen)
	)
	
	test_2_Root := method(
		//self sdb := self sdbm root
		assertEquals("SkipDB", sdb type)
	)
	
	test_3_Insert := method(
		sdbm open
		assertEquals(true, sdbm isOpen)
		assertEquals(0, sdb size)
		assertEquals(nil, sdb at("foo"))
		sdb atPut("a", "1")
		assertEquals(1, sdb size)
		sdb atPut("foo", "bar")
		assertEquals(2, sdb size)
		sdb atPut("b", "2")
		assertEquals(3, sdb size)
		assertEquals("bar", sdb at("foo"))
	)
	
	test_4_Remove := method(
		sdb removeAt("foo")
		assertEquals(nil, sdb at("foo"))
	)
	
	test_5_1_Cursor := method(
		self cursor := sdb cursor
		assertEquals("SkipDBCursor", cursor type)
	)

	test_5_2_CursorFirst := method(
		cursor first
		assertEquals("a", cursor key)
		assertEquals("1", cursor value)
	)
		
	test_5_3_CursorNext := method(
		cursor next
		assertEquals("b", cursor key)
		assertEquals("2", cursor value)
	)
		
	test_5_4_CursorNextNil := method(
		cursor next
		assertEquals(nil, cursor key)
		assertEquals(nil, cursor value)
	)
		
	test_5_5_CursorLast := method(
		cursor last
		assertEquals("b", cursor key)
		assertEquals("2", cursor value)
	)
		
	test_5_6_CursorPrevious := method(
		cursor previous
		assertEquals("a", cursor key)
		assertEquals("1", cursor value)
	)
		
	test_5_7_CursorPreviousNil := method(
		cursor previous
		assertEquals(nil, cursor key)
		assertEquals(nil, cursor value)
	)
)
