Message do (
	isOperator := method(
		arguments size == 0 and PostProcess ops hasKey(name)
	)

	operatorPrecedence := method(
		arguments size == 0 or return nil
		PostProcess ops at(name)
	)
)

PostProcess := Object clone do (
	ops := Map clone do (
		op := getSlot("atPut")

		op("'",   0)
		op(".",   0)
		op("?",   0)
		op("(",   0)
		op(")",   0)

		op("^",   1)

		op("++",  2)
		op("--",  2)

		op("*",   3)
		op("/",   3)
		op("%",   3)

		op("+",   4)
		op("-",   4)

		op("<<",  5)
		op(">>",  5)

		op(">",   6)
		op("<",   6)
		op("<=",  6)
		op(">=",  6)

		op("==",  7)
		op("!=",  7)

		op("&",   8)

		op("|",   9)

		op("and", 10)
		op("&&",  10)

		op("or",  11)
		op("||",  11)

		op("..",  12)

		op("=",   13)
		op("+=",  13)
		op("-=",  13)
		op("*=",  13)
		op("/=",  13)
		op("%=",  13)
		op("&=",  13)
		op("^=",  13)
		op("|=",  13)
		op("<<=", 13)
		op(">>=", 13)
		op(":=",  13)
		op("<-",  13)
		op("<->", 13)
		op("->",  13)

		op("return",  14)

		op(",", 15)
		op(";", 15)

		maxLevel := 16

		leftOps := Map clone atPut(":=", "setSlot") atPut("=", "updateSlot")

		removeSlot("op")
	)

	Tail := Object clone appendProto(thisContext) do (
		newSlot("msg")
		newSlot("level", -1)

		attach := method(m,
			pushImpl(m)
			AttachTail clone setMsg(m) setLevel(level)
		)

		setNext := method(m,
			msg setNextMessage(m)
			AttachTail clone setMsg(m) setLevel(level)
		)

		finish := method(
			msg setAttachedMessage
			self
		)

		code := method(msg code)
	)

	AttachTail := Tail clone do (pushImpl := method(m, msg setAttachedMessage(m)))
	FirstArgTail := Tail clone do (pushImpl := method(m, msg setArguments(list(m))))
	NewTail := Tail clone do (pushImpl := method(m, msg = m))

	MessageStack := Object clone appendProto(thisContext) do (
		first := nil
		stack := nil
		last := nil

		init := method(
			first = NewTail clone setLevel(ops maxLevel)
			stack = list(first)
		)

		attach := method(msg,
			last = msg
			if (msg isOperator,
				level := msg operatorPrecedence
				while (stack last and stack last level <= level,
					stack pop finish
				)
				stack push(stack pop attach(msg))
				stack push(FirstArgTail clone setMsg(msg) setLevel(level))
			,
				stack push(stack pop attach(msg))
			)
			#dumpStack
		)

		nextMessage := method(
			m := last nextMessage
			tail := nil; while (stack last, tail = stack pop finish)
			tail setNext(m)
			if (tail msg != last,
				last setNextMessage
			)
			self init
			m
		)

		dumpStack := method(
			sep := block(sender sep = block(write(", ")))
			stack foreach(v,
				sep
				write(v msg ?name, ": ", v level)
			)
			writeln
		)

		code := method(first code)
	)

	postProcess := method(msg,
		rearrangeOperators(msg)
		rearrangeSetUpdateSlot(msg)
		msg
	)

	rearrangeOperators := method(msg,
		stackLoop(argsToProcess, m, msg,
			stack := MessageStack clone

			while (m,
				while (m,
					stack attach(m)

					argsToProcess appendSeq(m arguments)
					m = m attachedMessage
				)

				m = stack nextMessage
			)
		)
	)

	rearrangeSetUpdateSlot := method(msg,
		stackLoop(msgToProcess, m, msg,
			if (leftOp := ops leftOps at(m name),
				slotName := m attachedToMessage name
				slotNameMessage := Message clone setName("\"" .. slotName .. "\"") setCachedResult(slotName)
				newArgs := m arguments atInsert(0, slotNameMessage)
				m attachedToMessage setName(leftOp)
				m attachedToMessage setArguments(newArgs)
				m attachedToMessage setAttachedMessage(m attachedMessage)
				m attachedToMessage setNextMessage(m nextMessage)
			)

			m removeSlot("attachedToMessage")
			m removeSlot("previousMessage")

			msgToProcess appendSeq(m arguments)
			m attachedMessage and(
				m attachedMessage attachedToMessage := m
				msgToProcess append(m attachedMessage)
			)
			m nextMessage and(
				m nextMessage previousMessage := m
				msgToProcess append(m nextMessage)
			)
		)
	)

	stackLoop := block(
		stackName := thisMessage argAt(0) name
		indexName := thisMessage argAt(1) name
		stack := list(sender doMessage(thisMessage argAt(2)))
		body := thisMessage argAt(3)

		sender setSlot(stackName, stack)
		while(stack size > 0,
			sender setSlot(indexName, stack pop)
			sender doMessage(body)
		)
	)
)

