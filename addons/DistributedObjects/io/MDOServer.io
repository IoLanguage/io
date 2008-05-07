
MDOServer := Server clone do(
	localObject ::= nil
	handleSocket := method(aSocket,
		//writeln("MDOServer got connection")
		MDOConnection clone setLocalObject(localObject) setSocket(aSocket) @receiveLoop
	)
)
