ZlibDecoder do(
	newSlot("inputBuffer", nil)
	newSlot("outputBuffer", nil)

	init := method(
	   setInputBuffer(Sequence clone)
	   setOutputBuffer(Sequence clone)
	)
)

Sequence unzip := method(
	z := ZlibDecoder clone
	z inputBuffer = self
	//writeln("z inputBuffer size = ", z inputBuffer size)
	z beginProcessing
	z process
	z endProcessing
	//writeln("z outputBuffer size = ", z outputBuffer size)
	self copy(z outputBuffer)
	self
)

Zlib := Object clone do(
	ZlibDecoder := ZlibDecoder
)
