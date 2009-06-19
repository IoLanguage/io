DistributedObjects

DOTest := UnitTest clone do(
	old_testDO := method(
		nil
		/*
		Root := Object clone
		Root test := method(v, return list(1))

		doServer := DOServer clone debugOn
		doServer setRootObject(Root)
		doServer setPort(8456)
		doServer @@start
		//doServer coroDo(start)
		yield
		DOConnection
		con := DOConnection clone setHost("127.0.0.1") setPort(8456) connect
		assertFalse(con isError)
		*/
		
		/*
		
		result := con serverObject test(1)
		assertFalse(result isError)
		assertEquals(result type, "DOProxy")
		
		r := result at(0)
		assertEquals(r, 1)
		
		r := result at(1)
		assertEquals(r, nil)
		*/
	)
) 