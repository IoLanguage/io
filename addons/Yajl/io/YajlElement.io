Yajl do(
	init := method(
		self stack := List clone
		self mapKey := nil
		self root := List clone
		push(root)
	)

	push := method(v,
		stack append(v)
	)

	pop := method(
		stack removeLast
	)

	addValue := method(v,
		//writeln("'", v, "'")
		if(mapKey,
			//writeln(stack last uniqueId, " atPut ", mapKey, ", ", v) 
			stack last atPut(mapKey, v)
			mapKey = nil
		,
			stack last append(v)
		)
	)

	indent := method(
		"\t" repeated(stack size)
	)

	startArray := method(
		//write("[");
		v := List clone
		addValue(v)
		push(v)
	)

	endArray := method(
		//writeln(indent, "]")
		pop
	)

	startMap := method(
		//writeln("{");
		v := Map clone
		addValue(v)
		push(v)
	)

	endMap := method(
		//writeln(indent, "}")
		pop
	)

	addMapKey := method(k,
		//write(indent, "", k, ": ")
		mapKey = k
	)
	
	cleanSeq := method(seq,
		seq = seq asMutable
		32 repeat(i,
			seq removeSeq(Sequence clone atPut(0, i))
		)
		seq
	)
	
	parseJson := method(json,
		Yajl clone parse(cleanSeq(json)) root first
	)
)

Sequence asJson := method("\"" .. self .. "\"")
Number asJson := method(self asString)
true asJson := method("true")
false asJson := method("false")
nil asJson := method("null")

Map asJson := method(
	s := Sequence clone
	s appendSeq("{")
	self keys foreach(k,
		s appendSeq(k asJson, ":", self at(k) asJson)
	)
	s appendSeq("}")
	s
)

List asJson := method(
	s := Sequence clone
	s appendSeq("[")
	s appendSeq(self map(asJson) join(", "))
	s appendSeq("]")
	s
)



