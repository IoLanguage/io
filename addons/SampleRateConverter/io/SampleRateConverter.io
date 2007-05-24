
SampleRateConverter do(
	newSlot("streamDestination")
	
	write := method(data,
		//setOutputToInputRatio(44100 / call sender sampleRate) 
		inputBuffer appendSeq(data)
		process
		if(streamDestination, 
			streamDestination write(outputBuffer)
			outputBuffer empty
		)
	)
)


