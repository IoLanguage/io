
MDOProxy := Object clone do(
	connection ::= nil
	
	forward := method(
		connection send(call message name, call evalArgs)
	)
)

Message setCachedArgs := method(args,
	args foreach(arg, self appendCachedArg(arg))
)

MDOConnection := Object clone do(
	socket ::= nil
	port ::= 8456
	corosWaitingOnResponses ::= nil
	
	remoteObject ::= nil
	localObject ::= nil
	
	init := method(
		setSocket(Socket clone setPort(port))
		setRemoteObject(MDOProxy clone setConnection(self))
		setCorosWaitingOnResponses(Map clone)
	)
	
	connect := method(
		socket connect
	)
	
	close := method(
		socket close
	)	
	
	send := method(messageName, args,
		coro := Coroutine currentCoroutine
		messageId := coro uniqueId asString
		socket writeListMessage(list("s", messageId, messageName) appendSeq(args))
		corosWaitingOnResponses atPut(id, coro)
		coro pause
		corosWaitingOnResponses removeAt(id)
		coro result
	)
	
	receiveLoop := method(
		while(socket isOpen,
			args := socket readListMessage
			messageType := args removeFirst
			
			if (messageType == "s") then(
				receiveSend(args)
			) elseif(messageType == "r") then(
				receiveResponse(args)
			) else(
				writeln("Warning: invalid message type: ", messageType, " - ignoring")
			)
		)
	)
	
	receiveResponse := method(args,
		messageId := args first
		result := args second
		coro := corosWaitingOnResponses at(messageId)
		if(coro) then(
			coro setResult(result) resumeLater
			yield
		) else(
			writeln("Warning: response to unknown coro : ", messageId, " - ignoring")
		)
	)
	
	receiveSend := method(args,
		messageId := args removeFirst
		messageName := args removeFirst
		m := Message clone setName(messageName) setCachedArgs(args)
		if(localObject acceptedMessageNames contains(s name)) then(
			result := localObject doMessage(m)
			socket writeListMessage(list("r", messageId, result))
		) else(
			writeln("Warning: unaccepted message '", s name, " - returning nil")
			socket writeListMessage(list("r", messageId, nil))
		)		
	)
)
