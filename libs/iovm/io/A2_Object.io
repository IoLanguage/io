/*
nil ioDoc(
  docCopyright("Steve Dekorte", 2002)
  docLicense("BSD revised")
  docObject("nil")
  docDescription("nil is a singleton object that is used as a placeholder and to mean false in Io.")
)

nil do(
    docSlot("clone", "returns self since nil is a singleton.")
	clone := nil

	docSlot("and(expression)", "Returns nil without evaluating expression.")
	setSlot("and", nil)

	elseif := Object getSlot("if")

	docSlot("then(expression)", "Returns nil without evaluating expression.")
	setSlot("then", nil)

	docSlot("else(expression)", "Returns nil without evaluating expression.")
	setSlot("else", method(v, v))

	docSlot("or(anObject)", "Returns anObject if anObject is not nil. Otherwise returns nil.")
	setSlot("or", method(v, if(v, v, nil)))

	docSlot("print", "Prints 'nil'. Returns self.")
	print := method(write("nil"))

	setSlot("==", method(v, self isIdenticalTo(v)))
	setSlot("!=", method(v, self isIdenticalTo(v) not))

	docSlot("isNil", "Returns Lobby.")
	isNil := Lobby

	docSlot("ifNil(expression)", "Evaluates message.")
	ifNil := method(v, v)  
)
*/

// if(a == 1) then(b) elseif(b == c) then(d) else(f)
// (a == 1) ifTrue(b) ifFalse(c)

true do(
        then    := Object getSlot("evalArgAndReturnNil")
        elseif  := true
        else    := true
	ifTrue  := Object getSlot("evalArgAndReturnSelf")
	ifFalse := true
	setSlot("and", Object getSlot("evalArg"))
	setSlot("or", true)
	asString := "true"
	asSimpleString := "true"
	not := false
	clone := true
)

false do(
        then    := false
	ifTrue  := false
	ifFalse := Object getSlot("evalArgAndReturnSelf")
        elseif  := Object getSlot("if")
        else := Object getSlot("evalArgAndReturnNil")
	setSlot("and", false)
	setSlot("or", Object getSlot("evalArg"))
	
	type := "false"
	asString := "false"
	asSimpleString := "false"
	not := true
	clone := false
)

