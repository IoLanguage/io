
nil   justSerialized := method(stream, stream write("nil"))
true  justSerialized := method(stream, stream write("true"))
false justSerialized := method(stream, stream write("false"))

Number justSerialized := method(stream,
	stream write(self asSimpleString)
)

Sequence justSerialized := method(stream,
	stream write(self asMutable escape asSimpleString)
)

List justSerialized := method(stream,
	stream write("list(")
	stream write(self map(serialized) join(", "))
	stream write(");")
)

Map justSerialized := method(stream,
	stream write("Map clone do(")
	self foreach(k, v,
		stream write("atPut(" .. k serialized .. ", " .. getSlot("v") serialized .. ");")
	)
	stream write(");")
)

//doc Block justSerialized(stream) Write's the receivers code into the stream.


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
	serialized := method(stream,
		if(stream == nil, stream := SerializationStream clone)
		justSerialized(stream)
		stream output
	)

	justSerialized := method(stream,
		stream write(getSlot("self") type, " clone do(\n")
		self serializedSlots(stream)
		stream write(")\n")
	)

	serializedSlots := method(stream,
		self serializedSlotsWithNames(self slotNames, stream)
	)

	serializedSlotsWithNames := method(names, stream,
		names foreach(slotName,
			stream write("\t", slotName, " := ")
			self getSlot(slotName) serialized(stream)
			stream write("\n")
		)
	)
)
