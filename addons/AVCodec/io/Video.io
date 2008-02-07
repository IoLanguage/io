Video := AVCodec clone do(
	//doc Video frameNumber The current frame number.
	//doc Video setFrameNumber(aNumber) Private setter for frameNumber. Returns self.
	frameNumber ::= 0
	
	//doc Video audioOn Whether or not the decoded audio is directed to the AudioDevice.
	//doc Video setAudioOn(aBool) Sets whether or not the decoded audio is directed to the AudioDevice. Returns self.
	audioOn ::= true
	
	isDone ::= false

	init := method(
		self image := Image clone
	)

	/*doc Video readNextFrame Decodes the next video frame into the receiver's image object. 
	Returns the image object if successful or nil otherwise.
	*/
	readNextFrame := method(
		if(self isAtEnd, open; return nil)

		while(frames size == 0,
			if(decode == nil, writeln("breaking"); break)
		)

		if(audioOn and audioSampleRate == 44100 and audioChannels == 2,
			AudioDevice asyncWrite(audioOutputBuffer)
			audioOutputBuffer empty
		)

		frameNumber = frameNumber + 1
		frame := frames removeFirst
		if(frame, image setDataWidthHeightComponentCount(frame, videoSize x, videoSize y, 3); image, nil)
	)
)
