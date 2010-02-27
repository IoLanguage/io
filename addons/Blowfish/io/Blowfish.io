Blowfish do(
	key ::= nil
	inputBuffer  ::= Sequence clone
	//doc Blowfish inputBuffer Returns the input buffer.
	outputBuffer ::= Sequence clone
	//doc Blowfish outputBuffer Returns the output buffer.

	init := method(
		resend
		self inputBuffer  := self inputBuffer clone
		self outputBuffer := self outputBuffer clone
	)


	encrypt := method(s,
		//doc Blowfish encrypt(aSequence) Returns an encrypted version of aSequence.
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
		//doc Blowfish decrypt(aSequence) Returns an decrypted version of aSequence.
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
