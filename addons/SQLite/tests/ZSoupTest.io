SQLite

Soup deleteDB := method(
	File clone setPath(self db path) remove
)

SQLiteTest := UnitTest clone do(	

	test_1_open := method(
		self s := Soup clone
		s setPath("mySoup.sqlite") 
		s delete
		s open(list("name", "city"))
		assertTrue(s isOpen)
	)
	
	test_2_append := method(
		contact := Object clone
		contact name := "Steve"
		contact city := "San Francisco"
		
		s append(contact)
		self items := s itemsWhere("name", "Steve")
		//items foreach(item, item print)
		assertEquals(items size, 1)
	)
	
	test_3_update := method(
		rContact := items first
		rContact city := "Melborne"		
		s update(rContact)
		
		self items := s itemsWhere("name", "Steve")
		assertEquals(items size, 1)
		assertEquals(items first city, "Melborne")
	)
	
	test_4_remove := method(
		items foreach(i, item, s remove(item))
		items := s itemsWhere("name", "Steve")
		assertEquals(items size, 0)
	)
	
	test_5_close := method(
		s close
		assertFalse(s isOpen)
		s delete
	)
)
