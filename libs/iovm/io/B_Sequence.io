
Vector := Sequence clone setItemType("float32") setEncoding("number")
vector := method(v := Vector clone; call evalArgs foreach(n, v append(n)); v)


/*
Object moveSlotToObject := method(slotName, target,
	v := self getSlot(slotName)
	self removeSlot(slotName)
	target setSlot(slotName, getSlot("v"))
	writeln(self type, " removeSlot(\"", slotName, "\")")
)


list("*", "*=", "+", "+=", "-", "-=", "..", "/", "/=", "<", 
"<=", ">", ">=", "Max", "Min", "abs", "acos", "addEquals", "asin", 
"bitAt", "bitCount", "bitwiseAnd", "bitwiseNot", "bitwiseOr", "bitwiseXor", "byteAt", "ceil", 
"cos", "cosh", "distanceTo", "dotProduct", "duplicateIndexes", "isZero", 
"log", "log10", "logicalOr", "max", "mean", "meanSquare", "min", "negate", "normalize",  
"product", "rangeFill", "removeEvenIndexes", "removeOddIndexes", "rootMeanSquare", 
"setX", "setY", "setZ", "sin", "sinh", "sort", "sqrt", "square", "sum", 
"tan", "tanh", "toBase", "translate", "x", "y", "z", "zero") foreach(slotName, Sequence moveSlotToObject(slotName, Vector))

Sequence appendProto(Vector)
*/

Lobby Protos Core do(
	ImmutableSequence := ""
	String := ImmutableSequence
)

