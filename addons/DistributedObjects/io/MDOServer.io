
MDOServer := Server clone do(
	setPort(MDOConnection port)
	localObject ::= nil
	handleSocket := method(aSocket,
		MDOConnection clone setLocalObject(localObject) setSocket(aSocket) @receiveLoop
	)
)
