SQLite

SQLiteTest := UnitTest clone do(
	type := "SQLiteTest"
	
	test_1_open := method(
		self name := "myDatabase.sqlite"
		File clone setPath(name) remove
		self db := SQLite clone
		//db debugOn		
		db setPath(name)
		db open
		assertTrue(db isOpen)
	)
	
	test_2_create := method(
		db exec("CREATE TABLE Dbm (key, value)")
		db exec("CREATE INDEX DbmIndex ON Dbm (key)")
	)
	
	test_3_insert := method(
		db exec("INSERT INTO Dbm ('key', 'value') VALUES ('a', '123')")
		db exec("INSERT INTO Dbm ('key', 'value') VALUES ('a', 'efg')")
		rows := db exec("SELECT key, value FROM Dbm WHERE key='a'")
		assertEquals(rows size, 2)
	)
		
	test_3_delete := method(
		db exec("DELETE FROM Dbm WHERE key='a'")
		rows := db exec("SELECT key, value FROM Dbm WHERE key='a'")
		assertEquals(rows size, 0)
	)
	
	test_4_close := method(
		db close
		assertFalse(db isOpen)
		File clone setPath(db path) remove
	)
)