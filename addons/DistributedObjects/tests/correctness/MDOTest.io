DistributedObjects

root := Object clone do(
	increment := method(v, 1 + v)
)

MDOTest := UnitTest clone do(	
	done := false
	
	doit := method(
		writeln("starting server")
		server := MDOServer clone setHost("127.0.0.1") debugOn setLocalObject(root) 
		server @start
		yield

		writeln("starting connection")
		con := MDOConnection clone setHost("127.0.0.1") connect
		writeln("connected to server")
		yield
		writeln("sending message")
		result := con send("increment", list(1))
		writeln("result = ", result)
		yield
		writeln("result = ", result)
		assertEquals(result, 2)
		done = true
	)
	
	testMDO := method(
		assertEquals(1, 2)
		return
		
		self @doit
		while(done == false,
			System sleep(.1)
			writeln("yield")
			yield
		)
	)
) 