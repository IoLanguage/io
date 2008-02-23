/*
//metadoc nil description nil is a singleton object that is used as a placeholder and to mean false in Io.")

nil do(
	//doc nil clone returns self since nil is a singleton.
	clone := nil

	//doc nil and(expression) Returns nil without evaluating expression.
	setSlot("and", nil)

	elseif := Object getSlot("if")

	//doc nil then(expression) Returns nil without evaluating expression.
	setSlot("then", nil)

	//doc nil else(expression) Returns nil without evaluating expression.
	setSlot("else", method(v, v))

	//doc nil or(anObject) Returns anObject if anObject is not nil. Otherwise returns nil.
	setSlot("or", method(v, if(v, v, nil)))

	//doc nil print Prints 'nil'. Returns self.
	print := method(write("nil"))

	setSlot("==", method(v, self isIdenticalTo(v)))
	setSlot("!=", method(v, self isIdenticalTo(v) not))

	//doc nil isNil Returns Lobby.
	isNil := Lobby

	//doc nil ifNil(expression) Evaluates message.
	ifNil := method(v, v)
)
*/

// if(a == 1) then(b) elseif(b == c) then(d) else(f)
// (a == 1) ifTrue(b) ifFalse(c)

true do(
	//doc true then Evaluates the argument and returns nil.
	then    := Object getSlot("evalArgAndReturnNil")

	//doc true elseif Does not eval argument and returns true.
	elseif  := true

	//doc true else Does not eval argument and returns true.
	else    := true

	//doc true ifTrue Evaluates the argument and returns self.
	ifTrue  := Object getSlot("evalArgAndReturnSelf")
	
	//doc true ifFalse Does not eval argument and returns true.
	ifFalse := true
	
	//doc true and Evaluates the argument and returns the result.
	setSlot("and", Object getSlot("evalArg"))
	
	//doc true or Does not eval argument and returns true.
	setSlot("or", true)

	//doc true asString Returns true.
	asString := "true"

	//doc true asSimpleString Returns true.
	asSimpleString := "true"

	//doc true not Does not eval argument and returns false.
	not := false
	
	//doc true clone Returns self.
	clone := true
)

false do(
	then    := false
	ifTrue  := false
	ifFalse := Object getSlot("evalArgAndReturnSelf")
	elseif  := Object getSlot("if")
	else    := Object getSlot("evalArgAndReturnNil")
	setSlot("and", false)
	setSlot("or", Object getSlot("evalArg"))

	type := "false"
	asString := "false"
	asSimpleString := "false"
	not := true
	clone := false
)

nil do(
	not := true
	isNil := true

	ifNonNil := Object getSlot("thisContext")
	ifNil := Object getSlot("evalArgAndReturnSelf")

	ifNilEval    := Object getSlot("evalArg")
	ifNonNilEval := Object getSlot("thisContext")

	type := "nil"
	asString := type
	asSimpleString := type

	setSlot("and", false)
	setSlot("or", Object getSlot("evalArg"))
	then := nil
	else := nil
	elseif := nil
	clone := nil
)

// I think non-local returns can eliminate all this stopStatus stuff 

Call do(
	relayStopStatus := method(
		ss := stopStatus(r := call evalArgAt(0))
		call sender call setStopStatus(ss)
		getSlot("r")
	)

	resetStopStatus := method(
		setStopStatus(Normal)
	)
)

Normal do(
	stopLooping := false
	isReturn := false
	isBreak := false
	isContinue := false

	return := method(arg,
		call setStopStatus(Return)
		getSlot("arg")
	)
)

Eol appendProto(Normal)
Continue appendProto(Normal) do(
	isContinue := true
)

Break appendProto(Normal) do(
	stopLooping := true
	isBreak := true
)

Return appendProto(Normal) do(
	stopLooping := true
	isReturn := true

	return := method(arg,
		call setStopStatus(Return)
		call sender call setStopStatus(Return)
		getSlot("arg")
	)
)

Object do(
	not := nil

	isNil := false

	ifNil := Object getSlot("thisContext")
	ifNonNil := Object getSlot("evalArgAndReturnSelf")

	ifNonNilEval := Object getSlot("evalArg")
	ifNilEval    := Object getSlot("thisContext")

	setSlot("and", Object getSlot("evalArg"))
	setSlot("or", true)
)

Sequence do(
	makeFirstCharacterLowercase := method(
		if(self size > 0, self atPut(0, self at(0) asLowercase))
	)

	makeFirstCharacterUppercase := method(
		if(self size > 0, self atPut(0, self at(0) asUppercase))
	)

	slicesBetween := method(startSeq, endSeq,
		chunks := List clone
		lastIndex := 0
		while (startIndex := self findSeq(startSeq, lastIndex),
			endIndex := self findSeq(endSeq, startIndex + startSeq size)
			endIndex ifNil(break)
			chunks append(self slice(startIndex + startSeq size, endIndex))
			lastIndex := endIndex + endSeq size
		)
		chunks
	)
)

