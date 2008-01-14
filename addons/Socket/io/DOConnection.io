Object NULL := Sequence clone
Object NULL append(0x0)

DOConnection := Object clone do(
	type := "DOConnection"
	docCategory("Networking")

	docDescription("The DOConnection object is usefull for communicating with remote servers in a way that makes it look just like the sending of local messages. Proxies are automatically created on either side for passed objects, with the exception of strings and numbers, which are passed by value. Example:
	<pre>
	con := DOConnection clone setHost("127.0.0.1") setPort(8456) connect
	result := con serverObject test(1)
	writeln(result)
	r := result at(0)
	writeln(r)
	r := result at(1)
	writeln(r)
	</pre>

	Implementation Notes:

	<pre>
	  The format of the Distributed Objects message is a list of null
	  terminated strings in one of these two formats:

	  Send message format:

		s NULL targetId NULL messageName NULL argCount NULL argType NULL argValue NULL (next arg type and value, etc)

	  Reply message format:

		r NULL argType NULL argvalue NULL

	  If the argument is not a String, Number or nil then:
		If it is local to the sender, the type is RemoteObject
		If it is a proxy to a remote object, the type is LocalObject

	  This isn't optimized yet.
	  </pre>
	")

	type := "DOConnection"

	localObjects ::= nil
	proxies ::= nil
	socket ::= nil

	init := method(
		setLocalObjects(Map clone)
		setProxies(Map clone)
		setSocket(Socket clone)
	)

	handleSocket := method(aSocket, /* Called to when starting a server */
		debugWriteln("Got connection")
		setSocket(aSocket)
		while(socket isOpen, self readMessage)
		debugWriteln("Closed connection")
	)

	docSlot("close", "Close the connection, if it is open. Returns self.")

	close := method(
		socket close
		self
	)

	docSlot("setHost(ipString)", "Sets the host ip to connect to. Returns self.")
	docSlot("host", "Returns the host ip.")

	setHost := method(aString, socket setHost(aString); self)
	host := method(socket host)

	docSlot("setPort(portNumber)", "Sets the port number to connect to. Returns self.")
	docSlot("port", "Returns the port.")

	setPort := method(aString, socket setPort(aString); self)
	port := method(socket port)

	docSlot("connect", "Connect to the remote DOServer. Returns self. Raises an exception on error.")

	connect := method(
		self error := nil
		socket raiseOnError(socket connect)
		self serverObject := DOProxy clone setProxyId(0) setConnection(self)
		self
	)

	docSlot("serverObject", "A handle to the remote DOServer's root object.")

	sendMessage := method(m,
		ifDebug(write("sending message "); ShowMessage(m))
		raiseOnError(socket write(m))
		debugWriteln("waiting for result")
		socket readBuffer empty
		result := nil
		while(socket read,
			list := socket readBuffer splitNoEmpties(NULL)
			ifDebug(write("got result "); ShowMessage(socket readBuffer))
			type := list at(0) asString

			if(type == "r") then( /* if it's a result */
				argType  := list at(1) asString
				argValue := list at(2) asString
				result := self decode(argType, argValue)
				break
			) else(
				raiseOnError(performMessage(m))
			)
		)
		return result
	)

	readMessage := method(
		socket readBuffer empty
		if(socket read,
			m := socket readBuffer
			ifDebug(write("read message "); ShowMessage(m))
			self performMessage(m)
		)
	)

	performMessage := method(m,
		list := m splitNoEmpties(0 asCharacter)
		type := list at(0) asString
		objectId := list at(1) asString
		target := localObjects at(objectId)
		methodName := list at(2) asString
		argCount := list at(3) asString
		args := List clone
		i := 4

		while (list at(i) != nil,
			argType := list at(i) asString
			argValue := list at(i+1) asString
			a := self decode(argType, argValue)
			args append(a)
			i = i + 2
		)

		debugWriteln("performing ", methodName, " on a ", target type, "\n")
		result := target performWithArgList(methodName, args)
		b := Sequence clone
		b appendSeq("r", NULL)
		self encode(b, result, localObjects)
		ifDebug(write("returning result "); ShowMessage(b))
		socket write(b)
	)

	NULL := 0 asCharacter

	encode := method(b, arg,
		if(arg == nil, b appendSeq("nil", NULL, " ", NULL); return)

		if(arg type ==("Sequence") or(arg type == "Number"),
			b appendSeq(arg type, NULL, arg asString, NULL)
			return
		)

		if(arg type ==("Object") and(arg hasSlot("proxyId")),
			b appendSeq("LocalObject", NULL, arg proxyId, NULL)
			return
		)

		localObjects atPut(arg uniqueId asString, arg)
		debugWriteln("adding localObject ", arg uniqueId asString, " ", arg type, "\n")
		b appendSeq("RemoteObject", NULL, arg uniqueId asString, NULL)
	)

	decode := method(argType, argValue,
		if(argType == "nil", return nil)

		if(argType == "LocalObject",
			obj := localObjects at(argValue)
			if(obj == nil, write("DECODE ERROR: local obj ", argValue, " not found\n"))
			return obj
		)

		if(argType == "RemoteObject",
			proxy := proxies at(argValue)
			if(proxy == nil, proxy := DOProxy clone setProxyId(argValue) setConnection(self))
			return proxy
		)

		if(argType == "Number", return argValue asNumber)
		if(argType == "String", return argValue)
		if(argType == "Sequence", return argValue)
		write("ERROR: unknown decode type\n")
		return nil
	)

	ShowMessage := method(b,
		list := b splitNoEmpties(NULL)
		write("[")
		list foreach(i, v,
			write("'", v asString, "'")
			if(i != (list size - 1), write(", "))
		)
		write("]\n")
	)
)


DOProxy := Object clone do(
	docCategory("Networking")
	proxyId ::= nil
	connection ::= nil

	forward := method(
		methodName := call message name
		args := call message argsEvaluatedIn(call sender)

		b := Sequence clone
		b appendSeq("s", NULL)
		b appendSeq(self proxyId)
		b appendSeq(NULL)
		b appendSeq(methodName)
		b appendSeq(NULL)
		b appendSeq(call message argCount)
		b appendSeq(NULL)

		args foreach(v, connection encode(b, v))
		return(connection sendMessage(b))
	)
)
