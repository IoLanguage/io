OperatorTable do(
	addOperator := method(symbol, precedence,
		precedence = precedence ifNilEval(0)
		operators atPut(symbol, precedence)
		self
	)

	addAssignOperator := method(symbol, messageName,
		assignOperators atPut(symbol, messageName)
		self
	)

	asString := method(
		s := Sequence clone appendSeq(OperatorTable asSimpleString, ":\n")

		s appendSeq("Operators")
		OperatorTable operators values unique sort foreach(precedence,
			s appendSeq("\n  ", precedence asString alignLeft(4), OperatorTable operators select(k, v, v == precedence) keys sort join(" "))
		)

		s appendSeq("\n\nAssign Operators")
		OperatorTable assignOperators keys sort foreach(symbol,
			name := OperatorTable assignOperators at(symbol)
			s appendSeq("\n  ", symbol alignLeft(4), name)
		)

		s appendSeq("\n\n")
		s appendSeq("To add a new operator: OperatorTable addOperator(\"+\", 4) and implement the + message.\n")
		s appendSeq("To add a new assign operator: OperatorTable addAssignOperator(\"=\", \"updateSlot\") and implement the updateSlot message.\n")

		s
	)

	reverseAssignOperators := method(assignOperators reverseMap)
)

# Make the lookup path shorter for the opShuffle. IoMessage_opShuffle looks up
# the OperatorTable object on the first message before starting shuffling.
Message OperatorTable := OperatorTable
