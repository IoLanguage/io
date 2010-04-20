//doc Object NullCharacter A sequence with a single zero byte.
Object NullCharacter := Sequence clone
Object NullCharacter append(0x0)

DOConnection := Object clone do(
//metadoc DOConnection category Networking

/*metadoc DOConnection description 
The DOConnection object is useful for communicating with remote servers in a way that makes it look just like the sending of local messages. Proxies are automatically created on either side for passed objects, with the exception of strings and numbers, which are passed by value. Example:
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
<p>

The format of the Distributed Objects message is a list of NullCharacter
terminated strings in one of these two formats:
<p>
Send message format:

<pre>
s NullCharacter targetId NullCharacter messageName NullCharacter argCount NullCharacter argType NullCharacter argValue NullCharacter (next arg type and value, etc)
</pre>

Reply message format:

<pre>
r NullCharacter argType NullCharacter argvalue NullCharacter
</pre>

If the argument is not a String, Number or nil then:
If it is local to the sender, the type is RemoteObject.
If it is a proxy to a remote object, the type is LocalObject.

This isn't optimized yet.
*/

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
		while(socket isOpen,
			self readMessage ifError(e,
				debugWriteln("Error reading message: ", e description)
				socket close
			)
		)
		debugWriteln("Closed connection")
	)

	//doc DOConnection close Close the connection, if it is open. Returns self.

	close := method(
		socket close
		self
	)

	//doc DOConnection setHost(ipString) Sets the host ip to connect to. Returns self or an Error, if one occurs.
	//doc DOConnection host Returns the host ip.

	setHost := method(aString, socket setHost(aString) returnIfError; self)
	host := method(socket host)

	//doc DOConnection setPort(portNumber) Sets the port number to connect to. Returns self.
	//doc DOConnection port Returns the port.

	setPort := method(aString, socket setPort(aString); self)
	port := method(socket port)

	//doc DOConnection connect Connect to the remote DOServer. Returns self or an Error, if one occurs.

	connect := method(
		socket connect returnIfError
		self serverObject := DOProxy clone setProxyId(0) setConnection(self)
		self
	)

	/*doc DOConnection serverObject
	A handle to the remote DOServer's root object. Returns result from server or an Error, if one occurs.
	*/

	sendMessage := method(m,
		ifDebug(write("sending message "); ShowMessage(m))
		socket streamWrite(m) returnIfError
		debugWriteln("waiting for result")
		socket readBuffer empty
		result := nil
		loop(
			socket read returnIfError
			list := socket readBuffer splitNoEmpties(NullCharacter)
			ifDebug(write("got result "); ShowMessage(socket readBuffer))
			type := list at(0) asString

			if(type == "r") then( /* if it's a result */
				argType  := list at(1) asString
				argValue := list at(2) asString
				result := self decode(argType, argValue)
				break
			) else(
				performMessage(m) returnIfError
			)
		)
		return result
	)

	readMessage := method(
		socket readBuffer empty
		socket read returnIfError
		m := socket readBuffer
		ifDebug(write("read message "); ShowMessage(m))
		performMessage(m)
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
		b appendSeq("r", NullCharacter)
		self encode(b, result, localObjects)
		ifDebug(write("returning result "); ShowMessage(b))
		socket appendToWriteBuffer(b) writeFromBuffer
	)

	NullCharacter := 0 asCharacter
	
	encode := method(b, arg,
		if(arg == nil, b appendSeq("nil", NullCharacter, " ", NullCharacter); return)

		if(arg type ==("Sequence") or(arg type == "Number"),
			b appendSeq(arg type, NullCharacter, arg asString, NullCharacter)
			return
		)

		if(arg type ==("Object") and(arg hasSlot("proxyId")),
			b appendSeq("LocalObject", NullCharacter, arg proxyId, NullCharacter)
			return
		)

		localObjects atPut(arg uniqueId asString, arg)
		debugWriteln("adding localObject ", arg uniqueId asString, " ", arg type, "\n")
		//b appendSeq("RemoteObject", NullCharacter, arg uniqueId asString, NullCharacter)
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
		list := b splitNoEmpties(NullCharacter)
		write("[")
		list foreach(i, v,
			write("'", v asString, "'")
			if(i != (list size - 1), write(", "))
		)
		write("]\n")
	)
)

DOProxy := Object clone do(
	//metadoc DOProxy category Networking
	proxyId ::= nil
	connection ::= nil

	forward := method(
		methodName := call message name
		args := call message argsEvaluatedIn(call sender)

		b := Sequence clone
		b appendSeq("s", NullCharacter)
		b appendSeq(self proxyId)
		b appendSeq(NullCharacter)
		b appendSeq(methodName)
		b appendSeq(NullCharacter)
		b appendSeq(call message argCount)
		b appendSeq(NullCharacter)

		args foreach(v, connection encode(b, v))
		connection sendMessage(b)
	)
)
