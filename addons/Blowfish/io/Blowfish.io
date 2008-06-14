Blowfish do(
	key ::= nil
	inputBuffer  ::= Sequence clone
	outputBuffer ::= Sequence clone

	init := method(
		resend
		self inputBuffer  := self inputBuffer clone
		self outputBuffer := self outputBuffer clone
	)

	encrypt := method(s,
		outputBuffer empty
		setIsEncrypting(true)
		beginProcessing
		inputBuffer copy(s)
		process
		endProcessing
		inputBuffer empty
		outputBuffer
	)

	decrypt := method(s,
		outputBuffer empty
		setIsEncrypting(false)
		beginProcessing
		inputBuffer copy(s)
		process
		endProcessing
		inputBuffer empty
		outputBuffer
	)
)
