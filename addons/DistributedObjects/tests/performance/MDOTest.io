DistributedObjects

root := Object clone do(
	acceptedMessageNames := list("increment")
	
	value := 0
	
	increment := method(
		//writeln("increment")
		//value = value + 1
		//value
		nil
	)
)

MDOTest := UnitTest clone do(	
	testMDO := method(
		writeln("MDOTest starting server")
		server := MDOServer clone setHost("127.0.0.1") setPort(8123) debugOn setLocalObject(root) 
		server @@start
		yield

		con := MDOConnection clone setHost("127.0.0.1") setPort(8123) connect
		con @receiveLoop
		yield
		count := 1000
		t := Date clone now
		count repeat(
			//con send("increment", list(1))
			con remoteObject increment
		)
		dt := Date clone now - t
		writeln((count/dt totalSeconds) floor, " sync message send & receive response per second")
	)
) 