
ObsidianChannel := Object clone do(
	db ::= nil
	
	//debugWriteln := getSlot("writeln")
	
	sendList := method(aList,
		debugWriteln("ObsidianChannel ", socket descriptorId, " send ", aList)
		socket writeMessage(aList asEncodedList)
	)
	
	handleSocket := method(aSocket,
		self socket := aSocket
		while(aSocket isOpen,
		 	request := List fromEncodedList(aSocket readMessage)
			debugWriteln("ObsidianChannel ", socket descriptorId, " recv ", request)
			rid   := request at(0)
			rtype := request at(1)
			objId := request at(2)
			arg1  := request at(3)
			arg2  := request at(4)
			if(rtype == "set") then(
				db transactionalAtPut(objId .. "/" .. arg1, arg2)
				sendList(list(rid))
			) elseif(rtype == "get") then(
				sendList(list(rid, db at(objId .. "/" .. arg1)))
			) elseif(rtype == "remove") then(
				db transactionalRemoveAt(objId .. "/" .. arg1)
				sendList(list(rid))		
			) elseif(rtype == "first") then(
				c := db prefixCursor setPrefix(objId asString)
				c first
				keys := list()
				arg1 asNumber repeat(
					k := c key
					if(k == nil, break)
					keys append(k)
					c next
				)
				c close
				sendList(list(rid) appendSeq(keys))				
			) elseif(rtype == "last") then(
				c := db prefixCursor setPrefix(objId)
				c last
				keys := list()
				arg1 asNumber repeat(
					k := c key
					if(k == nil, break)
					keys append(k)
					c previous
				)
				c close
				sendList(list(rid) appendSeq(keys))				
			) elseif(rtype == "after") then(
				c := db prefixCursor setPrefix(objId)
				c goto(arg1)
				keys := list()
				arg2 asNumber repeat(
					k := c key
					if(k == nil, break)
					keys append(k)
					c next
				)
				c close
				sendList(list(rid) appendSeq(keys))		
			) elseif(rtype == "before") then(
				c := db prefixCursor setPrefix(objId)
				c goto(arg1)
				keys := list()
				arg2 asNumber repeat(
					k := c key
					if(k == nil, break)
					keys append(k)
					c previous
				)
				c close
				sendList(list(rid) appendSeq(keys))			
			)

			yield
		)
	)
)

ObsidianServer := Server clone do(
	//metadoc ObsidianServer description 
	init := method(
		self setHost("localhost") setPort(8000)
		//self setHost("localhost")
		self db := TokyoCabinet clone open("test.tc")
	)
	
 	//debugWriteln := getSlot("writeln")
	handleSocket := method(aSocket, 
		debugWriteln("ObsidianServer got connection")
        ObsidianChannel clone setDb(db) @handleSocket(aSocket)
    )
)

//ObsidianServer clone start