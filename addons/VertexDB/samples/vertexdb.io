prependProto(VertexDB)

VertexDB Settings setHost("localhost") setPort(8080)

///*
//Request debugOn

writeln(Node with("/foo") asQueue path)
System exit

foo := Node with("/foo") mkdir
3 repeat(i,
	foo atWrite("_" .. i asString, "The key is _" .. i asString)
)


bar := Node with("/bar") mkdir
baz := bar nodeAt("baz")
foo linkTo(baz path)

baz keys foreach(k,
	writeln(k, ": ", baz read(k))
)

//*/

//writeln(Server clone shutdown)