
SampleRateConverter do(
	newSlot("streamDestination")
	
	write := method(data,
		setOutputToInputRatio(44100 / call sender sampleRate) 
		inputBuffer appendSeq(data)
		//writeln("SampleRateConverter process")
		process
		//writeln("SampleRateConverter process done")
		if(streamDestination, 
			streamDestination write(outputBuffer)
			outputBuffer empty
		)
	)
)


