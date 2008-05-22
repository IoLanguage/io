DistributedObjects

root := Object clone do(
	increment := method(v, 
		1 + v
	)
)

MDOTest := UnitTest clone do(	
	doit := method(
		writeln("starting server")
		server := MDOServer clone setHost("127.0.0.1") debugOn setLocalObject(root) 
		server @@start
		yield

		con := MDOConnection clone setHost("127.0.0.1") connect
			yield
		result := con send("increment", list(1))
		writeln("result = ", result)
		yield
		writeln("result = ", result)
		assertEquals(result, 2)
	)
	
	testMDO := method(
		self @@doit
		yield
		//while(Scheduler yieldingCoros size > 0,
			while(true,
				System sleep(.1)
				writeln("yielding")
			//writeln("Scheduler yieldingCoros size = ", Scheduler yieldingCoros size)
			yield
		)
	)
) 