Object do(
	hasSlot := method(n,
		getSlot("self") hasLocalSlot(n) or(getSlot("self") ancestorWithSlot(n) != nil)
	)

	//doc Object list(...) Returns a List containing the arguments.
	list := method(call message argsEvaluatedIn(call sender))

	//doc Object ..(arg) .. is an alias for: method(arg, self asString append(arg asString))
	setSlot("..", method(arg, getSlot("self") asString .. arg asString))

	Map addKeysAndValues := method(keys, values, keys foreach(i, k, self atPut(k, values at(i))); self)

	slotDescriptionMap := method(
		slotNames := getSlot("self") slotNames sort
		slotDescs := slotNames map(name, getSlot("self") getSlot(name) asSimpleString)
		Map clone addKeysAndValues(slotNames, slotDescs)
	)

	apropos := method(keyword,
		Protos Core foreachSlot(name, p,
			slotDescriptions := getSlot("p") slotDescriptionMap ?select(k, v, k asMutable lowercase containsSeq(keyword))

			if(slotDescriptions and slotDescriptions size > 0,
				s := Sequence clone
				slotDescriptions keys sortInPlace foreach(k,
					s appendSeq("  ", k alignLeft(16), " = ", slotDescriptions at(k), "\n")
				)

				writeln(name)
				writeln(s)
			)
		)
		nil
	)

	slotSummary := method(keyword,
		if(getSlot("self") type == "Block",
			return getSlot("self") asSimpleString
		)
		
		s := Sequence clone
		s appendSeq(" ", getSlot("self") asSimpleString, ":\n")
		slotDescriptions := slotDescriptionMap
		
		if(keyword,
			slotDescriptions = slotDescriptions select(k, v, k asMutable lowercase containsSeq(keyword))
		)
		
		slotDescriptions keys sortInPlace foreach(k,
			s appendSeq("  ", k alignLeft(16), " = ", slotDescriptions at(k), "\n")
		)
		s
	)

	asString := getSlot("slotSummary")

	asSimpleString := method(getSlot("self") type .. "_" .. getSlot("self") uniqueHexId)

	/*doc Object newSlot(slotName, aValue)
	Creates a getter and setter for the slot with the name slotName
	and sets it's default the value aValue. Returns self. For example,
	newSlot("foo", 1) would create slot named foo with the value 1 as well as a setter method setFoo().
	*/

	newSlot := method(name, value, doc,
		getSlot("self") setSlot(name, getSlot("value"))
		getSlot("self") setSlot("set" .. name asCapitalized,
			doString("method(" .. name .. " = call evalArgAt(0); self)"))
			//if(doc, getSlot("self") docSlot(name, doc))
		value
	)

	//doc Object launchFile(pathString) Eval file at pathString as if from the command line in it's folder.

	launchFile := method(path, args,
		args ifNil(args = List clone)
		System launchPath :=  path pathComponent
		Directory setCurrentWorkingDirectory(System launchPath)
		System launchScript = path
		self doFile(path)
	)

	//doc Object println Same as print, but also prints a new line. Returns self.
	println := method(getSlot("self") print; write("\n"); self)

	/*doc Object ?(aMessage)
	description: Sends the message aMessage to the receiver if it can respond to it. Example:
	<code>
	MyObject test // performs test
	MyObject ?test // performs test if MyObject has a slot named test
	</code>
	The search for the slot only follows the receivers proto chain.
	*/

	setSlot("?",
		method(
			m := call argAt(0)
			if (self getSlot(m name) != nil,
				call relayStopStatus(m doInContext(self, call sender))
			,
				nil
			)
		)
	)

	//doc Object ancestors Returns a list of all of the receiver's ancestors as found by recursively following the protos links.

	ancestors := method(a,
		if(a, if(a detect(x, x isIdenticalTo(self)), return a), a = List clone)
		a append(self)
		self protos foreach(ancestors(a))
		a
	)

	//doc Object isKindOf(anObject) Returns true if anObject is in the receiver's ancestors.

	isKindOf := method(anObject, getSlot("self") ancestors contains(getSlot("anObject")))

	/*doc Object super(aMessage)
	Sends the message aMessage to the receiver's proto with the context of self. Example:
	<code>
	self test(1, 2)   // performs test(1, 2) on self
	super(test(1, 2)) // performs test(1, 2) on self proto but with the context of self
	</code>
	*/

	setSlot("super", method(
		senderSlotContext := call sender call slotContext
		m := call argAt(0)
		m ifNil(Exception raise("Object super requires an argument"))
		senderSlotContext ifNil(Exception raise("Object super called outside of block context"))
		slotName := m name
		ancestor := senderSlotContext ancestorWithSlot(slotName)
		if(ancestor == nil,
			slotName = "forward"
			ancestor = senderSlotContext ancestorWithSlot(slotName)
		)
		if(ancestor isIdenticalTo(senderSlotContext), Exception raise("Object super slot " .. slotName .. " not found"))
		b := ancestor getSlot(slotName)
		if(getSlot("b") isActivatable == false,
			b
		,
			getSlot("b") performOn(call sender call target, call sender, m, ancestor)
		)
	))

	/*doc Object resend
	Send the message used to activate the current method to the Object's proto.
	For example;
	<code>
	Dog := Mammal clone do(
	init := method(
		resend
	)
	)
	</code>
	calling Dog init will send an init method to Mammal, but using the Dog's context.
	*/

	setSlot("resend", method(
		senderSlotContext := call sender call slotContext
		senderSlotContext ifNil(Exception raise("Object resend called outside of block context"))
		m := call sender call message
		slotName := m name
		ancestor := senderSlotContext ancestorWithSlot(slotName)

		if(ancestor isIdenticalTo(nil),
			slotName = "forward"
			ancestor = senderSlotContext ancestorWithSlot(slotName)
		)

		if(ancestor isIdenticalTo(senderSlotContext),
			Exception raise("Object resend slot " .. slotName .. " not found")
		)

		b := ancestor getSlot(slotName)
		if(getSlot("b") != nil,
			getSlot("b") performOn(call sender getSlot("self"), call sender call sender, m, ancestor)
		,
			getSlot("b")
		)
	))


	//doc Object list(...) Returns a List containing the arguments.
	list := method(call message argsEvaluatedIn(call sender))

	Object print := method(write(getSlot("self") asString); getSlot("self"))

	//doc Object println Same as print, but also prints a new line. Returns self.
	println := method(getSlot("self") print; write("\n"); getSlot("self"))

	//doc Object in(aList) Same as: aList contains(self)
	in := method(aList, aList contains(self))

	uniqueHexId := method("0x" .. getSlot("self") uniqueId asString toBase(16))

	lazySlot := method(
		if(call argCount == 1,
			m := method(
				self setSlot(call message name, nil)
			)

			args := getSlot("m") message next arguments
			args atPut(1, call argAt(0) clone)
			getSlot("m") message next setArguments(args)

			getSlot("m") clone
		,
			name := call evalArgAt(0)
			m := ("self setSlot(\"" .. name .. "\", " .. call argAt(1) code .. ")") asMessage
			self setSlot(name, method() setMessage(m))
			nil
		)
	)

	foreachSlot := method(
		self slotNames sort foreach(n,
			call sender setSlot(call message argAt(0) name, n)
			call sender setSlot(call message argAt(1) name, getSlot("self") getSlot(n))
			r := call relayStopStatus(call evalArgAt(2))
			if(call stopStatus isReturn, return getSlot("r"))
			if(call stopStatus stopLooping,
				call resetStopStatus
				break
			)
		)
	)

	/*doc Object switch(<key1>, <expression1>, <key2>, <expression2>, ...) 
	Execute an expression depending on the value of the caller. (This is an equivalent to C switch/case)
	<code>
	hour := Date hour switch(
		12, "midday",
		0, "midnight",
		17, "teatime",
		Date hour asString
	)
	</code>
	*/
	
	switch := method(
		for(couple, 0, call argCount - 2, 2,
			if(call evalArgAt(couple) == self,
				return call relayStopStatus(call evalArgAt(couple + 1))
			)
		)
		if(call argCount isOdd,
			call relayStopStatus(call evalArgAt(call argCount - 1))
		,
			nil
		)
	)

	//doc Object isLaunchScript Returns true if the current file was run on the command line. Io's version of Python's __file__ == "__main__"
	isLaunchScript := method(
		call message label == System launchScript
	)

	/*doc Object doRelativeFile(pathString)
		Evaluates the File in the context of the receiver. Returns the result. 
		pathString is relative to the file calling doRelativeFile. (Duplicate of relativeDoFile)
	*/
	doRelativeFile := method(path,
		self doFile(Path with(call message label pathComponent, path))
	)

	/*doc Object relativeDoFile(pathString)
		Evaluates the File in the context of the receiver. Returns the result. 
		pathString is relative to the file calling doRelativeFile. (Duplicate of doRelativeFile)
	*/
	relativeDoFile := getSlot("doRelativeFile")
	
	/*doc Object deprecatedWarning(optionalNewName) 
	Prints a warning message that the current method is deprecated.
	If optionalNewName is supplied, the warning will suggest using that instead.
	Returns self.
	*/
	deprecatedWarning := method(newName,
		writeln("Warning: ", sender call message name, " is deprecated. ", if(name, "use " .. newName .. " instead.", ""))
		self
	)
)
