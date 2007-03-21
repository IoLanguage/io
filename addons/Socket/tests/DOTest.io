Socket

DOTest := UnitTest clone do(
	testDO := method(
		Root := Object clone
		Root test := method(v, return list(1))

		doServer := DOServer clone
		doServer setRootObject(Root)
		doServer setPort(8456)
		doServer coroDo(start)
		yield
		
		con := DOConnection clone setHost("127.0.0.1") setPort(8456) connect
		
		con ifNil(Exception raise("Error: unable to connect\n"))
		
		result := con serverObject test(1)
		assertEquals(result type, "DOProxy")
		
		r := result at(0)
		assertEquals(r, 1)
		
		r := result at(1)
		assertEquals(r, nil)
	)
) 