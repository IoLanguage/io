

AudioDevice do(
	/*doc AudioDevice write(aSeq) Writes aSeq to the inputBuffer and yields until the 
	receiver is ready for more input data. Returns self.
	*/
	write := method(data,
		asyncWrite(data)
		//writeln("AudioDevice write yielding until data is needed")
		while(needsData not, yield)
		//writeln("AudioDevice data needed")
		self
	)
)

AudioMixer := Object clone do(
	//metadoc AudioMixer module PortAudio
	//metadoc AudioMixer category Media
	//metadoc AudioMixer description A minimal audio mixer.
	init := method(
		self sources := List clone
		self mixed := Seq clone
	)

	streamDestination ::= AudioDevice
	processedSamples ::= 0
	isRunning ::= false

	appendSource := method(source, sources append(source))
	removeSource := method(source, sources remove(source))

	process := method(sampleCount,
		//doc AudioMixer process(sampleCount) Internal method used for processing a chunk of the input sources. Returns self.
		byteCount := sampleCount * 8
		mixed setSize(byteCount)
		mixed zero
		sources foreach(source,
			while(source outputBuffer < byteCount, source read)
			mixed float32ArrayAdd(source outputBuffer)
			source outputBuffer removeSlice(0, byteCount)
		)
		mixed float32ArrayMultiplyByScalar(sources size)
		streamDestination write(mixed)
		processedSamples = processedSamples + sampleCount
		self
	)

	start := method(
		/*doc AudioMixer start Start the mixer loop processing 1/64th of a second chunks
		by calling process(22050) in a loop.
		Will not return until stop is called. Returns self.
		*/
		setIsRunning(true)
		while(isRunning, process(22050)) // 1/16th of a second
		self
	)

	stop := method(
		//doc AudioMixer stop Stops the mixer if it is running.
		setIsRunning(false)
		self
	)
)


AudioMixer2 := Object clone do(
	init := method(
		self buffers := List clone
		self mixed := Sequence clone setItemType("float32")
	)

	newSlot("isRunning", false)

	play := method(buf, buffers append(buf clone setItemType("float32")))

	process := method(sampleCount,
		mixed setSize(sampleCount) zero

		r :=  buffers size
		ir = 1 / r

		buffers foreach(buffer,
			buf := buffer slice(0, sampleCount)
			buf *= r
			buffer removeSlice(0, sampleCount)

			mixed += buf
			if(buffer isEmpty, buffers remove(buffer))

		)
		mixed *= ir
		AudioDevice asyncWrite(mixed)
	)

	start := method(
		setIsRunning(true)
		while(isRunning, process(22050)) // 1/16th of a second
	)

	stop := method(
		setIsRunning(false)
	)
)
