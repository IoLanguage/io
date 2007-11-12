
LibSndFile do(
	newSlot("path")
	newSlot("sampleRate")
	newSlot("channels")
	newSlot("format")
)

Sound := Object clone do(
	newSlot("path")
	newSlot("buffer")
	newSlot("sampleRate")
	newSlot("channels")
	newSlot("format")
	load := method(
		lsf := LibSndFile clone
		setBuffer(lsf outputBuffer)
		lsf setPath(path)
		lsf openForReading
		while(lsf read(16384), nil)
		setSampleRate(lsf sampleRate)
		setChannels(lsf channels)
		lsf close
		self
	)
)
