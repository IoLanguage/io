
AVCodec do(
	newSlot("codecName", "mp2")
	newSlot("inputCoro")
	newSlot("internalCoro")
	newSlot("streamDestination", AudioDevice)
	
	willProcess := method(
		//writeln("AVCodec willProcess")
		if(inputBuffer isEmpty, 
			//writeln("AVCodec inputBuffer empty - pausing internalCoro")
			setInternalCoro(Coroutine currentCoroutine)
			internalCoro pause
		)
	)

	didProcess := method(
		//writeln("AVCodec didProcess")
		if(streamDestination, streamDestination write(outputBuffer))
		outputBuffer empty
		inputCoro ?resumeLater
	)
	
	write := method(data,
		inputBuffer appendSeq(data)
		setInputCoro(Coroutine currentCoroutine)
		//writeln("AVCodec write - resuming internalCoro")
		internalCoro resume // this will pause the current coro
	)
)

