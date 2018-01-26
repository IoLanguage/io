/*
//metadoc nil description nil is a singleton object that is used as a placeholder and to mean false in Io.

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
	
	//doc true or Does not eval argument and returns true.
	setSlot("or", true)

	//doc true asString Returns true.
	asString := "true"

	//doc true asSimpleString Returns true.
	asSimpleString := "true"

	//doc true not Does not eval argument and returns false.
	not := false
	
	//doc true clone Returns true.
	clone := true
)

false do(
  //doc false then Returns false.
	then    := false
	//doc false ifTrue Returns false.
	ifTrue  := false
	//doc false ifFalse Evaluates the argument and returns self.
	ifFalse := Object getSlot("evalArgAndReturnSelf")
	//doc false elseif Same as <tt>if</tt>.
	elseif  := Object getSlot("if")
	//doc false else Evaluates the argument and returns nil.
	else    := Object getSlot("evalArgAndReturnNil")
	//doc false and Returns false.
	setSlot("and", false)
	//doc false or Evaluates the argument and returns the result.
	or := method(v, v isTrue)
  
  //doc false type Returns "false".
	type := "false"
	//doc false asString Returns "false".
	asString := "false"
	//doc false asSimpleString Returns "false".
	asSimpleString := "false"
	//doc false not Returns true.
	not := true
	//doc false clone Returns self.
	clone := false
	isTrue := false
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
	or := method(v, v isTrue)
	then := nil
	else := nil
	elseif := nil
	clone := nil
	isTrue := false
)

// I think non-local returns can eliminate all this stopStatus stuff 

Call do(
  /*doc Call relayStopStatus(arg) 
  Sets sender's stop status (Normal, Return, 
  Break, Continue etc.) and returns evaluated argument.
  */
	relayStopStatus := method(
		ss := stopStatus(r := call evalArgAt(0))
		call sender call setStopStatus(ss)
		getSlot("r")
	)

  /*doc Call resetStopStatus(arg) 
  Sets stop status to Normal.
  See also <tt>Call setStopStatus</tt>.
  */
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
	//doc Object not Returns nil.
	not := nil
	//doc Object isNil Returns false.
	isNil := false
	//doc Object ifNil(arg) Does nothing, returns self.
	ifNil := Object getSlot("thisContext")
	//doc Object ifNonNil(arg) Evaluates argument and returns self.
	ifNonNil := Object getSlot("evalArgAndReturnSelf")
	//doc Object ifNonNilEval(arg) Evaluates argument and returns the result.
	ifNonNilEval := Object getSlot("evalArg")
	//doc Object ifNilEval(arg) Does nothing, returns self.
	ifNilEval    := Object getSlot("thisContext")
	//doc Object or(arg) Returns true.
	setSlot("or", true)
	
	//doc Object isTrue Returns true.
	isTrue := true
	//doc Object and(arg) Evaluates argument and returns the result.
	and := method(v, v isTrue)
)

Sequence do(
  /*doc Sequence makeFirstCharacterLowercase 
  Receiver must be mutable (see also asMutable). Returns receiver.
  <br/>
  <pre>
  Io> "ABC" asMutable makeFirstCharacterLowercase
  ==> aBC
  </pre>
  */
	makeFirstCharacterLowercase := method(
		if(self size > 0, self atPut(0, self at(0) asLowercase))
	)
  /*doc Sequence makeFirstCharacterUppercase 
  Receiver must be mutable (see also asMutable). Returns receiver.
  <br/>
  <pre>
  Io> "abc" asMutable makeFirstCharacterUppercase
  ==> Abc
  </pre>
  */
	makeFirstCharacterUppercase := method(
		if(self size > 0, self atPut(0, self at(0) asUppercase))
	)

  /*doc Sequence slicesBetween(startSeq, endSeq) 
  Returns a list of slices delimited 
  by <tt>startSeq</tt> and <tt>endSeq</tt>.
  <br>
  <pre>
  Io> "<a><b></b></a>" slicesBetween("<", ">")
  ==> list("a", "b", "/b", "/a")
  </pre>
  */
	slicesBetween := method(startSeq, endSeq,
		chunks := List clone
		lastIndex := 0
		while (startIndex := self findSeq(startSeq, lastIndex),
			endIndex := self findSeq(endSeq, startIndex + startSeq size)
			endIndex ifNil(break)
			chunks append(self exclusiveSlice(startIndex + startSeq size, endIndex))
			lastIndex := endIndex + endSeq size
		)
		chunks
	)
)

