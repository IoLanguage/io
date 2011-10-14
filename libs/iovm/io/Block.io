getSlot("Block") do(

	//doc Block asSimpleString Returns a short description of the receiver.
	asSimpleString := method(
		if(scope, "block", "method") .. "(" .. argumentNames append("...") join(", ") .. ")"
	)

	//doc Block asString Returns a full description of the receiver with decompiled code.
	asString := method(
		Formatter clone formatBlock(getSlot("self")) buffer
	)

	print := method(
		writeln
		writeln(asString)
	)

  	//doc Block callWithArgList(aList) Activates the block with the given argument list.
	callWithArgList := method(argList,
		getSlot("self") doMessage(argList asMessage setName("call"))
	)

  //doc Block println Same as <tt>print</tt>.
	println := getSlot("print")
    
  //doc Block Formatter Helper object for the <tt>asString</tt> method.
	Formatter := Object clone do(
		newSlot("lineNumber", 0)
		newSlot("isLineEmpty", true)
		newSlot("depth", 0)
		newSlot("buffer")

		newSlot("operators")
		newSlot("reverseAssignOperators")

		init := method(
			buffer = Sequence clone
			operators = OperatorTable operators
			reverseAssignOperators = OperatorTable reverseAssignOperators
			reverseAssignOperators atPut("setSlotWithType", ":=")
		)

		appendSeq := method(
			call delegateTo(buffer)
			isLineEmpty = false
		)

		newLine := method(
			buffer appendSeq("\n")
			lineNumber = lineNumber + 1
			isLineEmpty = true
		)

		newLinesTo := method(msg,
			(msg lineNumber - lineNumber) minMax(0, 2) repeat(newLine)
			lineNumber = msg lineNumber
		)

		indent := method(
			depth repeat(appendSeq("    "))
		)

		formatBlock := method(theBlock,
			msg := getSlot("theBlock") message

			if(msg label != CLI commandLineLabel,
				appendSeq("# " .. msg label .. ":" .. msg lineNumber, "\n")
			)
			appendSeq("method(")
			if(getSlot("theBlock") argumentNames size > 0,
				getSlot("theBlock") argumentNames foreach(i, name,
					if(i > 0,
						appendSeq(", ")
					)

					appendSeq(name)
				)

				appendSeq(", ")
			)

			newLine
			lineNumber = msg lineNumber
			formatIndentedMessage(msg)
			newLine
			appendSeq(")")
			self
		)

		formatIndentedMessage := method(msg,
			depth = depth + 1
			formatMessage(msg)
			depth = depth - 1
		)

		formatMessage := method(msg,
			m := msg
			while(m,
				if(m isEndOfLine,
					if(lineNumber == m next ?lineNumber,
						appendSeq(m name)
					)
					m = m next
					continue
				)

				newLinesTo(m)

				if(isLineEmpty,
					indent
				,
					if(m != msg,
						appendSeq(" ")
					)
				)

				if(reverseAssignOperators hasKey(m name)) then(
					args := m arguments

					if(args first cachedResult,
						appendSeq(args first cachedResult, " ")

						appendSeq(reverseAssignOperators at(m name))

						appendSeq(" ")
						if(args at(1),
							formatMessage(args at(1))
						)
					,
						appendSeq(m name)
						if(m argCount > 0,
							formatArguments(m)
						)
					)
				) elseif(operators hasKey(m name)) then(
					appendSeq(m name)

					appendSeq(" ")
					if(m arguments first,
						formatMessage(m arguments first)
					)
				) else(
					appendSeq(m name)
					if(m argCount > 0,
						formatArguments(m)
					)
				)

				m = m next
			)
		)

		formatArguments := method(msg,
			appendSeq("(")

			startingLineNumber := lineNumber
			msg arguments foreach(i, arg,
				# Insert separator between arguments
				if(i > 0,
					# Still on the same line as the "("
					if(startingLineNumber == lineNumber,
						appendSeq(", ")
					,
						newLine; indent; appendSeq(","); newLine
					)
				)

				# Format the argument
				formatIndentedMessage(arg)
			)

			if(startingLineNumber != lineNumber,
				newLine
				indent
			)
			appendSeq(")")
		)
	)
)