nil do(
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

    docsSlot := method(
	   if(getSlot("self") hasLocalSlot("docs"), 
	   getSlot("self") docs, 
	   getSlot("self") docs := Object clone do(slots := Object clone)
	   )
    )
    
    
    docSlot := method(k, v, 
    /*
        entry := Object clone 
        entry description := v asSymbol
        if (k containsSeq("("), 
            entry args := k afterSeq("(") beforeSeq(")") split(",") map(strip)
        )	
        getSlot("self") docsSlot slots setSlot(k beforeSeq("("), entry)
    */
    nil
    )
        
    docCopyright := method(v, 
	   k := call message name asMutable removePrefix("doc") makeFirstCharacterLowercase
	   getSlot("self") docsSlot setSlot(k, v)
    )
    docCopyright := nil //    turnOffDocs
 
    docLicense := getSlot("docCopyright")
    docObject := getSlot("docCopyright")
    docDescription := getSlot("docCopyright")
    docCredits := getSlot("docCopyright")
    docInclude := getSlot("docCopyright")
    docDependsOn := getSlot("docCopyright")
    docCategory := getSlot("docCopyright")

    ioDoc := method(
	   if(call argAt(0), getSlot("self") doMessage(call argAt(0)))
    )
    
    ioDoc := nil
    
    docSlot("list(...)", "Returns a List containing the arguments.")
    list := method(call message argsEvaluatedIn(call sender))
    
    docSlot("..(arg)", ".. is an alias for: method(arg, self asString append(arg asString))")
    setSlot("..", method(arg, getSlot("self") asString .. arg asString))
    
    Map addKeysAndValues := method(keys, values, keys foreach(i, k, self atPut(k, values at(i))); self)
    
    slotDescriptionMap := method(
        slotNames := getSlot("self") slotNames sort 
        slotDescs := slotNames map(name, getSlot("self") getSlot(name) asSimpleString)
        Map clone addKeysAndValues(slotNames, slotDescs) 
    )
    
    slotSummary := method(
        if(getSlot("self") type == "Block", return getSlot("self") asSimpleString)
        s := Sequence clone
        s appendSeq(" ", getSlot("self") asSimpleString, ":\n")
        slotDescriptions := slotDescriptionMap
        slotDescriptions keys sortInPlace foreach(k,
            s appendSeq("  ", k alignLeft(16), " = ", slotDescriptions at(k), "\n")
        )
        s
    )
    
    asString := getSlot("slotSummary")
    
    asSimpleString := method(getSlot("self") type .. "_" .. getSlot("self") uniqueHexId)
    
    docSlot("newSlot(slotName, aValue)", 
    """Creates a getter and setter for the slot with the name slotName 
    and sets it's default the value aValue. Returns self. For example, 
    newSlot("foo", 1) would create slot named foo with the value 1 as well as a setter method setFoo().""")

    newSlot := method(name, value, doc,
		getSlot("self") setSlot(name, value)
		getSlot("self") setSlot("set" .. name asCapitalized, 
			doString("method(" .. name .. " = call evalArgAt(0); self)"))
			if(doc, getSlot("self") docSlot(name, doc))
		value
    )
    
    docSlot("launchFile(pathString)", 
    "Eval file at pathString as if from the command line in it's folder.")
    
    launchFile := method(path, args,
        args ifNil(args = List clone)
        Lobby args := args
        Lobby launchPath :=  path pathComponent
        Directory setCurrentWorkingDirectory(Lobby launchPath)
        self doFile(path)
    )
    
    docSlot("println", "Same as print, but also prints a new line. Returns self.")
    println := method(self print; write("\n"); self)

    docSlot("?(aMessage)", """
    description: Sends the message aMessage to the receiver if it can respond to it. Example:
    <pre>
    MyObject test // performs test
    MyObject ?test // performs test if MyObject has a slot named test
    </pre>
    The search for the slot only follows the receivers proto chain. 
    """)

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

    docSlot("ancestors", 
    "Returns a list of all of the receiver's ancestors as found by recursively following the protos links.") 

    ancestors := method(a, 
        if(a, if(a detect(x, x isIdenticalTo(self)), return a), a = List clone)
        a append(self)
        self protos foreach(ancestors(a))
        a
    )

    docSlot("isKindOf(anObject)", "Returns true if anObject is in the receiver's ancestors.") 

    isKindOf := method(anObject, getSlot("self") ancestors contains(getSlot("anObject")))
    
    docSlot("super(aMessage)", 
    """Sends the message aMessage to the receiver's proto with the context of self. Example:
    <pre>
    self test(1, 2)   // performs test(1, 2) on self
    super(test(1, 2)) // performs test(1, 2) on self proto but with the context of self
    </pre>
    """)

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

    docSlot("resend", 
    """Send the message used to activate the current method to the Object's proto.
    For example;
    <pre>
    Dog := Mammal clone do(
	init := method(
	    resend
	)
    )
    </pre>
    calling Dog init will send an init method to Mammal, but using the Dog's context.
    """)

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


    docSlot("list(...)", "Returns a List containing the arguments.")
    list := method(call message argsEvaluatedIn(call sender))

    Object print := method(write(getSlot("self") asString); getSlot("self"))

    docSlot("println", "Same as print, but also prints a new line. Returns self.")
    println := method(getSlot("self") print; write("\n"); getSlot("self"))

    docSlot("in(aList)", "Same as: aList contains(self)")
    in := method(aList, aList contains(self)) 

	uniqueHexId := method("0x" .. getSlot("self") uniqueId asString toBase(16))
	
	lazySlot := method(name,
		m := (("self setSlot(\"" .. name .. "\", " .. call message argAt(1) code .. ")") asMessage)
        self setSlot(name, method(1) setMessage(m))
        nil
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

	docSlot("switch", """
	Execute an expression depending on the value of the caller. (This is an equivalent to C switch/case)
	<pre>
	hour := Date hour switch(
		12, "midday",
		0, "midnight",
		17, "teatime",
		Date hour asString
	)
	</pre>
	""")
	switch := method(
		for(couple, 0, call argCount - 2, 2, 
			if(call evalArgAt(couple) == self, 
				return call relayStopStatus(call evalArgAt(couple + 1))
			)
		)
		call relayStopStatus(call evalArgAt(call argCount - 1))
	)
)

Lobby args := method(System args)
