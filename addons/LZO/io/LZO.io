
method(
m := message(
	newSlot("inputBuffer", nil)
	newSlot("outputBuffer", nil)

	init := method(
	   setInputBuffer(Sequence clone)
	   setOutputBuffer(Sequence clone)
	)
)

LZODecoder doMessage(m)
LZOEncoder doMessage(m)
) call

Sequence do(
	zCompress := method(
		z := LZOEncoder clone
		z inputBuffer = self
		z beginProcessing process endProcessing
		self copy(z outputBuffer)
		self
	)

	zUncompress := method(
		z := LZODecoder clone
		z inputBuffer = self
		z beginProcessing process endProcessing
		self copy(z outputBuffer)
		self
	)
)

LZO := Object clone do(
	LZODecoder := LZODecoder
	LZOEncoder := LZOEncoder
)
