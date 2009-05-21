//metadoc MDOConnection category Networking
/*metadoc MDOConnection description
A Minimal Distributed Objects connection. Example;
<pre>
dateServerCon := MDOConnection clone setHost("127.0.0.1") setPort(8123) connect
writeln("date from date server: ", Date fromNumber(dateServerCon currentDate))
dateServerCon close
</pre>

See the docs for MDOServer for the DateServer code.
<p>
A MDOConnection will pause calling coroutines until the response is received. 
Mutliple requests can be sent before a single request returns if they are sent 
from separate coroutines.
*/

DistributedObjects := Object clone

MDOProxy := Object clone do(
	connection ::= nil
	
	forward := method(
		connection send(call message name, call evalArgs)
	)
)

//doc Message setCachedArgs(listOfValues) Appends evaluated arguments to a message. Returns self. 
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
	
	debugWriteln := nil //getSlot("writeln")
	
	send := method(messageName, args,
		writeln("con send(", messageName, ", ", args, ")")
		coro := Coroutine currentCoroutine
		messageId := coro uniqueId asString // not entirely safe...
		socket writeListMessage(list("s", messageId, messageName) appendSeq(args))
		corosWaitingOnResponses atPut(messageId, coro)
		writeln("pausing coro ", messageId)
		coro pause
		writeln("resumed coro ", messageId)
		corosWaitingOnResponses removeAt(messageId)
		coro result
	)
	
	receiveLoop := method(
		while(socket isOpen,
			args := socket readListMessage
			//debugWriteln("got message: ", args)
			messageType := args removeFirst
			if(messageType == "s", receiveSend(args); continue)
			if(messageType == "r", receiveResponse(args); continue)
			writeln("Warning: invalid message type: ", messageType, " - ignoring")
		)
	)
	
	receiveResponse := method(args,
		writeln("receiveResponse")
		messageId := args first
		result := args second
		coro := corosWaitingOnResponses at(messageId)
		if(coro == nil,
			writeln("Warning: response to unknown coro : ", messageId, " - ignoring")
			return
		)
		coro setResult(result) resumeLater
		yield
	)
	
	receiveSend := method(args,
		writeln("receiveSend")
		messageId := args removeFirst
		messageName := args removeFirst
		m := Message clone setName(messageName) 
		if(args, m setCachedArgs(args))
		if(localObject acceptedMessageNames contains(messageName),
			result := localObject doMessage(m)
			//r := list("r", messageId, result)
			//writeln("messageId = ", messageId, " ", messageId type)
			//writeln("result = ", result, " ", result type)
			socket writeListMessage(list("r", messageId, result))
			//debugWriteln("socket writeListMessage(", r, ")")
		,
			writeln("Warning: unaccepted message '", messageName, " - returning nil")
			socket writeListMessage(list("r", messageId, nil))
		)
		//debugWriteln("done with receiveSend")
	)
)
