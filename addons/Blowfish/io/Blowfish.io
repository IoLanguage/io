Blowfish do(
	newSlot("key", nil)
	newSlot("inputBuffer", Sequence clone)
	newSlot("outputBuffer", Sequence clone)

	init := method(
		resend
		self inputBuffer := self inputBuffer clone
		self outputBuffer := self outputBuffer clone
	)

	encrypt := method(s,
		outputBuffer empty
		setIsEncrypting(true)
		beginProcessing
		inputBuffer copy(s)
		process
		endProcessing
		outputBuffer
	)

	decrypt := method(s,
		outputBuffer empty
		setIsEncrypting(false)
		beginProcessing
		inputBuffer copy(s)
		process
		endProcessing
		outputBuffer
	)
)
