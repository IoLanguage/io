

AudioDevice do(
	write := method(data,
		asyncWrite(data)
		//writeln("AudioDevice write yielding until data is needed")
		while(needsData not, yield)
		//writeln("AudioDevice data needed")
	)
)

AudioMixer := Object clone do(
	init := method(
		self sources := List clone
		self mixed := Seq clone
	)

	newSlot("streamDestination", AudioDevice)
	newSlot("processedSamples", 0)
	newSlot("isRunning", false)

	appendSource := method(source, sources append(source))
	removeSource := method(source, sources remove(source))

	process := method(sampleCount,
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
	)

	start := method(
		setIsRunning(true)
		while(isRunning, process(22050)) // 1/16th of a second
	)

	stop := method(
		setIsRunning(false)
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
