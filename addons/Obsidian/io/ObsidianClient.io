
ObsidianClient := Object clone do(
	//metadoc ObsidianServer description 
	init := method(
		self socket := Socket clone setHost("localhost") setPort(8000)
	)
	
	open := method(
		debugWriteln("connecting")
		socket connect
		if(socket isOpen not, Exception raise("connect failed"))
		debugWriteln("connected")
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
	
	first := method(objId, count,
		incrementRequestId
		socket writeListMessage(list(rid, "first", objId, count))
		response := socket readListMessage
		response slice(1)
	)
	
	last := method(objId, count,
		incrementRequestId
		socket writeListMessage(list(rid, "last", objId, count))
		response := socket readListMessage
		response slice(1)
	)
	
	after := method(objId, key, count,
		incrementRequestId
		socket writeListMessage(list(rid, "after", objId, key, count))
		response := socket readListMessage
		response slice(1)
	)

	before := method(objId, key, count,
		incrementRequestId
		socket writeListMessage(list(rid, "before", objId, key, count))
		response := socket readListMessage
		response slice(1)
	)
)

/*
writeln("begin")
client := ObsidianClient clone open
client onAtPut("1", "aKey", "aSlot")
client onAtPut("1", "cKey", "cSlot")
client onAtPut("1", "bKey", "bSlot")
client first("1", 5) println
a := client onAt("1", "aKey")
writeln("a = ", a)
b := client onAt("1", "bKey")
writeln("b = ", b)
client close
*/