// Helper stuff

List popFirst := method(
	item := first
	if(item, removeAt(0))
	item
)

Message do(
	addArg := method(m,
	self setArguments(self arguments append(m))
	)

	parse := method(tokens,
		tokens first isMessage ifNil(return)
	//writeln("parse ", tokens first name)
		m := Message clone
	m parseName(tokens)
	m parseArgs(tokens)
	m parseAttached(tokens)
	while(m parseNext(tokens), Nop)
	m
	)

	valueForToken := method(token,
	tokenType := token type
	tokenName := token name
	if (tokenType == "TriQuote", return tokenName exclusiveSlice(3, -3) unescape)
	if (tokenType == "MonoQuote", return tokenName exclusiveSlice(1, -1) unescape)
	if (tokenType == "Number" or tokenType == "HexNumber", return tokenName asNumber)
	nil
	)

	parseName := method(tokens,
		//writeln("parseName ", tokens first name, " ", tokens first type)
	token := tokens popFirst
	setName(token name)
		token isLiteral and setCachedResult(valueForToken(token))
	)

	parseArgs := method(tokens,
	if (tokens first type != "OpenParen", return)
	tokens popFirst
	//writeln("parseArgs ", tokens first name)

	while(arg := parse(tokens),
		addArg(arg)
		if (tokens first type != "Comma", break)
		tokens popFirst
	)

	if (tokens first type != "CloseParen", Exception raise("Message", "missing close paren"))
	tokens popFirst
	)


	parseAttached := method(tokens,
		tokens first ?isMessage or return
	//writeln("parseAttached ", tokens first name)
	m := parse(tokens)
	if (m,
		setAttachedMessage(m)
	)
	)

	parseNext := method(tokens,
	if (tokens first type == "Terminator",
		tokens popFirst
		m := parse(tokens)
		setNextMessage(m)
		m
	)
	nil
	)
)

Token := Object clone do(
	isMessage := method(
	t := self type
	t == "Identifier" or t == "Operator" or t == "MonoQuote" or t == "TriQuote" or t == "Number" or t == "HexNumber"
	)
	isLiteral := method(
	t := self type
	t == "MonoQuote" or t == "TriQuote" or t == "Number" or t == "HexNumber"
	)
)

/*
Compiler do(
	_messageForString := getSlot("messageForString")
	messageForString := method(text,
		tokens := tokensForString(text)

		write("tokens: ")
		tokens foreach(t, write("[", t name, "] "))
		write("\n")

		tokens foreach(t, t appendProto(Token))
		tokens parens := List clone
		m := Message parse(tokens)
			post := PostProcess clone postProcess(m clone)

		writeln("parsing: ", text)
		writeln("parsed:  ", m code asMutable replaceSeq("\n", " "))
			writeln("post:    ", post code asMutable replaceSeq("\n", " "))
			writeln
		post
	)
)
*/
