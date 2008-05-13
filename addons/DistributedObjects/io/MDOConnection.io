DistributedObjects := Object clone

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
	setPort := method(port, socket setPort(port); self)
	corosWaitingOnResponses ::= nil
	
	remoteObject ::= nil
	localObject ::= nil
	
	init := method(
		setSocket(Socket clone setPort(8456))
		setRemoteObject(MDOProxy clone setConnection(self))
		setCorosWaitingOnResponses(Map clone)
	)
	
	setHost := method(host,
		socket setHost(host)
		self
	)
	
	connect := method(
		socket connect
		if(socket isOpen not, Exception raise(self type .. " unable to connect to host " ..  socket host))
		self
	)
	
	close := method(
		socket close
		self
	)	
	
	send := method(messageName, args,
		//writeln("con send(", messageName, ", ", args, ")")
		coro := Coroutine currentCoroutine
		messageId := coro uniqueId asString
		socket writeListMessage(list("s", messageId, messageName) appendSeq(args))
		corosWaitingOnResponses atPut(messageId, coro)
		//writeln("pausing coro ", messageId)
		coro pause
		//writeln("resumed coro ", messageId)
		corosWaitingOnResponses removeAt(messageId)
		coro result
	)
	
	receiveLoop := method(
		while(socket isOpen,
			args := socket readListMessage
			//writeln("got message: ", args)
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
		if(localObject acceptedMessageNames contains(messageName)) then(
			result := localObject doMessage(m)
			socket writeListMessage(list("r", messageId, result))
		) else(
			writeln("Warning: unaccepted message '", s name, " - returning nil")
			socket writeListMessage(list("r", messageId, nil))
		)		
	)
)