Object do(
  /*doc Object hasSlot(name) 
  Returns <tt>true</tt> if slot is found somewhere in the inheritance chain 
  (including receiver itself).
  */
	hasSlot := method(n,
		getSlot("self") hasLocalSlot(n) or(getSlot("self") ancestorWithSlot(n) != nil)
	)

	//doc Object list(...) Returns a List containing the arguments.
	list := method(call message argsEvaluatedIn(call sender))

	//doc Object ..(arg) .. is an alias for: method(arg, self asString append(arg asString))
	setSlot("..", method(arg, getSlot("self") asString .. arg asString))

	Map addKeysAndValues := method(keys, values, keys foreach(i, k, self atPut(k, values at(i))); self)
  
  /*doc Object slotDescriptionMap
  Returns raw map of slot names and short values' descriptions.
  See also <tt>Object slotSummary</tt>.
  */
	slotDescriptionMap := method(
		slotNames := getSlot("self") slotNames sort
		slotDescs := slotNames map(name, getSlot("self") getSlot(name) asSimpleString)
		Map clone addKeysAndValues(slotNames, slotDescs)
	)
  //doc Object apropos Prints out <tt>Protos Core</tt> slot descriptions.
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
  /*doc Object slotSummary 
  Returns a formatted <tt>slotDescriptionMap</tt>.
  <br/>
  <pre>
  Io> slotSummary
  ==>  Object_0x30c590:
    Lobby            = Object_0x30c590
    Protos           = Object_0x30c880
    exit             = method(...)
    forward          = method(...)
  </pre>
  */
  
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
  
  //doc Object asString Same as <tt>slotSummary</tt>.
	asString := getSlot("slotSummary")

  //doc Object asSimpleString Returns <type>_<uniqueHexId> string.
	asSimpleString := method(getSlot("self") type .. "_" .. getSlot("self") uniqueHexId)

	/*doc Object newSlot(slotName, aValue)
	Creates a getter and setter for the slot with the name slotName
	and sets its default value to aValue. Returns self. For example,
	newSlot("foo", 1) would create slot named foo with the value 1 as well as a setter method setFoo().
	*/

	newSlot := method(name, value, doc,
		getSlot("self") setSlot(name, getSlot("value"))
		getSlot("self") setSlot("set" .. name asCapitalized,
			doString("method(" .. name .. " = call evalArgAt(0); self)"))
			//if(doc, getSlot("self") docSlot(name, doc))
		getSlot("value")
	)

	//doc Object launchFile(pathString) Eval file at pathString as if from the command line in its folder.
  //doc System launchPath Returns a pathComponent of the launch file.
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
	<pre>
	MyObject test // performs test
	MyObject ?test // performs test if MyObject has a slot named test
	</pre>
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
	  if(a, if(a detect(x, getSlot("x") isIdenticalTo(getSlot("self"))), return a), a = List clone)
	  a append(getSlot("self"))
	  self protos foreach(ancestors(a))
	  a
	)

	//doc Object isKindOf(anObject) Returns true if anObject is in the receiver's ancestors.

	isKindOf := method(anObject, getSlot("self") ancestors contains(getSlot("anObject")))

	/*doc Object super(aMessage)
	Sends the message aMessage to the receiver's proto with the context of self. Example:
	<pre>
	self test(1, 2)   // performs test(1, 2) on self
	super(test(1, 2)) // performs test(1, 2) on self proto but with the context of self
	</pre>
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
  For example:
  <pre>
  Dog := Mammal clone do(
    init := method(
  	  resend
    )
  )
  </pre>
	Calling Dog init will send an init method to Mammal, but using the Dog's context.
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

  /*doc Object uniqueHexId 
  Returns uniqueId in a hexadecimal form (with a "0x" prefix)
  <pre>
  Io> Object uniqueId
  ==> 3146784
  Io> Object uniqueHexId
  ==> 0x300420
  </pre>
  */
	uniqueHexId := method("0x" .. getSlot("self") uniqueId asString toBase(16))

  /*doc Object lazySlot(code) 
  Defines a slot with a lazy initialization code. 
  Code is run only once: the first time slot is accessed. 
  Returned value is stored in a regular slot.
  <br/>
  <pre>
  Io> x := lazySlot("Evaluated!" println; 17)
  Io> x
  Evaluated!
  ==> 17
  Io> x
  ==> 17
  Io> x
  ==> 17
  </pre>
  <br/>
  Another form is <tt>lazySlot(name, code)</tt>:
  <br/>
  <pre>
  Io> lazySlot("x", "Evaluated!" println; 17)
  Io> x
  Evaluated!
  ==> 17
  Io> x
  ==> 17
  Io> x
  ==> 17
  </pre>
  */
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
  
  /*doc Object foreachSlot(slotName, slotValue, code)
  Iterates over all the slots in a receiver. Provides slotValue (non-activated)
  along with slotName. Code is executed in context of sender. <tt>slotName</tt> and <tt>slotValue</tt>
  become visible in the receiver (no Locals created! Maybe, it is not the best decision).
  <br/>
  <pre>
  Io> thisContext foreachSlot(n, v, n println)
  Lobby
  Protos
  exit
  forward
  n
  v
  ==> false
  </pre>
  */
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
	Object deprecatedWarning := method(newName,
		writeln("Warning in ", call sender call message label, ": '", call sender call message name, "' is deprecated", if(newName, ".  Use '" .. newName .. "' instead.", " and will be removed from a later version."))
		self
	)
	
	//referenceIdForObject := method(obj, getSlot("obj") unqiueId)
	//objectForReferenceId := method(id, Collector objectWithUniqueId(id))
)
