AVCodec do(
	//doc AVCodec codecName Returns name of audio or video codec.
	//doc AVCodec setCodecName(aSeq) Sets the codec name. Returns self. See: encodeCodecNames and decodeCodecNames
	codecName ::= "mp2"

	//doc AVCodec inputCoro Returns coroutine currently writing to the receiver.
	//doc AVCodec setInputCoro(aCoro) Private method for setting inputCoro. Returns self.
	
	inputCoro ::= nil

	//doc AVCodec internalCoro Returns coroutine used for the AVCodec to process data...
	//doc AVCodec setInternalCoro(aCoro) Private method for setting internalCoro. Returns self.
	internalCoro ::= nil
	
	//doc AVCodec streamDestination  Returns the streamDestination.
/*doc AVCodec setStreamDestination(anObject) 
Sets the streamDestination to anObject. The didProcess method will call:
<pre>
if(streamDestination, streamDestination write(outputBuffer))
outputBuffer empty
</pre>
*/
	streamDestination ::= AudioDevice
	
	//doc AVCodec path Returns path to file the receiver is reading or writing to.
	//doc AVCodec setPath(aSeq) Sets the path to file the receiver is reading or writing to. Returns self.
	path ::= nil
	
	/*
	setPath := method(path,
		self path := path
		setVideoCodecName(path pathExtension)
	)
	
	//doc AVCodec videoCodecName Returns the name of the video codec.
	videoCodecName ::= nil
	*/

	/*doc AVCodec willProcess
	Called before the receiver will process more of the input buffer. 
	If inputBuffer is empty, it pauses the receiver's coro.
	*/
	willProcess := method(
		//writeln("AVCodec willProcess")
		if(inputBuffer isEmpty,
			//writeln("AVCodec inputBuffer empty - pausing internalCoro")
			setInternalCoro(Coroutine currentCoroutine)
			internalCoro pause
		)
	)

	/*doc AVCodec didProcess
	Called after the receiver processes some more of the input buffer.
	*/
	didProcess := method(
		//writeln("AVCodec didProcess")
		if(streamDestination, streamDestination write(outputBuffer))
		outputBuffer empty
		inputCoro ?resumeLater
	)

	/*doc AVCodec write(aSequence)
	Appends aSequence to the input buffer and resumes the receiver's coroutine in order to process it.
	The calling coroutine (inputCoroutine) will be scheduled to resume when didProcess is called.
	*/
	write := method(data,
		inputBuffer appendSeq(data)
		setInputCoro(Coroutine currentCoroutine)
		//writeln("AVCodec write - resuming internalCoro")
		internalCoro resume // this will pause the current coro
	)
)
