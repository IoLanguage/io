Video := AVCodec clone do(
	newSlot("frameNumber", 0)
	newSlot("audioOn", false)
	isDone ::= false
	
	init := method(
	   self image := Image clone
	)
	
	readNextFrame := method(
        if(self isAtEnd, open; return nil)
        
	    while(frames size == 0,
            if(decode == nil, writeln("breaking"); break)
	    )
	    
        //if(audioOn and audioSampleRate == 44100 and audioChannels == 2,
            AudioDevice asyncWrite(audioOutputBuffer)
            audioOutputBuffer empty  
        //)
	    
        frameNumber = frameNumber + 1
        frame := frames removeFirst
        if(frame, image setDataWidthHeightComponentCount(frame, videoSize x, videoSize y, 3), nil)
	)
)