Sequence do(
	validEncodings := "ascii utf8 ucs2 ucs4 number" split
	validItemTypes := "uint8 uint16 uint32 uint64 int8 int16 int32 int64 float32 float64" split
	setSlot("..", method(arg, self asString cloneAppendSeq(arg asString)))

	//doc Sequence repeated(n) Returns a new sequence containing the receiver repeated n number of times.
	repeated := method(n,
		s := Sequence clone
		n repeat(s appendSeq(self))
		s
	)

	//doc Sequence alignLeftInPlace(width, [padding]) Same as align left but operation is performed on the receiver.
	alignLeftInPlace := method(width, padding,
		originalSize := size
		if(padding isNil or padding size == 0,
			padding = " "
		)
		((width - size) / padding size) ceil repeat(appendSeq(padding))
		setSize(width max(originalSize))
	)

	/*doc Sequence alignLeft(width, [padding]) 
	Example:
	<p>
	<code>
	Io> "abc" alignLeft(10, "+")
	==> abc+++++++
	Io> "abc" alignLeft(10, "-=")
	==> abc-=-=-=-
	</code>
	*/
	
	alignLeft := method(width, padding, asMutable alignLeftInPlace(width, padding))

	/*doc Sequence alignRight(width, [padding]) 
	Example:
	<p>
	<code>
	Io> "abc" alignRight(10, "-")
	==> -------abc
	Io> "abc" alignRight(10, "-=")
	==> -=-=-=-abc
	</code>
	*/
	
	alignRight := method(width, padding,
		Sequence clone alignLeftInPlace(width - size, padding) appendSeq(self)
	)

	/*doc Sequence alignCenter(width, [padding]) ""
	Example:
	<code>
	Io> "abc" alignCenter(10, "-")
	==> ---abc----
	Io> "abc" alignCenter(10, "-=")
	==> -=-abc-=-=
	</code>
	*/
	
	alignCenter := method(width, padding,
		alignRight(((size + width) / 2) floor, padding) alignLeftInPlace(width, padding)
	)

	asSimpleString := method("\"" .. self asString asMutable escape .. "\"")

	/*doc Sequence splitNoEmpties(optionalArg1, optionalArg2, ...)
		Returns a list containing the non-empty sub-sequences of the receiver divided by the given arguments.
		If no arguments are given the sequence is split on white space.
		Examples:
		<code>
		"a   b  c d" splitNoEmpties => list("a", "b", "c", "d")
		"a***b**c*d" splitNoEmpties("*") => list("a", "b", "c", "d")
		"a***b||c,d" splitNoEmpties("*", "|", ",") => list("a", "b", "c", "d")
		</code>
	*/

	/*doc Sequence split Returns a list containing the sub-sequences of the receiver divided by the given arguments.
		If no arguments are given the sequence is split on white space.
	*/
		
	splitNoEmpties := method(
		self performWithArgList("split", call evalArgs) selectInPlace(size != 0)
	)

	//doc Sequence findNthSeq(aSequence, n) Returns a number with the nth occurrence of aSequence.
	findNthSeq := method(str, n,
		num := self findSeq(str)
		if(num isNil, return nil)
		if(n == 1, return num)
		num + self exclusiveSlice(num + 1, self size) findNthSeq(str, n - 1)
	)


	orderedSplit := method(
			separators := call evalArgs
			if(separators size == 0, return list(self))
			i := 0
			skipped := 0
			r := list
			separators foreach(separator,
				j := findSeq(separator, i) ifNil(
					skipped = skipped + 1
					continue
				)
				r append(exclusiveSlice(i, j))
				if(skipped > 0,
					skipped repeat(r append(nil))
					skipped = 0
				)
				i = j + separator size
			)
			if(size == 0,
				r append(nil)
			,
				r append(exclusiveSlice(i))
			)
			skipped repeat(r append(nil))
			r
	)

	//doc Sequence prependSeq(object1, object2, ...) Prepends given objects asString in reverse order to the receiver.  Returns self.
	prependSeq := method(self atInsertSeq(0, call evalArgs join); self)

	//doc Sequence itemCopy Returns a new sequence containing the items from the receiver.
	itemCopy := method(Sequence clone copy(self))

	sequenceSets := Map clone do(
		atPut("lowercaseSequence",
			lst := list
			"abcdefghijklmnopqrstuvwxyz" foreach(v, lst append(v))
			lst
		)
		atPut("uppercaseSequence",
			lst := list
			at("lowercaseSequence") foreach(v, lst append(v asUppercase))
			lst
		)
		atPut("digitSequence",
			lst := list
			"0123456789" foreach(v, lst append(v))
			lst
		)
		removeSlot("lst")
		removeSlot("v")
	)
	
	//doc Sequence reverse Reverses the ordering of all the items of the receiver. Returns copy of receiver.
	reverse := method(self itemCopy reverseInPlace)

	//doc Sequence asHex Returns a hex string for the receiving sequence, e.g., \"abc\" asHex -> \"616263\".
	asHex := method(
		r := Sequence clone
		self foreach(c, r appendSeq(c asHex))
		r
	)

	//doc Sequence print Prints contents of a sequence.
	if(getSlot("cPrint") == nil,
		cPrint := getSlot("print")
		print := method(
			self asUTF8 cPrint
		)
	)
	
	asDecodedList := method(List fromEncodedList(self))
	
	//doc Sequence slice Deprecated method. Use exSlice instead.
	slice := method(start,
		deprecatedWarning("exSlice")
		performWithArgList("exSlice", call evalArgs)
	)
	
	//doc Sequence asJson Converts to form that could be interpreted as json if it already contains json, e.g. {"aaa":"bbb"} --> "{\"aaa\":\"bbb\"}"
	asJson := method(
		//TODO Return unicode string with hex replacements as per http://www.json.org/
		replacementMap := Map clone\
			atPut("\"", "\\\"")\
			atPut("/", "\\/")\
			atPut("\b", "\\b")\
			atPut("\f", "\\f")\
			atPut("\n", "\\n")\
			atPut("\t", "\\t")
			
		
		"\"" .. self asMutable replaceSeq("\\", "\\\\") replaceMap(replacementMap) .. "\""
	)

	fileName := method(
		if(self isEmpty, return(self))
		lastPathComponent := self split("/") last
		parts := lastPathComponent split(".") 
		if(parts size > 1,
			parts removeLast
		)
		parts join(".")
	)
)
