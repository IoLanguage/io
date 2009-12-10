
//doc nil justSerialized(stream) Writes the receiver's code into the stream.
//doc true justSerialized(stream) Writes the receiver's code into the stream.
//doc false justSerialized(stream) Writes the receiver's code into the stream.

nil   justSerialized := method(stream, stream write("nil"))
true  justSerialized := method(stream, stream write("true"))
false justSerialized := method(stream, stream write("false"))

//doc Number justSerialized(stream) Writes the receiver's code into the stream.
Number justSerialized := method(stream,
	stream write(self asSimpleString)
)

//doc Sequence justSerialized(stream) Writes the receiver's code into the stream.
Sequence justSerialized := method(stream,
	stream write(self asMutable escape asSimpleString)
)

//doc List justSerialized(stream) Writes the receiver's code into the stream.
List justSerialized := method(stream,
	stream write("list(")
	stream write(self map(serialized) join(", "))
	stream write(");")
)

//doc Map justSerialized(stream) Writes the receiver's code into the stream.
Map justSerialized := method(stream,
	stream write("Map clone do(")
	self foreach(k, v,
		stream write("atPut(" .. k serialized .. ", " .. getSlot("v") serialized .. ");")
	)
	stream write(");")
)

//doc Block justSerialized(stream) Writes the receiver's code into the stream.
Block justSerialized := method(stream,
		stream write(code, "\n")
)

SerializationStream := Object clone do(
	init := method(
		self seen := Map clone
		self output := Sequence clone
	)

	write := method(
		for(i, 0, call argCount - 1,
			//super(writeln(i, call argAt(i)))
			output appendSeq(call evalArgAt(i))
		)
	)
)

Object do(
  /*doc Object serialized
  Returns a serialized representation of the receiver.
  <br/>
  <pre>
  Io> Object clone do(x:=1) serialized
  ==> Object clone do(
  	x := 1
  )
  </pre>
  */
	serialized := method(stream,
		if(stream == nil, stream := SerializationStream clone)
		justSerialized(stream)
		stream output
	)
  
  /*doc Object justSerialized(stream)
  Writes serialized representation to a SerializationStream. Returns stream contents. 
  [This is unintended side effect! Returned value may change in the future.]
  */
	justSerialized := method(stream,
		stream write(
			if(getSlot("self") hasLocalSlot("type"),
				getSlot("self") proto type
			,
				getSlot("self") type
			)
		, " clone do(\n")
		self serializedSlots(stream)
		stream write(")\n")
	)

  //doc Object serializedSlots(stream) Writes all slots to a stream.
	serializedSlots := method(stream,
		self serializedSlotsWithNames(self slotNames, stream)
	)

  //doc Object serializedSlotsWithNames(names, stream) Writes selected slots to a stream.
	serializedSlotsWithNames := method(names, stream,
		names foreach(slotName,
			stream write("\t", slotName, " := ")
			self getSlot(slotName) serialized(stream)
			stream write("\n")
		)
	)
)
