
ObsidianClient := Object clone do(
	//metadoc ObsidianServer description 
	init := method(
		self socket := Socket clone setHost("localhost") setPort(8000)
	)
	
	open := method(
		writeln("connecting")
		socket connect
		if(socket isOpen not, Exception raise("connect failed"))
		writeln("connected")
		self
	)
	
	close := method(
		socket close
		self
	)
	
	rid := 0
	
	incrementRequestId := method(rid = rid + 1)
	
	onAtPut := method(objId, k, v, 
		incrementRequestId
		socket writeListMessage(list(rid, "set", objId, k, v))
		response := socket readListMessage
		self
    )

	onAt := method(objId, k,
		incrementRequestId
		socket writeListMessage(list(rid, "get", objId, k))
		response := socket readListMessage
		response at(1)
	)
	
	onRemoveAt := method(objId, k,
		incrementRequestId
		socket writeListMessage(list(rid, "remove", objId, k))
		response := socket readListMessage
		self		
	)
)

writeln("begin")
client := ObsidianClient clone open
client onAtPut("1", "aKey", "aSlot")
client onAtPut("1", "bKey", "bSlot")
a := client onAt("1", "aKey")
writeln("a = ", a)
b := client onAt("1", "bKey")
writeln("b = ", b)
client close



